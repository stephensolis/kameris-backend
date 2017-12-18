#ifndef _MMG_FORMATS_REPR_WRITER_
#define _MMG_FORMATS_REPR_WRITER_

#include <cstdint>
#include <fstream>

#include "common/raw_binary_io.hpp"
#include "common/storage_encoding.hpp"

#include "headers.hpp"

namespace mmg {
	class repr_writer {
	 private:
		std::ofstream file;

		void write_header(const repr_header &header) {
			write_binary_raw(file, to_storage_encoding(header.is_sparse));
			write_binary_raw(file, to_storage_encoding(static_cast<element_underlying_t>(header.key_type)));
			write_binary_raw(file, to_storage_encoding(static_cast<element_underlying_t>(header.value_type)));
			write_binary_raw(file, to_storage_encoding(header.count));
			write_binary_raw(file, to_storage_encoding(header.rows));
			write_binary_raw(file, to_storage_encoding(header.cols));
		}
	};
}

#endif
