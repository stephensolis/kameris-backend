#ifndef _MMG_FORMATS_COMMON_STORAGE_ENCODING_
#define _MMG_FORMATS_COMMON_STORAGE_ENCODING_

#include <cstddef>
#include <cstdint>
#include <limits>

#include <boost/endian/conversion.hpp>
#include "pseudo_cast.hpp"

namespace mmg {
	static_assert(std::numeric_limits<float>::is_iec559 && sizeof(float) == sizeof(uint32_t),
		"floats must follow the IEEE 754 standard");
	static_assert(std::numeric_limits<double>::is_iec559 && sizeof(double) == sizeof(uint64_t),
		"doubles must follow the IEEE 754 standard");


	template <typename T, decltype(boost::endian::endian_reverse(T{})) = T{}>
	inline T to_storage_encoding(T val) {
		return boost::endian::native_to_little(val);
	}
	inline uint32_t to_storage_encoding(float val) {
		return boost::endian::native_to_little(pseudo_cast<uint32_t>(val));
	}
	inline uint64_t to_storage_encoding(double val) {
		return boost::endian::native_to_little(pseudo_cast<uint64_t>(val));
	}


	template <typename T>
	using storage_encoding_t = decltype(to_storage_encoding(T{}));


	template <typename T, typename = storage_encoding_t<T>>
	struct from_storage_encoding_impl {
		T operator()(T val) {
			return boost::endian::little_to_native(val);
		}
	};
	template <>
	struct from_storage_encoding_impl<float, uint32_t> {
		float operator()(uint32_t val) {
			return pseudo_cast<float>(boost::endian::little_to_native(val));
		}
	};
	template <>
	struct from_storage_encoding_impl<double, uint64_t> {
		double operator()(uint64_t val) {
			return pseudo_cast<double>(boost::endian::little_to_native(val));
		}
	};

	template <typename Original, typename Encoded>
	inline Original from_storage_encoding(Encoded val) {
		from_storage_encoding_impl<Original> converter;
		return converter(val);
	}
}

#endif
