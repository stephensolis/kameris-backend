#include <chrono>
#include <cstdint>
#include <fstream>
#include <ios>
#include <string>
#include <utility>

#include <mmg-formats/cpp/common/binary_io.hpp>
#include "checkpointer.hpp"

using namespace std;
using namespace mmg;

constexpr unsigned Checkpointer::update_rate;

void Checkpointer::rewind() {
	_file.seekg(-int(state_size), ios::end);
	_file.seekp(-int(state_size), ios::end);
}

void Checkpointer::write_state(const state &curr_state) {
	write_binary(_file, curr_state.stage_num);
	write_binary(_file, curr_state.stage_pos);
	_file.flush();
}

Checkpointer::Checkpointer(fstream &&resume_file, bool initialize_file) : _file(move(resume_file)) {
	if (initialize_file) {
		_file.seekp(0, ios::end);
		write_state({});
	}
}

Checkpointer::state Checkpointer::read_state() {
	rewind();

	state curr_state{};
	read_binary(_file, curr_state.stage_num);
	read_binary(_file, curr_state.stage_pos);

	return curr_state;
}

void Checkpointer::checkpoint(const state &curr_state) {
	chrono::steady_clock::time_point now = chrono::steady_clock::now();
	if (now - _last_write < chrono::milliseconds(update_rate)) {
		return;
	}
	_last_write = now;

	rewind();
	write_state(curr_state);
}
