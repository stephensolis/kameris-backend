#include <exception>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <string>

#include <boost/program_options.hpp>

#include "../common/defaults.hpp"
#include "../common/filesystem_checks.hpp"
#include "../common/output_headers.hpp"

#include "args_parser.hpp"
#include "console_marks.hpp"
#include "exceptions.hpp"
#include "usage_strings.hpp"

namespace po = boost::program_options;
using namespace std;

void check_option_present(const po::variables_map &args, const string &opt_name, const string &opt_display_name = "") {
	if (!args.count(opt_name)) {
		throw invalid_argument("The option '" + (opt_display_name.empty() ? opt_name : opt_display_name) + //
			"' is required but missing");
	}
}

void check_option_absent(const po::variables_map &args, const string &mode, const string &opt_name,
	/**/ const string &opt_display_name = "", const string &extra_message = "") {
	if (args.count(opt_name) && !args[opt_name].defaulted()) {
		throw invalid_argument("The option '" + (opt_display_name.empty() ? opt_name : opt_display_name) + //
			"' is invalid for mode '" + mode + "'" + (extra_message.empty() ? "" : " " + extra_message));
	}
}

void check_value_positive(int value, const string &opt_display_name) {
	if (value <= 0) {
		throw invalid_argument("The argument ('" + to_string(value) + "') for option '" + opt_display_name + //
			"' is invalid");
	}
}

element_type parse_int_precision(const string &precision_str) {
	if (precision_str == "8") {
		return element_type::uint8;
	} else if (precision_str == "16") {
		return element_type::uint16;
	} else if (precision_str == "32") {
		return element_type::uint32;
	} else if (precision_str == "64") {
		return element_type::uint64;
	} else if (precision_str == "default") {
		return defaults::int_precision;
	} else {
		throw invalid_argument("The argument ('" + precision_str + "') for option 'int-precision' is invalid");
	}
}

element_type parse_float_precision(program_mode mode, const string &precision_str) {
	if (precision_str == "single") {
		return element_type::float32;
	} else if (precision_str == "double") {
		return element_type::float64;
	} else if (precision_str == "default") {
		if (mode == program_mode::repr) {
			return defaults::repr_float_precision;
		} else if (mode == program_mode::dist) {
			return defaults::dist_float_precision;
		} else {
			throw logic_error("Unknown program mode value");
		}
	} else {
		throw invalid_argument("The argument ('" + precision_str + "') for option 'float-precision' is invalid");
	}
}

program_args parse_program_args(int argc, const char *const argv[]) {
	try {
		program_args result;
		string mode_str, int_precision_str, float_precision_str;
		int threads = 0, blocksize = 0;
		bool no_cuda = false, no_opencl = false;

		po::positional_options_description pos_opts;
		pos_opts.add("mode", 1);
		pos_opts.add("jobspecs", 1);

		po::options_description opts;
		opts.add_options()
			//positional args
			("mode", po::value<string>(&mode_str)) //
			("jobspecs", po::value<string>(&result.jobspecs)) //
			//regular args
			("in", po::value<string>(&result.options.in1_path)) //
			("in1", po::value<string>(&result.options.in1_path)) //
			("in2", po::value<string>(&result.options.in2_path)) //
			("out", po::value<string>(&result.options.out_path)) //
			("threads", po::value<int>(&threads)->default_value(defaults::threads)) //
			("blocksize", po::value<int>(&blocksize)->default_value(defaults::blocksize)) //
			("no-cuda", po::bool_switch(&no_cuda)->default_value(false)) //
			("no-opencl", po::bool_switch(&no_opencl)->default_value(false)) //
			("int-precision", po::value<string>(&int_precision_str)->default_value("default")) //
			("float-precision", po::value<string>(&float_precision_str)->default_value("default"));

		po::variables_map args;
		try {
			po::store(po::command_line_parser(argc, argv).options(opts).positional(pos_opts).run(), args);
			po::notify(args);
		} catch (const exception &ex) {
			//for consistency with other exception strings
			string message = ex.what();
			message[0] = toupper(message[0]);
			throw invalid_argument(message);
		}

		check_option_present(args, "mode");
		if (mode_str == "repr") {
			check_option_present(args, "jobspecs", "job");
			check_option_present(args, "in");
			check_option_present(args, "out");
			check_value_positive(threads, "threads");
			check_option_absent(args, "repr", "in1");
			check_option_absent(args, "repr", "in2");
			check_option_absent(args, "repr", "blocksize");
			check_option_absent(args, "repr", "no-cuda");
			check_option_absent(args, "repr", "no-opencl");

			result.mode = program_mode::repr;
			result.options.threads = threads;
			result.options.int_precision = parse_int_precision(int_precision_str);
			result.options.float_precision = parse_float_precision(program_mode::repr, float_precision_str);

			check_directory_output(result.options.out_path);
		} else if (mode_str == "dist") {
			check_option_present(args, "jobspecs", "job");
			check_option_present(args, "out");
			check_value_positive(threads, "threads");
			check_value_positive(blocksize, "blocksize");

			if (args.count("in1") || args.count("in2")) {
				check_option_present(args, "in1");
				check_option_present(args, "in2");
				check_option_absent(args, "dist", "in", "in", "when options 'in1' or 'in2' are specified also");
			} else {
				check_option_present(args, "in");
				check_option_absent(args, "dist", "in1", "in1", "when option 'in' is specified also");
				check_option_absent(args, "dist", "in2", "in2", "when option 'in' is specified also");
			}

			result.mode = program_mode::dist;
			result.options.threads = threads;
			result.options.blocksize = blocksize;
			result.options.use_cuda = !no_cuda;
			result.options.use_opencl = !no_opencl;
			result.options.int_precision = parse_int_precision(int_precision_str);
			result.options.float_precision = parse_float_precision(program_mode::dist, float_precision_str);

			check_directory_output(result.options.out_path);
		} else if (mode_str == "resume") {
			check_option_absent(args, "resume", "in1");
			check_option_absent(args, "resume", "in2");
			check_option_absent(args, "resume", "jobspecs", "job");
			check_option_absent(args, "resume", "out");
			check_value_positive(threads, "threads");
			check_value_positive(threads, "blocksize");
			check_option_absent(args, "resume", "int-precision");
			check_option_absent(args, "resume", "float-precision");

			result.mode = program_mode::resume;
			result.options.threads = threads;
			result.options.blocksize = blocksize;
			result.options.use_cuda = !no_cuda;
			result.options.use_opencl = !no_opencl;

			if (!args.count("in")) {
				result.options.in1_path = defaults::resume_filename;
			}
			check_file_input(result.options.in1_path);
		} else if (mode_str == "help") {
			check_option_absent(args, "help", "jobspecs", "job");
			check_option_absent(args, "help", "in");
			check_option_absent(args, "help", "in1");
			check_option_absent(args, "help", "in2");
			check_option_absent(args, "help", "out");
			check_option_absent(args, "help", "threads");
			check_option_absent(args, "help", "blocksize");
			check_option_absent(args, "help", "no-cuda");
			check_option_absent(args, "help", "no-opencl");
			check_option_absent(args, "help", "int-precision");
			check_option_absent(args, "help", "float-precision");

			result.mode = program_mode::help;
		} else {
			throw invalid_argument("The argument ('" + mode_str + "') for option 'mode' is invalid");
		}

		return result;
	} catch (const invalid_argument &ex) {
		cerr << error_mark << ex.what() << endl << endl;
		usage::put_options_help(cerr);
		throw already_handled_error();
	}
}
