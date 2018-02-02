#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

#include "../common/job_execution.hpp"
#include "../common/version.hpp"

#include "args_parser.hpp"
#include "console_marks.hpp"
#include "exceptions.hpp"
#include "progress_bar.hpp"
#include "usage_strings.hpp"

using namespace std;

bool confirm_prompt() {
	while (true) {
		cout << " Continue? [Y/N]: ";
		string answer;
		getline(cin, answer);

		if (answer == "Y" || answer == "y") {
			return true;
		} else if (answer == "N" || answer == "n") {
			return false;
		} else {
			cout << "Invalid response.";
		}
	}
}

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

	run_args args;
	try {
		args = parse_run_args(argc, argv);

		executor::build(args)->run(
			[&](const confirm_action_info &info) {
				//on confirmation prompt
				cout << warning_mark << info.message;
				if (args.options.quiet) {
					cout << " Continuing anyway." << endl;
				} else if (!confirm_prompt()) {
					exit(0);
				}
			},
			[](const run_start_info &info) {
				//on program run start
				//TODO: (code here)
			},
			[](const stage_change_info &info) {
				//on stage change
				//TODO: (code here)
			},
			[](const progress_info &info) {
				//on stage progress
				//TODO: (code here)
			});

		return 0;
	} catch (const already_handled_error &) {
		return 1;
	} catch (const exception &ex) {
		cerr << error_mark << "An unexpected error occurred: " << ex.what() << endl;
		if (args.options.was_resumed) {
			cerr << error_mark
				 << "This error may be caused by a corrupt resume file. Try deleting the resume file and trying again."
				 << ex.what() << endl;
		}
		return 1;
	}
}
