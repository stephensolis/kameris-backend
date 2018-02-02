#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <typeinfo>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/variant.hpp>

#include <libmmg/utils/parallel.hpp>
#include <mmg-formats/cpp/all_formats.hpp>

#include "checkpointer.hpp"
#include "defaults.hpp"
#include "filesystem_checks.hpp"
#include "job_execution.hpp"
#include "options_structs.hpp"
#include "run_args.hpp"

using namespace std;
using namespace mmg;
namespace fs = boost::filesystem;


Checkpointer setup_resume_file(const run_args &args, on_confirm_action_t on_confirm_action) {
	if (args.options.was_resumed) {
		fstream resume_file(args.options.resume_filename, ios::in | ios::out | ios::binary);
		return Checkpointer(std::move(resume_file), false);
	} else {
		if (fs::exists(fs::path(args.options.resume_filename))) {
			on_confirm_action({confirmation_type::overwrite_resume,
				"The resume file '" + args.options.resume_filename + "' already exists."});
		}

		fstream resume_file(args.options.resume_filename, ios::in | ios::out | ios::binary | ios::trunc);
		write_run_args(resume_file, args);
		return Checkpointer(std::move(resume_file), true);
	}
}


class repr_executor : public executor {
 protected:
	const vector<repr_options> _jobs;

	repr_executor(const run_options &options, vector<repr_options> jobs) : executor(options), _jobs(std::move(jobs)) {}
};

class cgrlike_repr_executor : public repr_executor {
 public:
	cgrlike_repr_executor(const run_options &options, const vector<repr_options> &jobs) : repr_executor(options, jobs) {
		check_directory_input(options.in1_path);
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//TODO: (code here)
	}
};

class descr_repr_executor : public repr_executor {
 public:
	descr_repr_executor(const run_options &options, const vector<repr_options> &jobs) : repr_executor(options, jobs) {
		check_file_input(options.in1_path);
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//open input file
		repr_reader input_reader(_options.in1_path);
		const repr_header &input_header = input_reader.header();

		//build run list
		vector<function<void(uint64_t)>> job_fns;
		vector<repr_writer> output_writers;
		for (const auto &variant_job_options : _jobs) {
			const auto &job_options = get<descr_options>(variant_job_options);

			string output_filename = "???";
			//output_writers.emplace_back((fs::path(_options.out_path) / fs::path(output_filename)).string(), repr_header{
			//
			//}, !_options.was_resumed);
		}

		//make checkpointer
		Checkpointer checkpointer = setup_resume_file({run_mode::repr, _options, _jobs}, on_confirm_action);

		//set up execution
		ParallelExecutor exec;
		vector<future<void>> futures;
		for (size_t i = 0; i < input_header.count; ++i) {
			futures.push_back(exec.enqueue([&, i](unsigned) {
				for (const auto &job_fn : job_fns) {
					job_fn(i);
				}
			}));
		}

		//run
		exec.execute(thread::hardware_concurrency());
		for (size_t i = 0; i < futures.size(); ++i) {
			futures[i].wait();
		}
	}
};


class dist_executor : public executor {
 protected:
	const vector<dist_options> _jobs;

	dist_executor(const run_options &options, vector<dist_options> jobs) : executor(options), _jobs(std::move(jobs)) {}
};

class general_dist_executor : public dist_executor {
 public:
	general_dist_executor(const run_options &options, const vector<dist_options> &jobs) : dist_executor(options, jobs) {
		check_file_input(options.in1_path);
		if (!options.in2_path.empty()) {
			check_file_input(options.in2_path);
		}
	}

	void run(on_confirm_action_t on_confirm_action, on_run_start_t on_run_start, on_stage_change_t on_stage_change,
		/**/ on_progress_t on_progress) const override {
		//TODO: (code here)
	}
};

class usm_dist_executor : public dist_executor {
 public:
	usm_dist_executor(const run_options &options, const vector<dist_options> &jobs) : dist_executor(options, jobs) {
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


unique_ptr<executor> make_executor(const run_args &args) {
	if (args.mode == run_mode::repr) {
		const auto &jobs = get<vector<repr_options>>(args.jobs);
		if (jobs.empty()) {
			throw invalid_argument("The list of jobs cannot be empty");
		}

		if (jobs.front().type() == typeid(descr_options)) {
			return make_unique<descr_repr_executor>(args.options, jobs);
		} else {
			return make_unique<cgrlike_repr_executor>(args.options, jobs);
		}
	} else if (args.mode == run_mode::dist) {
		const auto &jobs = get<vector<dist_options>>(args.jobs);
		if (jobs.empty()) {
			throw invalid_argument("The list of jobs cannot be empty");
		}

		if (jobs.front().type() == typeid(usm_options)) {
			return make_unique<usm_dist_executor>(args.options, jobs);
		} else {
			return make_unique<general_dist_executor>(args.options, jobs);
		}
	} else {
		throw invalid_argument("Invalid run mode");
	}
}

unique_ptr<executor> make_executor_for_resume(const run_options &start_opts) {
	fstream resume_file(start_opts.resume_filename, ios::in | ios::out | ios::binary);
	run_args args = read_run_args(resume_file);

	args.options.threads = start_opts.threads;
	args.options.blocksize = start_opts.blocksize;
	args.options.use_cuda = start_opts.use_cuda;
	args.options.use_opencl = start_opts.use_opencl;
	args.options.quiet = start_opts.quiet;
	args.options.was_resumed = true;
	args.options.resume_filename = start_opts.resume_filename;

	return make_executor(args);
}

unique_ptr<executor> executor::build(const run_args &args) {
	if (args.mode == run_mode::resume) {
		return make_executor_for_resume(args.options);
	} else {
		return make_executor(args);
	}
}
