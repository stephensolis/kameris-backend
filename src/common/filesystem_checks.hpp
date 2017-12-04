#ifndef _COMMON_FILESYSTEM_CHECKS_
#define _COMMON_FILESYSTEM_CHECKS_

#include <string>

void check_directory_input(const std::string &dirname);
void check_directory_output(const std::string &dirname);

void check_file_input(const std::string &filename);
void check_file_output(const std::string &filename);

#endif
