#include "simulator.hpp"

uint8_t clock_penalty;
opcode_package_t last_instruction;
uint64_t total_branches = 0;
bool op_branch = false;

uint64_t cond = 0;

time_t begin, end;

btb_table_t btb_table;
int prediction_type;

piecewise_linear_predictor_t plp;
bool predicted;

//value as in paper :)
double theta = 2.14*(GH_LENGTH + 1) + 20.58;

inline uint64_t btb_idx(uint64_t pc) {
	return (pc & 1023);
}

void piecewise_linear_predictor_t::creat_piecewise_instance() {
	this->total_taken = 0;
	this->total_ntaken = 0;
	this->right_dir_predict = 0;
	this->wrong_dir_predict = 0;
	for (int i = 0; i < GH_LENGTH; ++i) {
		this->GA[i] = 0;
		this->GHR[i] = false;
	}

	// weight[ADDR_RANGE][GA_RANGE][GH_LENGTH + 1]
	for (int i = 0; i < ADDR_RANGE; ++i) {
		for (int j = 0; j < GA_RANGE; ++j)
		{
			for (int k = 0; k < GH_LENGTH + 1; ++k)
			{
				this->weight[i][j][k] = 0;
			}
		}
	}
}

void piecewise_linear_predictor_t::predict(uint64_t pc) {
	predicted = false;
	int idx = pc & (ADDR_RANGE-1);
	//output is initialized to bias weight
	plp.output = plp.weight[idx][0][0];
	//sum weights (or their negations) chosen using
	//the addresses of the last GHL branches
	int j = 0;
	for (int i = 0; i < GH_LENGTH; i++)
    {
		j = plp.GA[i] & (GA_RANGE-1);
		//if taken
        if (plp.GHR[i]) {
            plp.output += plp.weight[idx][j][i+1];
		}
        else {
            plp.output -= plp.weight[idx][j][i+1];
		}
    }
	if (plp.output >= 0) {
		predicted = true;
	}
}

void piecewise_linear_predictor_t::update_weights_original(bool taken, uint64_t pc) {
	int idx = pc & (ADDR_RANGE-1);

	if ((abs(plp.output) < theta) || predicted != taken)
	{
		if (taken) {
			plp.weight[idx][0][0] += 1;
		}
		else { 
			plp.weight[idx][0][0] -= 1;
		}

		int j = 0;
		for (int i = 0; i < GH_LENGTH; i++) {
			j = plp.GA[i] & (GA_RANGE-1);
			if (plp.GHR[i] == taken) {
				plp.weight[idx][j][i+1] += 1;
			}
			else {
				plp.weight[idx][j][i+1] -= 1;
			}
   		}
	}

    for (int i = GH_LENGTH - 1; i > 0; i--) {
		plp.GA[i] = plp.GA[i-1];
		plp.GHR[i] = plp.GHR[i-1];
	}
    plp.GA[0] = pc;
    plp.GHR[0] = taken;
}

void btb_table_t::create_btb_table() {
	for(int i = 0; i < ENTRIES*COLS; ++i) {
		this->table[i].pc_address = -1LU;
		this->table[i].pc_size = 0;
		this->table[i].bht = START_BHT;
		this->table[i].lru = 0;
	}
	this->miss = 0;
	this->hit = 0;
	this->total_taken = 0;
	this->total_ntaken = 0;
	this->right_dir_predict = 0;
	this->wrong_dir_predict = 0;
}

bool btb_table_t::is_in_btb(uint64_t pc) {
	uint64_t idx = btb_idx(pc)*COLS;
	for(int i = 0; i < COLS; ++i) {
		if (this->table[idx + i].pc_address == pc) {
			this->table[idx + i].lru = orcs_engine.get_global_cycle();
			return true;
		}
	}
	return false;
}

void btb_table_t::create_btb_row(btb_row_t *row, uint64_t pc, uint32_t size) {
	row->pc_address = pc;
	row->pc_size = size;
	row->lru = orcs_engine.get_global_cycle();
	row->bht = START_BHT;
}

