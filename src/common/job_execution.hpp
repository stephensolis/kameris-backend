#ifndef _COMMON_JOB_EXECUTION_
#define _COMMON_JOB_EXECUTION_

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <boost/variant.hpp>

#include "options_structs.hpp"
#include "run_options.hpp"

enum class confirmation_type { overwrite_resume, high_memory };
struct confirm_action_info {
	confirmation_type type;
	std::string message;
};

struct run_start_info {
	run_options options;
	uint64_t est_memory = 0;
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

using on_confirm_action_t = std::function<bool(const confirm_action_info &)>;
using on_run_start_t = std::function<void(const run_start_info &)>;
using on_stage_change_t = std::function<void(const stage_change_info &)>;
using on_progress_t = std::function<void(const progress_info &)>;

class executor {
 public:
	executor() = default;
	virtual void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start,
		on_stage_change_t on_stage_change, on_progress_t on_progress) const = 0;
	virtual ~executor() = default;

	//these should be handled on subclasses
	executor(const executor &) = delete;
	executor &operator=(const executor &) = delete;
	executor(executor &&) = delete;
	executor &operator=(executor &&) = delete;
};

std::unique_ptr<executor> execute_repr_jobs(const program_options &options, const std::vector<repr_options> &jobs);
std::unique_ptr<executor> execute_dist_jobs(const program_options &options, const std::vector<dist_options> &jobs);
std::unique_ptr<executor> execute_resume(const program_options &options);

#endif
