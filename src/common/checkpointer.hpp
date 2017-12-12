#ifndef _COMMON_CHECKPOINTER_
#define _COMMON_CHECKPOINTER_

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>

class Checkpointer {
 public:
	struct state {
		uint8_t stage_num;
		uint32_t stage_pos;
	};

 private:
	std::fstream resume_file;
	std::chrono::steady_clock::time_point last_write;

	static const unsigned update_rate = 2000;
	void rewind();

 public:
	void init_new(const std::string &filename);
	state init_existing(const std::string &filename);
	void checkpoint(const state &curr_state);
};

#endif
