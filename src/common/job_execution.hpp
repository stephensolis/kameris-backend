#ifndef _COMMON_JOB_EXECUTION_
#define _COMMON_JOB_EXECUTION_

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <boost/variant.hpp>

#include "options_structs.hpp"
#include "run_args.hpp"

enum class confirmation_type { overwrite_resume, high_memory };
struct confirm_action_info {
	confirmation_type type;
	std::string message;
};

struct run_start_info {
	run_args args;

	std::string in1_description;
	std::string in2_description;

	uint64_t est_memory_use = 0;
};

enum class stage_change_type { begin, end };
struct stage_change_info {
	stage_change_type change_type;

	unsigned stage_num;
	unsigned total_stages;
	unsigned total_progress;

	std::string stage_name;
	double elapsed_time;
};

struct progress_info {
	unsigned current_progress;
};

using on_confirm_action_t = std::function<void(const confirm_action_info &)>;
using on_run_start_t = std::function<void(const run_start_info &)>;
using on_stage_change_t = std::function<void(const stage_change_info &)>;
using on_progress_t = std::function<void(const progress_info &)>;

class executor {
 protected:
	const run_options _options;

	explicit executor(run_options options) : _options(std::move(options)) {}

 public:
	virtual ~executor() = default;

	virtual void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start,
		on_stage_change_t on_stage_change, on_progress_t on_progress) const = 0;

	static std::unique_ptr<executor> build(const run_args &args);

	//these should be handled on subclasses
	executor(const executor &) = delete;
	executor &operator=(const executor &) = delete;
	executor(executor &&) = delete;
	executor &operator=(executor &&) = delete;
};

#endif
