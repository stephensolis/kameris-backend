#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <typeinfo>
#include <utility>
#include <vector>

#include "checkpointer.hpp"
#include "filesystem_checks.hpp"
#include "job_execution.hpp"
#include "options_structs.hpp"

using namespace std;

class repr_executor : public executor {
 private:
	const program_options options;
	const vector<repr_options> jobs;

 public:
	repr_executor(program_options options, vector<repr_options> jobs)
		: options(std::move(options)), jobs(std::move(jobs)) {}
	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

class dist_executor : public executor {
 private:
	const program_options options;
	const vector<dist_options> jobs;

 public:
	dist_executor(program_options options, vector<dist_options> jobs)
		: options(std::move(options)), jobs(std::move(jobs)) {}
	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

unique_ptr<executor> execute_repr_jobs(const program_options &options, const vector<repr_options> &jobs) {
	assert(!jobs.empty());

	//check input format
	if (jobs.front().type() == typeid(descr_options)) {
		check_file_input(options.in1_path);
	} else {
		check_directory_input(options.in1_path);
	}

	return make_unique<repr_executor>(options, jobs);
}

unique_ptr<executor> execute_dist_jobs(const program_options &options, const vector<dist_options> &jobs) {
	assert(!jobs.empty());

	//check input format
	if (jobs.front().type() == typeid(usm_options)) {
		check_directory_input(options.in1_path);
		if (!options.in2_path.empty()) {
			check_directory_input(options.in2_path);
		}
	} else {
		check_file_input(options.in1_path);
		if (!options.in2_path.empty()) {
			check_file_input(options.in2_path);
		}
	}

	return make_unique<dist_executor>(options, jobs);
}

unique_ptr<executor> execute_resume(const program_options &options) {
	//(code here)
	return nullptr;
}
