// ============================================================================
// ============================================================================

#include <cmath>
#include <time.h>

/*-----Defines for the BTB and two bit counter-----*/
/*-------------------------------------------------*/
#define BTB_ON 1
#define ENTRIES 1024
#define COLS 4
#define START_BHT 1
#define TAKEN 1
#define NTAKEN 0
/*-------------------------------------------------*/


#define GH_LENGTH 20
#define ADDR_RANGE 256 //Value of N
#define GA_RANGE 256 //Value of M

class processor_t {
    private:    
    
    
    public:

		// ====================================================================
		/// Methods
		// ====================================================================
		processor_t();
	    void allocate();
	    void clock();
	    void statistics();
};

class btb_row_t {
	public:
		uint64_t pc_address;
		uint64_t pc_size;
		int bht;
        uint64_t lru;
};

class btb_table_t {
    public:
        uint32_t hit, miss;
        uint64_t total_taken, total_ntaken;
        uint64_t right_dir_predict, wrong_dir_predict;
        uint64_t right_tgt_predict, wrong_tgt_predict;
        btb_row_t table[ENTRIES*COLS];

        void create_btb_table();
        bool is_in_btb(uint64_t pc);
        void create_btb_row(btb_row_t *row, uint64_t pc, uint32_t size);
        void insert_btb_value(uint64_t pc, uint32_t size);
        void update_bht(uint64_t idx, bool taken);
};


class piecewise_linear_predictor_t {
    public:
        uint64_t total_taken, total_ntaken;
        uint64_t right_dir_predict, wrong_dir_predict;
        bool GHR[GH_LENGTH]; //true is taken and false is not taken
        uint64_t GA[GH_LENGTH];
        int output;
        int weight[ADDR_RANGE][GA_RANGE][GH_LENGTH + 1];

        void creat_piecewise_instance();
        void predict(uint64_t pc);
        void update_weights_original(bool taken, uint64_t pc);
};

int branch_predictor_2bc(opcode_package_t instruction);
void check_prediction_2b(uint64_t idx, uint64_t current_pc);
bool check_prediction_piece_linear(uint64_t current_pc);