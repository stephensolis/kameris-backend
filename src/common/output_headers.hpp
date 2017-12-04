#ifndef _COMMON_OUTPUT_HEADERS_
#define _COMMON_OUTPUT_HEADERS_

#include <cstdint>
#include <istream>
#include <ostream>

// clang-format off
enum class element_type : uint8_t {
	uint8, uint16, uint32, uint64, //unsigned integral types
	float32, float64 //float types
};
// clang-format on

enum class repr_type : uint8_t { cgr, krap, cv, descriptor };

struct repr_header {
	repr_type type;
	uint8_t /*(bool)*/ is_sparse;
	element_type key_type;
	element_type value_type;
	uint32_t count;
	uint64_t rows, cols;
};
repr_header read_repr_header(std::istream &stream);
void write_repr_header(std::ostream &stream, const repr_header &header);

struct dist_header {
	element_type value_type;
	uint32_t size;
};
dist_header read_dist_header(std::istream &stream);
void write_dist_header(std::ostream &stream, const dist_header &header);

#endif
