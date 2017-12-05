#ifndef _CLI_CONSOLE_MARKS_
#define _CLI_CONSOLE_MARKS_

#include <cstddef>
#include <ostream>
#include <string>
#include "colors.hpp"

inline std::ostream &error_mark(std::ostream &stream) {
	stream << colors::red << "[!] " << colors::default_color;
	return stream;
}
inline std::ostream &warning_mark(std::ostream &stream) {
	stream << colors::yellow << "[!] " << colors::default_color;
	return stream;
}
inline std::ostream &info_mark(std::ostream &stream) {
	stream << colors::blue << "[i] " << colors::default_color;
	return stream;
}

#endif
