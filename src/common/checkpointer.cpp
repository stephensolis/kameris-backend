#include <chrono>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <utility>

#include <mmg-formats/cpp/common/raw_binary_io.hpp>
#include <mmg-formats/cpp/common/storage_encoding.hpp>

#include "checkpointer.hpp"

using namespace std;
using namespace mmg;

constexpr unsigned Checkpointer::update_rate;

void Checkpointer::rewind() {
	size_t state_size = -int(sizeof state().stage_num + sizeof state().stage_pos);
	file.seekg(state_size, ios::end);
	file.seekp(state_size, ios::end);
}

void Checkpointer::write_state(const state &curr_state) {
	write_binary_raw(file, to_storage_encoding(curr_state.stage_num));
	write_binary_raw(file, to_storage_encoding(curr_state.stage_pos));
	file.flush();
}

Checkpointer::Checkpointer(fstream &&resume_file, bool initialize_file) : file(move(resume_file)) {
	if (initialize_file) {
		file.seekp(0, ios::end);
		write_state({});
	}
}

Checkpointer::state Checkpointer::read_state() {
	rewind();

	state curr_state{};
	read_binary_raw(file, curr_state.stage_num);
	curr_state.stage_num = from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_num);
	read_binary_raw(file, curr_state.stage_pos);
	curr_state.stage_pos = from_storage_encoding<decltype(curr_state.stage_num)>(curr_state.stage_pos);

	return curr_state;
}

void Checkpointer::checkpoint(const state &curr_state) {
	chrono::steady_clock::time_point now = chrono::steady_clock::now();
	if (now - last_write < chrono::milliseconds(update_rate)) {
		return;
	}
	last_write = now;

	rewind();
	write_state(curr_state);
}
