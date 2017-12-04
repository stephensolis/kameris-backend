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

struct gpu_info {};

enum class stage_change_type { begin, end };
struct stage_change_info {
	stage_change_type change_type;

	unsigned stage_num;
	unsigned total_stages;

	std::string stage_name;
	double elapsed_time;
};

class executor {
 public:
	executor() = default;
	virtual void run(std::function<bool(confirmation_type, std::string /*message*/)> on_confirm_action,
		std::function<void(const run_options &, uint64_t /*est_memory*/, const gpu_info &)> on_run_start,
		std::function<void(const stage_change_info &)> on_stage_change,
		std::function<void(unsigned)> on_progress) const = 0;
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
