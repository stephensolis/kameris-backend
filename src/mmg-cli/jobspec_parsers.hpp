#ifndef _CLI_JOBSPEC_PARSERS_
#define _CLI_JOBSPEC_PARSERS_

#include <string>
#include <vector>

#include "../common/options_structs.hpp"

std::vector<repr_options> parse_repr_jobs(const std::string &jobspecs);
std::vector<dist_options> parse_dist_jobs(const std::string &jobspecs);

#endif
