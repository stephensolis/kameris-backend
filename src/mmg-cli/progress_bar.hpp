#ifndef _CLI_PROGRESS_BAR_
#define _CLI_PROGRESS_BAR_

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

class ProgressBar {
 private:
	const std::string _message;
	uint64_t _curr_count = 0;
	const uint64_t _max_count;
	const std::chrono::steady_clock::time_point _start_time = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point _last_update;

 public:
	explicit ProgressBar(uint64_t max_count, std::string message = "")
			: _message(std::move(message)), _max_count(max_count) {}
	~ProgressBar();

	void increment(uint64_t incr = 1);

	//these write to the console, so multiple simultaneous instances are not a good idea
	ProgressBar(const ProgressBar &) = delete;
	ProgressBar &operator=(const ProgressBar &) = delete;

	//just to avoid extra destructor logic
	ProgressBar(ProgressBar &&) = delete;
	ProgressBar &operator=(ProgressBar &&) = delete;
};

#endif
