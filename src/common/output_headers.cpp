#include <cstdint>
#include <istream>
#include <ostream>
#include <type_traits>

#include <libmmg/io/binary.hpp>
#include <libmmg/io/storage_encoding.hpp>

#include "output_headers.hpp"

using namespace std;
using namespace mmg;

using element_underlying_t = typename underlying_type<element_type>::type;

repr_header read_repr_header(istream &stream) {
	repr_header result{};
	element_underlying_t key_val = 0, value_val = 0;

	read_binary_raw(stream, result.is_sparse);
	result.is_sparse = from_storage_encoding<decltype(result.is_sparse)>(result.is_sparse);
	read_binary_raw(stream, key_val);
	result.key_type = static_cast<element_type>(from_storage_encoding<decltype(key_val)>(key_val));
	read_binary_raw(stream, value_val);
	result.value_type = static_cast<element_type>(from_storage_encoding<decltype(value_val)>(value_val));
	read_binary_raw(stream, result.count);
	result.count = from_storage_encoding<decltype(result.count)>(result.count);
	read_binary_raw(stream, result.rows);
	result.rows = from_storage_encoding<decltype(result.rows)>(result.rows);
	read_binary_raw(stream, result.cols);
	result.cols = from_storage_encoding<decltype(result.cols)>(result.cols);

	return result;
}

void write_repr_header(ostream &stream, const repr_header &header) {
	write_binary_raw(stream, to_storage_encoding(header.is_sparse));
	write_binary_raw(stream, to_storage_encoding(static_cast<element_underlying_t>(header.key_type)));
	write_binary_raw(stream, to_storage_encoding(static_cast<element_underlying_t>(header.value_type)));
	write_binary_raw(stream, to_storage_encoding(header.count));
	write_binary_raw(stream, to_storage_encoding(header.rows));
	write_binary_raw(stream, to_storage_encoding(header.cols));
}

dist_header read_dist_header(istream &stream) {
	dist_header result{};
	element_underlying_t value_val = 0;

	read_binary_raw(stream, value_val);
	result.value_type = static_cast<element_type>(from_storage_encoding<decltype(value_val)>(value_val));
	read_binary_raw(stream, result.size);
	result.size = from_storage_encoding<decltype(result.size)>(result.size);

	return result;
}

void write_dist_header(ostream &stream, const dist_header &header) {
	write_binary_raw(stream, to_storage_encoding(static_cast<element_underlying_t>(header.value_type)));
	write_binary_raw(stream, to_storage_encoding(header.size));
}
