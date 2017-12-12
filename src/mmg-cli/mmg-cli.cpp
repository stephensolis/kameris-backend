#include <exception>
#include <iostream>
#include <memory>

#include "../common/job_execution.hpp"
#include "../common/version.hpp"

#include "args_parser.hpp"
#include "console_marks.hpp"
#include "exceptions.hpp"
#include "jobspec_parsers.hpp"
#include "progress_bar.hpp"
#include "usage_strings.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	ios::sync_with_stdio(false);

	if (argc == 2 && (argv[1] == "version"s || argv[1] == "--version"s)) {
		cout << "mmg-cli " VERSION_LONG << endl;
		return 0;
	}

	cout << "This is modmap-generator-cpp (mmg-cli) " VERSION_LONG "." << endl << endl;

	try {
		program_args args = parse_program_args(argc, argv);

		unique_ptr<executor> exec;
		switch (args.mode) {
			case program_mode::repr:
				exec = execute_repr_jobs(args.options, parse_repr_jobs(args.jobspecs));
				break;
			case program_mode::dist:
				exec = execute_dist_jobs(args.options, parse_dist_jobs(args.jobspecs));
				break;
			case program_mode::resume:
				exec = execute_resume(args.options);
				break;
			case program_mode::help:
				usage::put_help(cout);
				return 0;
		}

		exec->run(
			[](const confirm_action_info &info) {
				//on confirmation prompt
				//(code here)
				return true;
			},
			[](const run_start_info &info) {
				//on program run start
				//(code here)
			},
			[](const stage_change_info &info) {
				//on stage change
				//(code here)
			},
			[](const progress_info &info) {
				//on stage progress
				//(code here)
			});

		return 0;
	} catch (const already_handled_error &) {
		return 1;
	} catch (const exception &ex) {
		cerr << error_mark << "An unexpected error occurred: " << ex.what() << endl;
		return 1;
	}
}
