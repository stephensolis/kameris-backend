#include <exception>
#include <iostream>
#include <memory>

#include "../common/job_execution.hpp"
#include "../common/version.hpp"

#include "args_parser.hpp"
#include "console_marks.hpp"
#include "exceptions.hpp"
#include "progress_bar.hpp"
#include "usage_strings.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	ios::sync_with_stdio(false);

	if (argc == 2 && (argv[1] == "version"s || argv[1] == "--version"s || argv[1] == "-v"s)) {
		cout << "mmg-cli " VERSION_LONG << endl;
		return 0;
	}

	cout << "This is modmap-generator-cpp (mmg-cli) " VERSION_LONG "." << endl << endl;

	if (argc == 2 && (argv[1] == "help"s || argv[1] == "--help"s || argv[1] == "-h"s)) {
		usage::put_help(cout);
		return 0;
	}

	try {
		run_options options = parse_run_options(argc, argv);

		executor::build(options)->run(
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
