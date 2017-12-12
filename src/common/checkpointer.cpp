#include <chrono>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>

#include <libmmg/io/binary.hpp>
#include <libmmg/io/storage_encoding.hpp>

#include "checkpointer.hpp"

using namespace std;
using namespace mmg;

void Checkpointer::rewind() {
	resume_file.seekp(-int(sizeof state().stage_num + sizeof state().stage_pos), ios::end);
}

void Checkpointer::init_new(const string &filename) {
	resume_file.open(filename, ios::binary | ios::trunc);
}

Checkpointer::state Checkpointer::init_existing(const string &filename) {
	resume_file.open(filename, ios::binary);
	rewind();

	state curr_state{};
	read_binary_raw(resume_file, curr_state.stage_num);
	curr_state.stage_num = from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_num);
	read_binary_raw(resume_file, curr_state.stage_pos);
	curr_state.stage_pos = from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_pos);

	return curr_state;
}

void Checkpointer::checkpoint(const state &curr_state) {
	chrono::steady_clock::time_point now = chrono::steady_clock::now();
	if (now - last_write < chrono::milliseconds(update_rate)) {
		return;
	}
	last_write = now;

	write_binary_raw(resume_file, to_storage_encoding(curr_state.stage_num));
	write_binary_raw(resume_file, to_storage_encoding(curr_state.stage_pos));

	resume_file.flush();
	rewind();
}
