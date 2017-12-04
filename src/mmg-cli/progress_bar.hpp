#ifndef _CLI_PROGRESS_BAR_
#define _CLI_PROGRESS_BAR_

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

class ProgressBar {
 private:
	const std::string msg;
	uint64_t curr_count, max_count;
	std::chrono::steady_clock::time_point start_time, last_update;

 public:
	explicit ProgressBar(uint64_t max_count, std::string message = "")
		: msg(std::move(message)), curr_count(0), max_count(max_count), start_time(std::chrono::steady_clock::now()) {}
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
