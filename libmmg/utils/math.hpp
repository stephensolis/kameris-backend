#ifndef _LIBMMG_UTILS_MATH_
#define _LIBMMG_UTILS_MATH_

#include <cmath>

namespace mmg {
	template <typename T>
	inline T square(const T val) {
		return val * val;
	}

	template <typename T, typename U>
	inline T absdiff(const T val1, const U val2) {
		return (val1 < val2) ? val2 - val1 : val1 - val2;
	}

	//equivalent to std::is_unsigned<decltype(val)>::value, to work around an MSVC2015 bug
	template <typename T>
	inline bool is_unsigned(const T /*val*/) {
		return T(0) < T(-1);
	}
}

#endif
