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

		inline bool is_initial() const {
			return stage_num == 0;
		}
	};
	static constexpr size_t state_size = sizeof state().stage_num + sizeof state().stage_pos;

 private:
	std::fstream _file;
	std::chrono::steady_clock::time_point _last_write;

	static constexpr unsigned update_rate = 2000;
	void rewind();
	void write_state(const state &curr_state);

 public:
	explicit Checkpointer(std::fstream &&resume_file, bool initialize_file = false);
	state read_state();
	void checkpoint(const state &curr_state);
};

#endif
