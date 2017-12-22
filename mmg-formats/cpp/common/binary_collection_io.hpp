#ifndef _MMG_FORMATS_COMMON_BINARY_COLLECTION_IO_
#define _MMG_FORMATS_COMMON_BINARY_COLLECTION_IO_

#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <ostream>
#include <vector>

#include "binary_io.hpp"

namespace mmg {
	template <typename T>
	inline T *read_array_binary(std::istream &stream, size_t count) {
		auto *result = new T[count]; // NOLINT

		for (size_t i = 0; i < count; ++i) {
			read_binary(stream, result[i]);
		}

		return result;
	}

	template <typename T>
	inline void write_array_binary(std::ostream &stream, const T *data, size_t count) {
		for (size_t i = 0; i < count; ++i) {
			write_binary(stream, data[i]);
		}
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map>
	inline Map<Key, T> read_map_binary(std::istream &stream, size_t num_values) {
		Map<Key, T> result;

		for (uint64_t i = 0; i < num_values; ++i) {
			Key first;
			read_binary(stream, first);
			T second;
			read_binary(stream, second);

			result.emplace_hint(result.end(), first, second);
		}

		return result;
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	inline void write_map_binary(std::ostream &stream, const Map<Key, T, MapArgs...> &map) {
		for (const auto &el : map) {
			write_binary(stream, el.first);
			write_binary(stream, el.second);
		}
	}
}

#endif
