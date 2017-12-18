#ifndef _MMG_FORMATS_DIST_READER_
#define _MMG_FORMATS_DIST_READER_

#include <cstdint>
#include <fstream>

#include "common/raw_binary_io.hpp"
#include "common/storage_encoding.hpp"

#include "headers.hpp"

namespace mmg {
	class dist_reader {
	 private:
		std::ifstream file;

		dist_header read_header() {
			dist_header result{};
			element_underlying_t value_val = 0;

			read_binary_raw(file, value_val);
			result.value_type = static_cast<element_type>(from_storage_encoding<decltype(value_val)>(value_val));
			read_binary_raw(file, result.size);
			result.size = from_storage_encoding<decltype(result.size)>(result.size);

			return result;
		}
	};
}

#endif
