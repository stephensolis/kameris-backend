#ifndef _MMG_FORMATS_HEADERS_
#define _MMG_FORMATS_HEADERS_

#include <cstdint>
#include <type_traits>

namespace mmg {
	// clang-format off
	enum class element_type : uint8_t {
		uint8, uint16, uint32, uint64, //unsigned integral types
		float32, float64 //float types
	};
	// clang-format on
	using element_underlying_t = typename std::underlying_type<element_type>::type;

	struct repr_header {
		uint8_t /*(bool)*/ is_sparse;
		element_type key_type;
		element_type value_type;
		uint32_t count;
		uint64_t rows, cols;
	};

	struct dist_header {
		element_type value_type;
		uint32_t size;
	};
}

#endif
