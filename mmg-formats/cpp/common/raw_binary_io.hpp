#ifndef _MMG_FORMATS_COMMON_RAW_BINARY_IO_
#define _MMG_FORMATS_COMMON_RAW_BINARY_IO_

#include <istream>
#include <ostream>

namespace mmg {
	template <typename T>
	inline void read_binary_raw(std::istream &stream, T &target) {
		stream.read(reinterpret_cast<char *>(&target), sizeof(T)); // NOLINT
	}

	template <typename T>
	inline void write_binary_raw(std::ostream &stream, const T &data) {
		stream.write(reinterpret_cast<const char *>(&data), sizeof(T)); // NOLINT
	}
}

#endif
