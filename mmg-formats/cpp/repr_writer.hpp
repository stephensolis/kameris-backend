#ifndef _MMG_FORMATS_REPR_WRITER_
#define _MMG_FORMATS_REPR_WRITER_

#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

#include "common/binary_collection_io.hpp"
#include "common/binary_io.hpp"

#include "headers.hpp"

namespace mmg {
	class repr_writer {
	 private:
		std::ofstream _file;
		const repr_header _header;
		size_t _curr_matr = 0;

		void write_header(const repr_header &header) {
			write_array_binary(_file, repr_header::signature.data(), 7);
			write_binary(_file, static_cast<uint8_t>(header.is_sparse));
			write_binary(_file, static_cast<element_underlying_t>(header.key_type));
			write_binary(_file, static_cast<element_underlying_t>(header.value_type));
			write_binary(_file, header.count);
			write_binary(_file, header.rows);
			write_binary(_file, header.cols);

			//blank sizes table
			if (header.is_sparse) {
				std::vector<uint64_t> blank_sizes_table(header.count, 0);
				write_array_binary(_file, blank_sizes_table.data(), header.count);
			}
		}

		//for sparse matrices
		template <typename Matr, typename = typename Matr::key_type>
		void write_matrix_impl(const Matr &matr) {
			if (!_header.is_sparse) {
				throw std::invalid_argument("The given matrix must be sparse because the header is sparse");
			}
			if (element_type_for_type<Matr::key_type> != _header.key_type) {
				throw std::invalid_argument("The given matrix's key type must match the header");
			}

			//size, to sizes table
			_file.seekp(repr_header_size + (sizeof(uint64_t) * _curr_matr));
			write_binary(_file, static_cast<uint64_t>(matr.elements()));

			//data, to the end
			_file.seekp(0, std::ios::end);
			write_map_binary(_file, matr.data());
		}

		//for regular matrices
		template <typename Matr>
		void write_matrix_impl(const Matr &matr) {
			if (_header.is_sparse) {
				throw std::invalid_argument("The given matrix must not be sparse because the header is not sparse");
			}

			_file.seekp(0, std::ios::end);
			write_array_binary(_file, matr.data(), matr.size());
		}

	 public:
		explicit repr_writer(const std::string &filename, repr_header header, bool create_file = true)
				: _header(header) {
			if (create_file) {
				_file.open(filename, std::ios::binary | std::ios::trunc);
				write_header(_header);
			} else {
				_file.open(filename, std::ios::binary | std::ios::ate);
			}
		}

		template <typename Matr>
		void write_matrix(const Matr &matr) {
			if (element_type_for_type<Matr::value_type> != _header.value_type) {
				throw std::invalid_argument("The given matrix's value type must match the header");
			}

			write_matrix_impl(matr);
			++_curr_matr;
			_file.flush();
		}
	};
}

#endif
