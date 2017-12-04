#ifndef _CLI_STREAMS_
#define _CLI_STREAMS_

#include <cstddef>
#include <ostream>
#include <string>
#include "colors.hpp"

inline std::ostream &error(std::ostream &stream) {
	stream << colors::red << "[!] " << colors::default_color;
	return stream;
}
inline std::ostream &warning(std::ostream &stream) {
	stream << colors::yellow << "[!] " << colors::default_color;
	return stream;
}
inline std::ostream &info(std::ostream &stream) {
	stream << colors::blue << "[i] " << colors::default_color;
	return stream;
}

#endif
