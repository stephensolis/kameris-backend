#ifndef _TESTS_TEST_HELPERS_
#define _TESTS_TEST_HELPERS_

#include <cmath>
#include <limits>

inline bool double_approx_equal(double val1, double val2) {
	//std::cout << std::setprecision(16) << val1 << " " << val2 << std::endl;
	double rel_tolerance = std::numeric_limits<double>::epsilon() * 20;
	return std::fabs(val1 - val2) <= std::fmax(std::fabs(val1), std::fabs(val2)) * rel_tolerance;
}

#define TEST_CLOSE_TO_DOUBLE(description, expr, val) \
	THEN(#description " is correct") {               \
		CHECK(double_approx_equal((expr), (val)));   \
	}

#define TEST_THROWS(description, expr, exception_t) \
	THEN(#description " throws an " #exception_t) { \
		CHECK_THROWS_AS((expr), exception_t);       \
	}

#endif
