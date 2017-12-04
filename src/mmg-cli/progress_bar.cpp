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
	curr_count += incr;

	steady_clock::time_point now = steady_clock::now();
	if (now - last_update < milliseconds(update_rate)) {
		return;
	}

	last_update = now;
	steady_clock::duration elapsed = now - start_time;
	auto eta = elapsed * (double(max_count - curr_count) / curr_count);

	auto bar_unfilled_chars = unsigned(bar_width * (double(max_count - curr_count) / max_count));
	unsigned bar_filled_chars = max(int(bar_width - bar_unfilled_chars) - 1, 0);

	cout << "\r" << msg //beginning message
		 << 100 * curr_count / max_count << "%" //percentage
		 << " [" << string(bar_filled_chars, '=') << ">" << string(bar_unfilled_chars, ' ') << "] " //progress bar
		 << curr_count << "/" << max_count //progress
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
