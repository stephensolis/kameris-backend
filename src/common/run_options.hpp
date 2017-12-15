#ifndef _COMMON_RUN_OPTIONS_
#define _COMMON_RUN_OPTIONS_

#include <istream>
#include <ostream>
#include <vector>
#include <boost/variant.hpp>
#include "options_structs.hpp"

enum class run_mode : uint8_t { repr, dist, resume };

struct run_options {
	run_mode mode;
	program_options options;
	boost::variant<std::vector<repr_options>, std::vector<dist_options>> jobs;
};

run_options read_run_options(std::istream &stream);
void write_run_options(std::ostream &stream, const run_options &options);

#endif
