#ifndef _MMG_FORMATS_REPR_READER_
#define _MMG_FORMATS_REPR_READER_

#include <cstdint>
#include <fstream>

#include "common/raw_binary_io.hpp"
#include "common/storage_encoding.hpp"

#include "headers.hpp"

namespace mmg {
	class repr_reader {
	 private:
		std::ifstream file;

		repr_header read_header() {
			repr_header result{};
			element_underlying_t key_val = 0, value_val = 0;

			read_binary_raw(file, result.is_sparse);
			result.is_sparse = from_storage_encoding<decltype(result.is_sparse)>(result.is_sparse);
			read_binary_raw(file, key_val);
			result.key_type = static_cast<element_type>(from_storage_encoding<decltype(key_val)>(key_val));
			read_binary_raw(file, value_val);
			result.value_type = static_cast<element_type>(from_storage_encoding<decltype(value_val)>(value_val));
			read_binary_raw(file, result.count);
			result.count = from_storage_encoding<decltype(result.count)>(result.count);
			read_binary_raw(file, result.rows);
			result.rows = from_storage_encoding<decltype(result.rows)>(result.rows);
			read_binary_raw(file, result.cols);
			result.cols = from_storage_encoding<decltype(result.cols)>(result.cols);

			return result;
		}
	};
}

#endif
