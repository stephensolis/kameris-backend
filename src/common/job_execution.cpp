#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <typeinfo>
#include <utility>
#include <vector>

#include <boost/variant.hpp>

#include "checkpointer.hpp"
#include "filesystem_checks.hpp"
#include "job_execution.hpp"
#include "options_structs.hpp"
#include "run_options.hpp"

using namespace std;

class repr_executor : public executor {
 private:
	const program_options options;
	const vector<repr_options> jobs;
	const Checkpointer::state start_state;

 public:
	repr_executor(program_options in_options, vector<repr_options> in_jobs, Checkpointer::state in_start_state = {})
			: options(std::move(in_options)), jobs(std::move(in_jobs)), start_state(in_start_state) {
		assert(!jobs.empty());

		//check input format
		if (jobs.front().type() == typeid(descr_options)) {
			check_file_input(options.in1_path);
		} else {
			check_directory_input(options.in1_path);
		}
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

class dist_executor : public executor {
 private:
	const program_options options;
	const vector<dist_options> jobs;
	const Checkpointer::state start_state;

 public:
	dist_executor(program_options in_options, vector<dist_options> in_jobs, Checkpointer::state in_start_state = {})
			: options(std::move(in_options)), jobs(std::move(in_jobs)), start_state(in_start_state) {
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
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

unique_ptr<executor> executor_build_resume(const program_options &start_opts) {
	fstream resume_file(start_opts.in1_path, ios::in | ios::out | ios::binary);
	run_options run_opts = read_run_options(resume_file);

	run_opts.options.threads = start_opts.threads;
	run_opts.options.blocksize = start_opts.blocksize;
	run_opts.options.use_cuda = start_opts.use_cuda;
	run_opts.options.use_opencl = start_opts.use_opencl;
	run_opts.options.quiet = start_opts.quiet;

	Checkpointer checkpointer(move(resume_file));
	Checkpointer::state start_state = checkpointer.read_state();

	switch (run_opts.mode) {
		case run_mode::repr:
			return make_unique<repr_executor>(run_opts.options, get<vector<repr_options>>(run_opts.jobs), start_state);
		case run_mode::dist:
			return make_unique<dist_executor>(run_opts.options, get<vector<dist_options>>(run_opts.jobs), start_state);
		default:
			throw invalid_argument("Invalid run mode, the resume file may be corrupt");
	}
}

unique_ptr<executor> executor::build(const run_options &options) {
	switch (options.mode) {
		case run_mode::repr:
			return make_unique<repr_executor>(options.options, get<vector<repr_options>>(options.jobs));
		case run_mode::dist:
			return make_unique<dist_executor>(options.options, get<vector<dist_options>>(options.jobs));
		case run_mode::resume:
			return executor_build_resume(options.options);
		default:
			throw invalid_argument("Invalid run mode");
	}
}
