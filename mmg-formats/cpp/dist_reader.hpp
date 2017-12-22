#ifndef _MMG_FORMATS_DIST_READER_
#define _MMG_FORMATS_DIST_READER_

#include <algorithm>
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <utility>

#include <boost/mpl/transform.hpp>
#include <boost/variant.hpp>

#include <libmmg/utils/matrix_vector_adapters.hpp>

#include "common/binary_collection_io.hpp"
#include "common/binary_io.hpp"

#include "headers.hpp"

namespace mmg {
	class dist_reader {
	 private:
		static dist_header read_header(std::istream &file) {
			const std::unique_ptr<char[]> file_signature(read_array_binary<char>(file, dist_header::signature.size()));
			if (!std::equal(&file_signature[0], &file_signature[dist_header::signature.size()],
					dist_header::signature.begin())) {
				throw std::invalid_argument("The given file is not a valid mm-dist file");
			}

			dist_header result{};
			element_underlying_t value_val = 0;

			read_binary(file, value_val);
			result.value_type = static_cast<element_type>(value_val);
			read_binary(file, result.size);

			return result;
		}

	 public:
		static auto read_matrix(const std::string &filename) {
			std::ifstream file(filename, std::ios::binary);
			dist_header header = read_header(file);

			typename boost::make_variant_over<boost::mpl::transform<element_type_types,
				SymmetricDistanceMatrixAdapter<boost::mpl::_1>>::type>::type result;

			dispatch_on_element_type(header.value_type, [&](auto dummy_val = 0) {
				using Value = decltype(dummy_val);
				const size_t matrix_data_size = header.size * (header.size - 1) / 2;

				result = make_symmetric_distance_adapter<Value>(
					read_array_binary<Value>(file, matrix_data_size), header.size, true);
			});
			return result;
		}
	};
}

#endif
