#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <utility>
#include <vector>

#include <boost/variant.hpp>

#include "checkpointer.hpp"
#include "filesystem_checks.hpp"
#include "job_execution.hpp"
#include "options_structs.hpp"
#include "run_args.hpp"

using namespace std;


class repr_executor : public executor {
 protected:
	const run_options _options;
	const vector<repr_options> _jobs;
	const Checkpointer::state _start_state;

	repr_executor(run_options options, vector<repr_options> jobs, Checkpointer::state start_state = {})
			: _options(std::move(options)), _jobs(std::move(jobs)), _start_state(start_state) {}
};

class cgrlike_repr_executor : public repr_executor {
 public:
	cgrlike_repr_executor(
		const run_options &options, const vector<repr_options> &jobs, const Checkpointer::state &start_state = {})
			: repr_executor(options, jobs, start_state) {
		check_directory_input(options.in1_path);
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

class descr_repr_executor : public repr_executor {
 public:
	descr_repr_executor(
		const run_options &options, const vector<repr_options> &jobs, const Checkpointer::state &start_state = {})
			: repr_executor(options, jobs, start_state) {
		check_file_input(options.in1_path);
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};


class dist_executor : public executor {
 protected:
	const run_options _options;
	const vector<dist_options> _jobs;
	const Checkpointer::state _start_state;

	dist_executor(run_options options, vector<dist_options> jobs, Checkpointer::state start_state = {})
			: _options(std::move(options)), _jobs(std::move(jobs)), _start_state(start_state) {}
};

class general_dist_executor : public dist_executor {
 public:
	general_dist_executor(
		const run_options &options, const vector<dist_options> &jobs, const Checkpointer::state &start_state = {})
			: dist_executor(options, jobs, start_state) {
		check_file_input(options.in1_path);
		if (!options.in2_path.empty()) {
			check_file_input(options.in2_path);
		}
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//(code here)
	}
};

class usm_dist_executor : public dist_executor {
 public:
	usm_dist_executor(
		const run_options &options, const vector<dist_options> &jobs, const Checkpointer::state &start_state = {})
			: dist_executor(options, jobs, start_state) {
		check_directory_input(options.in1_path);
		if (!options.in2_path.empty()) {
			check_directory_input(options.in2_path);
		}

		throw logic_error("USM distance is not implemented yet");
	}

	void run(on_confirm_action_t /**/, on_run_start_t /**/, on_stage_change_t /**/, on_progress_t /**/) const override {
		throw logic_error("USM distance is not implemented yet");
	}
};


unique_ptr<executor> make_executor(const run_args &args, const Checkpointer::state &start_state = {}) {
	if (args.mode == run_mode::repr) {
		const auto &jobs = get<vector<repr_options>>(args.jobs);
		if (jobs.empty()) {
			throw std::invalid_argument("The list of jobs cannot be empty");
		}

		if (jobs.front().type() == typeid(descr_options)) {
			return make_unique<descr_repr_executor>(args.options, jobs, start_state);
		} else {
			return make_unique<cgrlike_repr_executor>(args.options, jobs, start_state);
		}
	} else if (args.mode == run_mode::dist) {
		const auto &jobs = get<vector<dist_options>>(args.jobs);
		if (jobs.empty()) {
			throw std::invalid_argument("The list of jobs cannot be empty");
		}

		if (jobs.front().type() == typeid(usm_options)) {
			return make_unique<usm_dist_executor>(args.options, jobs, start_state);
		} else {
			return make_unique<general_dist_executor>(args.options, jobs, start_state);
		}
	} else {
		throw invalid_argument("Invalid run mode");
	}
}

unique_ptr<executor> make_executor_for_resume(const run_options &start_opts) {
	fstream resume_file(start_opts.in1_path, ios::in | ios::out | ios::binary);
	run_args args = read_run_args(resume_file);

	args.options.threads = start_opts.threads;
	args.options.blocksize = start_opts.blocksize;
	args.options.use_cuda = start_opts.use_cuda;
	args.options.use_opencl = start_opts.use_opencl;
	args.options.quiet = start_opts.quiet;

	Checkpointer checkpointer(std::move(resume_file));
	Checkpointer::state start_state = checkpointer.read_state();

	return make_executor(args, start_state);
}

unique_ptr<executor> executor::build(const run_args &args) {
	if (args.mode == run_mode::resume) {
		return make_executor_for_resume(args.options);
	} else {
		return make_executor(args);
	}
}
