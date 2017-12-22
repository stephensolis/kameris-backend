#ifndef _MMG_FORMATS_DIST_WRITER_
#define _MMG_FORMATS_DIST_WRITER_

#include <fstream>
#include <ios>
#include <string>

#include "common/binary_collection_io.hpp"
#include "common/binary_io.hpp"

#include "headers.hpp"

namespace mmg {
	class dist_writer {
	 private:
		std::ofstream _file;
		const dist_header _header;
		size_t _curr_row = 0;

		void write_header(const dist_header &header) {
			write_array_binary(_file, dist_header::signature.data(), 7);
			write_binary(_file, static_cast<element_underlying_t>(header.value_type));
			write_binary(_file, header.size);
		}

	 public:
		explicit dist_writer(const std::string &filename, dist_header in_header, bool create_file = true)
				: _header(in_header) {
			if (create_file) {
				_file.open(filename, std::ios::binary | std::ios::trunc);
				write_header(_header);
			} else {
				_file.open(filename, std::ios::binary | std::ios::ate);
			}
		}

		template <typename Matr>
		void write_next_row(const Matr &matr) {
			if (element_type_for_type<Matr::value_type> != _header.value_type) {
				throw std::invalid_argument("The given matrix must contain values of the same type as the _header");
			}

			for (size_t i = _curr_row + 1; i < _header.size; ++i) {
				write_binary(_file, matr(_curr_row, i));
			}

			++_curr_row;
			_file.flush();
		}

		size_t curr_row() const {
			return _curr_row;
		}
	};
}

#endif
