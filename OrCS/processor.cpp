#include "simulator.hpp"

btb_table_t btb_table;
opcode_package_t last_instruction;
uint64_t total_branches = 0;
bool op_branch = false;
uint64_t right_predict = 0;
uint64_t wrong_predict = 0;
uint64_t total_taken = 0;
uint64_t total_ntaken = 0;
int prediction_type;
bool predicted;

PiecewiseLinearPredictor plp;

int _MAX_VAL = (int) (pow(2,WEIGHT_WIDTH-1)); // upper bound 
int _MIN_VAL = (-1)*(int) pow(2,WEIGHT_WIDTH-1); // lower bound 
int theta = (int)(floor(2.14*(HIST_LENGTH + 1)) + 20.58);

inline uint64_t btb_idx(uint64_t pc) {
	return (pc & 1023);
}

bool predict(uint64_t pc) {
	int idx = pc & (ADDR_RANGE-1);
	//output is initialized to bias weight
	plp.output = plp.weight[idx][0][0];
	// //sum weights (or their negations) chosen using
	// //the addresses of the last GHL branches
	int j = 0;
	for (int i = 0; i < HIST_LENGTH; i++)
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
	predicted = true ? (plp.output >= 0) : false;

	return predicted;
}

void update_weights(bool taken, uint64_t pc) {
	int idx = pc & (ADDR_RANGE-1);

	if ((abs(plp.output) < theta) || predicted != taken)
	{
		if (taken) {
			if(plp.weight[idx][0][0] < _MAX_VAL) {// upper bound check to go up 
				plp.weight[idx][0][0] += 1;
			}
		}
		else {
			if (plp.weight[idx][0][0] > _MIN_VAL) {// lower bound check to go down  
				plp.weight[idx][0][0] -= 1;
			}
		}

		int j = 0;
		for (int i = 0; i < HIST_LENGTH; i++) {
			j = plp.GA[i] & (GA_RANGE-1);
			if (plp.GHR[i] == taken) {
				if(plp.weight[idx][j][i+1] < _MAX_VAL) {// upper bound check to go up 
					plp.weight[idx][j][i+1] += 1;
				}
			}
			else {
				if (plp.weight[idx][j][i+1] > _MIN_VAL) {// lower bound check to go down  
					plp.weight[idx][j][i+1] -= 1;
				}
			}
   		}
	}

    for (int i = HIST_LENGTH - 1; i > 0; i--) {
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
	this->clock_penalty = 0;
	this->miss = 0;
	this->hit = 0;
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
		total_ntaken++;
		//check if we predicted right
		if (prediction_type == NTAKEN) {
			right_predict++;
		} else {
			wrong_predict++;
		}
		btb_table.update_bht(idx, false);
		
	//check if was taken
	} else {
		total_taken++;
		//check if we predicted right
		if (prediction_type == TAKEN) {
			right_predict++;
		} else {
			wrong_predict++;
		}
		btb_table.update_bht(idx, true);
	}
}

bool check_prediction_piece_linear(uint64_t current_pc) {
	// check if was not-taken
	//if current pc is the pc target (last_instruction pc + pc size)
	if (current_pc == (last_instruction.opcode_address + last_instruction.opcode_size)) {
		total_ntaken++;
		//check if we predicted right
		if (!predicted) {
			right_predict++;
		} else {
			wrong_predict++;
		}
		return false;
	//check if was taken
	} else {
		total_taken++;
		//check if we predicted right
		if (predicted) {
			right_predict++;
		} else {
			wrong_predict++;
		}
		return true;
	}
}

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {
	btb_table.create_btb_table();
	for (int i = 0; i < HIST_LENGTH; ++i) {
		plp.GA[i] = 0;
		plp.GHR[i] = false;
	}

	// weight[ADDR_RANGE][GA_RANGE][HIST_LENGTH + 1]
	for (int i = 0; i < ADDR_RANGE; ++i) {
		for (int j = 0; j < GA_RANGE; ++j)
		{
			for (int k = 0; k < HIST_LENGTH + 1; ++k)
			{
				plp.weight[i][j][k] = 0;
			}
		}
	}
};

// =====================================================================
void processor_t::clock() {
	/// Get the next instruction from the trace
	opcode_package_t new_instruction;
	if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
		/// If EOF
		orcs_engine.simulator_alive = false;
	}

	//if last instruction was a branch, now we are sure of the branch path (if it was taken or not taken)
	if (op_branch) {
		op_branch = false;
		// uint64_t idx = btb_idx(last_instruction.opcode_address)*COLS;
		// for(int i = 0; i < COLS; ++i) {
		// 	if (last_instruction.branch_type == BRANCH_COND && btb_table.table[idx + i].pc_address == last_instruction.opcode_address) {
		// 		// check_prediction_2bc(idx + i, new_instruction.opcode_address);
		// 		check_prediction_piece_linear(new_instruction.opcode_address);
		// 		break;
		// 	}
		// }
		if (last_instruction.branch_type == BRANCH_COND) {
			// check_prediction_2bc(idx + i, new_instruction.opcode_address);
			bool taken = check_prediction_piece_linear(new_instruction.opcode_address);
			update_weights(taken, last_instruction.opcode_address);
		}
	}

	//check if instruction is a branch
	if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH) {
		total_branches++;
		op_branch = true;

		//save the current instruction to check in the next operation (next clock).
		last_instruction = new_instruction;
		predicted = predict(new_instruction.opcode_address);
		//check if pc code is in BTB table
		// if (btb_table.is_in_btb(new_instruction.opcode_address)) {
		// 	btb_table.hit++;
		// 	// prediction_type = branch_predictor_2bc(new_instruction);
		// 	predicted = predict(new_instruction.opcode_address);

		// } else {
		// 	btb_table.miss++;
		// 	//insert this pc code and needed information on BTB
		// 	btb_table.insert_btb_value(new_instruction.opcode_address, new_instruction.opcode_size);
		// }
	}
};

// =====================================================================
void processor_t::statistics() {
	// ORCS_PRINTF("######################################################\n");
	// ORCS_PRINTF("processor_t\n");
    ORCS_PRINTF("BTB_HIT: %d\n", btb_table.hit);
    ORCS_PRINTF("BTB_MISS: %d\n\n", btb_table.miss);
    ORCS_PRINTF("total taken: %ld\n", total_taken);
    ORCS_PRINTF("total not-taken: %ld\n", total_ntaken);
    ORCS_PRINTF("right predict: %ld\n", right_predict);
    ORCS_PRINTF("wrong predict: %ld\n\n", wrong_predict);
    printf("porcentage: %.2f\n", (float)right_predict/(total_taken+total_ntaken));
    ORCS_PRINTF("Total branches: %ld\n", total_branches);
};