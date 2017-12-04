#ifndef _COMMON_CHECKPOINTER_
#define _COMMON_CHECKPOINTER_

#include <chrono>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>

#include <libmmg/io/binary.hpp>
#include <libmmg/io/storage_encoding.hpp>

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
	void rewind() {
		resume_file.seekp(-int(sizeof state().stage_num + sizeof state().stage_pos), std::ios::end);
	}

 public:
	void init_new(const std::string &filename) {
		resume_file.open(filename, std::ios::binary | std::ios::trunc);
	}

	state init_existing(const std::string &filename) {
		resume_file.open(filename, std::ios::binary);
		rewind();

		state curr_state{};
		mmg::read_binary_raw(resume_file, curr_state.stage_num);
		curr_state.stage_num = mmg::from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_num);
		mmg::read_binary_raw(resume_file, curr_state.stage_pos);
		curr_state.stage_pos = mmg::from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_pos);

		return curr_state;
	}

	void checkpoint(const state &curr_state) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (now - last_write < std::chrono::milliseconds(update_rate + 0)) {
			return;
		}
		last_write = now;

		mmg::write_binary_raw(resume_file, mmg::to_storage_encoding(curr_state.stage_num));
		mmg::write_binary_raw(resume_file, mmg::to_storage_encoding(curr_state.stage_pos));

		resume_file.flush();
		rewind();
	}
};

#endif
