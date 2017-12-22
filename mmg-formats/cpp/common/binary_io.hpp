#ifndef _MMG_FORMATS_COMMON_BINARY_IO_
#define _MMG_FORMATS_COMMON_BINARY_IO_

#include <istream>
#include <ostream>

#include "storage_encoding.hpp"

namespace mmg {
	template <typename T>
	inline void read_binary_raw(std::istream &stream, T &target) {
		stream.read(reinterpret_cast<char *>(&target), sizeof(T)); // NOLINT
	}

	template <typename T>
	inline void read_binary(std::istream &stream, T &target) {
		storage_encoding_t<T> val;
		read_binary_raw(stream, val);
		target = from_storage_encoding<T>(val);
	}

	template <typename T>
	inline void write_binary_raw(std::ostream &stream, const T &data) {
		stream.write(reinterpret_cast<const char *>(&data), sizeof(T)); // NOLINT
	}

	template <typename T>
	inline void write_binary(std::ostream &stream, const T &data) {
		write_binary_raw(stream, to_storage_encoding(data));
	}
}

#endif