void btb_table_t::insert_btb_value(uint64_t pc, uint32_t size) {
	uint64_t lru = orcs_engine.get_global_cycle();
	uint64_t min_idx = 0;
	uint64_t idx = btb_idx(pc)*COLS;
	for(int i = 0; i < COLS; ++i) {
		//check if this position is empty. if yes, can save directly
		if (this->table[idx + i].pc_address == -1LU) {
			create_btb_row(&(this->table[idx + i]), pc, size);
			return;
		//if not empty, check the pc cycle (when that pc was saved) and save the minimal cycle (the older)
		} else if (this->table[idx + i].lru <= lru) {
			lru = this->table[idx + i].lru;
			min_idx = idx + i;
		}
	}
	this->table[min_idx].pc_address = pc;
	this->table[min_idx].pc_size = size;
	this->table[min_idx].lru = orcs_engine.get_global_cycle();
}

int branch_predictor_2bc(opcode_package_t instruction) {
	uint64_t idx = btb_idx(instruction.opcode_address)*COLS;
	for(int i = 0; i < COLS; ++i) {
		if (btb_table.table[idx + i].pc_address == instruction.opcode_address) {
			//if bht indicates taken, update with the pc_size and update the status
			if (btb_table.table[idx + i].bht == 2 || btb_table.table[idx + i].bht == 3) {
				return TAKEN;
			}
		}
	}
	return NTAKEN;
}

void btb_table_t::update_bht(uint64_t idx, bool taken) {
	if (!taken)
	{
		if (this->table[idx].bht > 0) {
			this->table[idx].bht -= 1;
		}
	}
	else
	{
		if (this->table[idx].bht < 3) {
			this->table[idx].bht += 1;
		}
	}
}

void check_prediction_2bc(uint64_t idx, uint64_t current_pc) {
	// check if was not-taken
	//if current pc is the pc target (last_instruction pc + pc size)
	if (current_pc == (last_instruction.opcode_address + last_instruction.opcode_size)) {
		btb_table.total_ntaken++;
		//check if we predicted right
		if (prediction_type == NTAKEN) {
			btb_table.right_dir_predict++;
		} else {
			btb_table.wrong_dir_predict++;
			clock_penalty = 16;
		}
		btb_table.update_bht(idx, false);
		
	//check if was taken
	} else {
		btb_table.total_taken++;
		//check if we predicted right
		if (prediction_type == TAKEN) {
			btb_table.right_dir_predict++;
		} else {
			btb_table.wrong_dir_predict++;
			clock_penalty = 16;
		}
		btb_table.update_bht(idx, true);
	}
}

bool check_prediction_piece_linear(uint64_t current_pc) {
	// check if was not-taken
	//if current pc is the pc target (last_instruction pc + pc size)
	if (current_pc == (last_instruction.opcode_address + last_instruction.opcode_size)) {
		plp.total_ntaken++;
		//check if we predicted right
		if (!predicted) {
			plp.right_dir_predict++;
		} else {
			plp.wrong_dir_predict++;
			clock_penalty = 16;
		}
		return false;
	//check if was taken
	} else {
		plp.total_taken++;
		//check if we predicted right
		if (predicted) {
			plp.right_dir_predict++;
		} else {
			plp.wrong_dir_predict++;
			clock_penalty = 16;
		}
		return true;
	}
}

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {
	time(&begin);
	/*-----This block is for the BTB and two bit counter-----*/
	if (BTB_ON)
	{
		btb_table.create_btb_table();
	/*-----This block is for the Piecewise Linear Branch Prediction-----*/
	} else {
		plp.creat_piecewise_instance();
	}
};

