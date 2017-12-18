#ifndef _MMG_FORMATS_COMMON_BINARY_COLLECTION_IO_
#define _MMG_FORMATS_COMMON_BINARY_COLLECTION_IO_

#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <ostream>
#include <vector>

#include "raw_binary_io.hpp"
#include "storage_encoding.hpp"

namespace mmg {
	template <typename T>
	inline std::vector<T> read_array_binary(std::istream &stream, size_t count) {
		std::vector<T> result;
		result.reserve(count);

		for (size_t i = 0; i < count; ++i) {
			storage_encoding_t<T> curr_val;
			read_binary_raw(stream, curr_val);
			result.emplace_back(from_storage_encoding<T>(curr_val));
		}

		return result;
	}

	template <typename T>
	inline void write_array_binary(std::ostream &stream, const T *data, size_t count) {
		for (size_t i = 0; i < count; ++i) {
			write_binary_raw(stream, to_storage_encoding(data[i]));
		}
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map>
	inline Map<Key, T> read_map_binary(std::istream &stream) {
		Map<Key, T> result;

		uint64_t count{};
		read_binary_raw(stream, count);
		count = from_storage_encoding<uint64_t>(count);

		for (uint64_t i = 0; i < count; ++i) {
			storage_encoding_t<Key> first_val{};
			read_binary_raw(stream, first_val);
			Key first = from_storage_encoding<Key>(first_val);

			storage_encoding_t<T> second_val{};
			read_binary_raw(stream, second_val);
			T second = from_storage_encoding<T>(second_val);

			result.emplace_hint(result.end(), first, second);
		}

		return result;
	}

	template <typename Key, typename T, template <typename, typename, typename...> class Map = std::map,
		typename... MapArgs>
	inline void write_map_binary(std::ostream &stream, const Map<Key, T, MapArgs...> &map) {
		write_binary_raw(stream, to_storage_encoding<uint64_t>(map.size()));

		for (const auto &el : map) {
			write_binary_raw(stream, to_storage_encoding(el.first));
			write_binary_raw(stream, to_storage_encoding(el.second));
		}
	}
}

#endif
