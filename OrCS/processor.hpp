// ============================================================================
// ============================================================================

#define ENTRIES 1024
#define COLS 4

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
		uint8_t bht;
        uint64_t lru;
};

class btb_table_t {
    public:
        uint8_t clock_penalty;
        uint32_t hit, miss;
        btb_row_t table[ENTRIES*COLS];

        void create_btb_table();
        bool is_in_btb(uint64_t pc, uint64_t cycles);
        void create_btb_row(btb_row_t *row, uint64_t pc, uint32_t size, uint64_t cycles);
        void insert_btb_value(uint64_t pc, uint32_t size, uint64_t cycles);
};