// =====================================================================
void processor_t::clock() {
	/// Get the next instruction from the trace
	opcode_package_t new_instruction;
	if (clock_penalty > 0) {
		clock_penalty--;
		return;
	}

	if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
		/// If EOF
		orcs_engine.simulator_alive = false;
		time(&end);
	}

	//if last instruction was a branch, now we are sure of the branch path (if it was taken or not taken)
	if (op_branch && last_instruction.branch_type == BRANCH_COND) {
		op_branch = false;

		/*-----This block is for the BTB and two bit counter-----*/
		if (BTB_ON)
		{
			uint64_t idx = btb_idx(last_instruction.opcode_address)*COLS;
			for(int i = 0; i < COLS; ++i) {
				if (btb_table.table[idx + i].pc_address == last_instruction.opcode_address) {
					cond++;
					check_prediction_2bc(idx + i, new_instruction.opcode_address);
					break;
				}
			}
		/*-----This block is for the Piecewise Linear Branch Prediction-----*/
		} else {
			bool taken = check_prediction_piece_linear(new_instruction.opcode_address);
			plp.update_weights_original(taken, last_instruction.opcode_address);
		}


	}

	//check if instruction is a branch
	if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH) {
		total_branches++;
		op_branch = true;

		//save the current instruction to check in the next operation (next clock).
		last_instruction = new_instruction;


		/*-----This block is for the BTB and two bit counter-----*/
		if (BTB_ON)
		{
			// check if pc code is in BTB table
			if (btb_table.is_in_btb(new_instruction.opcode_address)) {
				btb_table.hit++;
				prediction_type = branch_predictor_2bc(new_instruction);

			} else {
				btb_table.miss++;
				clock_penalty = 16;
				//insert this pc code and needed information on BTB
				btb_table.insert_btb_value(new_instruction.opcode_address, new_instruction.opcode_size);
			}
		/*-----This block is for the Piecewise Linear Branch Prediction-----*/
		} else {
			plp.predict(new_instruction.opcode_address);
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");
	/*-----This block is for the BTB and two bit counter-----*/
	/*-------------------------------------------------------*/
	if (BTB_ON)
	{
		ORCS_PRINTF("-----Statistics for BTB and 21 bit counter-----\n");
		ORCS_PRINTF("BTB hits: %d\n", btb_table.hit);
		ORCS_PRINTF("BTB misses: %d\n\n", btb_table.miss);
		ORCS_PRINTF("Total taken: %ld\n", btb_table.total_taken);
		ORCS_PRINTF("Total not-taken: %ld\n\n", btb_table.total_ntaken);
		// ORCS_PRINTF("Total conditional: %ld\n\n", cond);
		ORCS_PRINTF("Right predict: %ld\n", btb_table.right_dir_predict);
		ORCS_PRINTF("Wrong predict: %ld\n\n", btb_table.wrong_dir_predict);
		printf("Percentage: %.4f%% \n", ((float)btb_table.right_dir_predict/(btb_table.total_taken+btb_table.total_ntaken))*100);
	} else {
		ORCS_PRINTF("-----Statistics for Piecewise Linear Branch Prediction------\n");
		ORCS_PRINTF("Total taken: %ld\n", plp.total_taken);
		ORCS_PRINTF("Total not-taken: %ld\n\n", plp.total_ntaken);
		// ORCS_PRINTF("Total conditional: %ld\n\n", cond);
		ORCS_PRINTF("Right predict: %ld\n", plp.right_dir_predict);
		ORCS_PRINTF("Wrong predict: %ld\n\n", plp.wrong_dir_predict);
		printf("Percentage: %.4f%% \n", ((float)plp.right_dir_predict/(plp.total_taken+plp.total_ntaken))*100);

		// printf("%.4f,%ld\n", ((float)plp.right_dir_predict/(plp.total_taken+plp.total_ntaken))*100, (end - begin));

	}

    ORCS_PRINTF("Total branches: %ld\n", total_branches);
	ORCS_PRINTF("Total time: %lds\n", end - begin);
	ORCS_PRINTF("-------------------------------------------------------\n");
};