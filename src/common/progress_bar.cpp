#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

#include "progress_bar.hpp"

using namespace std;
using namespace std::chrono;

using days = duration<int, ratio_multiply<ratio<24>, hours::period>>;

const unsigned bar_width = 35;
const unsigned update_rate = 1000;

void ProgressBar::increment(uint64_t incr) {
	_curr_count += incr;

	steady_clock::time_point now = steady_clock::now();
	if (now - _last_update < milliseconds(update_rate)) {
		return;
	}

	_last_update = now;
	steady_clock::duration elapsed = now - _start_time;
	auto eta = elapsed * (double(_max_count - _curr_count) / _curr_count);

	auto bar_unfilled_chars = unsigned(bar_width * (double(_max_count - _curr_count) / _max_count));
	unsigned bar_filled_chars = max(int(bar_width - bar_unfilled_chars) - 1, 0);

	cout << "\r" << _message //beginning message
		 << 100 * _curr_count / _max_count << "%" //percentage
		 << " [" << string(bar_filled_chars, '=') << ">" << string(bar_unfilled_chars, ' ') << "] " //progress bar
		 << _curr_count << "/" << _max_count //progress
		 << " ETA:";

	if (eta >= days(1)) {
		cout << duration_cast<days>(eta).count() << "d";
	}
	if (eta >= hours(1)) {
		cout << duration_cast<hours>(eta).count() % 24 << "h";
	}
	if (eta >= minutes(1)) {
		cout << duration_cast<minutes>(eta).count() % 60 << "m";
	}
	cout << duration_cast<seconds>(eta).count() % 60 << "s";

	cout << string(10, ' ') << flush;
}

ProgressBar::~ProgressBar() {
	cout << "\r" << string(100, ' ') << "\r" << flush;
}
