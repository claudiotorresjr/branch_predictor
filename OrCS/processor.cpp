#include "simulator.hpp"

btb_table_t btb_table;
opcode_package_t last_instruction;
uint64_t total_branches = 0;
bool op_branch = false;

inline uint64_t btb_idx(uint64_t pc) {
	return (pc & 1023);
}

void btb_table_t::create_btb_table() {
	for(int i = 0; i < ENTRIES*COLS; ++i) {
		this->table[i].pc_address = -1;
		this->table[i].pc_size = 0;
		this->table[i].bht = 0;
		this->table[i].lru = 0;
	}
	this->clock_penalty = 0;
	this->miss = 0;
	this->hit = 0;
}

bool btb_table_t::is_in_btb(uint64_t pc, uint64_t cycles) {
	// printf("indice_%ld - pc_address_%ld ", btb_idx(pc), pc);
	for(int i = 0; i < COLS; ++i) {
		if (this->table[btb_idx(pc)*COLS + i].pc_address == pc) {
			this->table[btb_idx(pc)*COLS + i].lru = cycles;
			// printf("HIT\n");
			return true;
		}
	}
	// printf("AMISS\n");
	return false;
}

void btb_table_t::create_btb_row(btb_row_t *row, uint64_t pc, uint32_t size, uint64_t cycles) {
	row->pc_address = pc;
	row->pc_size = size;
	row->lru = cycles;
	row->bht = 0;
}

void btb_table_t::insert_btb_value(uint64_t pc, uint32_t size, uint64_t cycles) {
	uint64_t lru = cycles;
	uint64_t min_idx = 0;
	for(int i = 0; i < COLS; ++i) {
		//check if this position is empty. if yes, can save directly
		if (this->table[btb_idx(pc)*COLS + i].pc_address == -1LU) {
			create_btb_row(&(this->table[btb_idx(pc)*COLS + i]), pc, size, cycles);
			return;
		//if not empty, check the pc cycle (when that pc was saved) and save the 
		//minimal cycle (the older)
		} else if (this->table[btb_idx(pc)*COLS + i].lru < lru) {
			lru = this->table[btb_idx(pc)*COLS + i].lru;
			min_idx = btb_idx(pc)*COLS + i;
		}
	}
	this->table[min_idx].pc_address = pc;
	this->table[min_idx].pc_size = size;
	this->table[min_idx].lru = cycles;
}

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {
	btb_table.create_btb_table();
};

// =====================================================================
void processor_t::clock() {
	/// Get the next instruction from the trace
	opcode_package_t new_instruction;
	if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
		/// If EOF
		orcs_engine.simulator_alive = false;
	}

	//if last instruction was a branch, now we are sure of the branch path
	//(if it was taken or not taken)
	// if (op_branch) {
	// 	op_branch = false;
	// 	uint64_t old_pc = last_instruction.opcode_address;
	// 	uint32_t old_size = last_instruction.opcode_size;
	// 	for(int i = 0; i < COLS; ++i) {
	// 		if (btb_table.table[btb_idx(old_pc)*COLS + i].pc_address == old_pc) {
	// 			//check if the branch was not taken (current pc == old pc + old pc size)
	// 			if (new_instruction.opcode_address == old_pc + old_size) {
	// 				//btb_table.table[btb_idx(old_pc)*COLS + i].bht = 
	// 			// check if was taken
	// 			} else {

	// 			}
	// 		}
	// 	}
	// 	//last_instruction = 0;
	// 	//check the 
	// }

	//check if instruction is a branch
	if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH) {
		total_branches++;
		op_branch = true;
		//save the current instruction to check in the next operation (next clock).
		last_instruction = new_instruction;
		//check if pc code is in BTB table
		if (btb_table.is_in_btb(new_instruction.opcode_address, orcs_engine.get_global_cycle())) {
			btb_table.hit++;
		} else {
			btb_table.miss++;
			//insert this pc code and needed information on BTB
			btb_table.insert_btb_value(new_instruction.opcode_address, new_instruction.opcode_size, orcs_engine.get_global_cycle());
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	// ORCS_PRINTF("######################################################\n");
	// ORCS_PRINTF("processor_t\n");
    ORCS_PRINTF("BTB_HIT: %d\n", btb_table.hit);
    ORCS_PRINTF("BTB_MISS: %d\n", btb_table.miss);
    ORCS_PRINTF("Total branches: %ld\n", total_branches);
};