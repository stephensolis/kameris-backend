#ifndef _COMMON_RUN_ARGS_
#define _COMMON_RUN_ARGS_

#include <istream>
#include <ostream>
#include <vector>
#include <boost/variant.hpp>
#include "options_structs.hpp"

enum class run_mode : uint8_t { repr, dist, resume };

struct run_args {
	run_mode mode;
	run_options options;
	boost::variant<std::vector<repr_options>, std::vector<dist_options>> jobs;
};

run_args read_run_args(std::istream &stream);
void write_run_args(std::ostream &stream, const run_args &options);

#endif
