#ifndef _CLI_ARGS_PARSER_
#define _CLI_ARGS_PARSER_

#include <string>
#include "../common/options_structs.hpp"

enum class program_mode { repr, dist, resume, help };

struct program_args {
	program_mode mode;
	std::string jobspecs;
	program_options options;
};

program_args parse_program_args(int argc, const char *const argv[]);

#endif
