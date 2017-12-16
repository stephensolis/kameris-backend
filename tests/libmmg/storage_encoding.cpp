#include <catch.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

#include <libmmg/io/storage_encoding.hpp>

#include "test_helpers.hpp"

using namespace std;
using namespace mmg;

#define TEST_STORAGE_ENCODING(type, value)                               \
	GIVEN(#value " as an " #type) {                                      \
		WHEN("calling to_storage_encoding then from_storage_encoding") { \
			THEN("the final result is the same") {                       \
				type original = value;                                   \
				auto encoded = to_storage_encoding(original);            \
				CHECK(original == from_storage_encoding<type>(encoded)); \
			}                                                            \
		}                                                                \
	}

#define TEST_STORAGE_ENCODING_NAN(type)                                  \
	GIVEN("A " #type " NaN") {                                           \
		WHEN("calling to_storage_encoding then from_storage_encoding") { \
			THEN("the final result is still a NaN") {                    \
				type original = numeric_limits<type>::quiet_NaN();       \
				auto encoded = to_storage_encoding(original);            \
				CHECK(isnan(from_storage_encoding<type>(encoded)));      \
			}                                                            \
		}                                                                \
	}

SCENARIO("libmmg storage encoding", "[libmmg][io][storage_encoding]") {
	TEST_STORAGE_ENCODING(uint8_t, 10);
	TEST_STORAGE_ENCODING(uint16_t, 10);
	TEST_STORAGE_ENCODING(uint32_t, 10);
	TEST_STORAGE_ENCODING(uint64_t, 10);

	TEST_STORAGE_ENCODING(int8_t, 10);
	TEST_STORAGE_ENCODING(int8_t, -10);
	TEST_STORAGE_ENCODING(int16_t, 10);
	TEST_STORAGE_ENCODING(int16_t, -10);
	TEST_STORAGE_ENCODING(int32_t, 10);
	TEST_STORAGE_ENCODING(int32_t, -10);
	TEST_STORAGE_ENCODING(int64_t, 10);
	TEST_STORAGE_ENCODING(int64_t, -10);

	TEST_STORAGE_ENCODING(float, 5.3f);
	TEST_STORAGE_ENCODING(float, -5.3f);
	TEST_STORAGE_ENCODING(float, numeric_limits<float>::infinity());
	TEST_STORAGE_ENCODING(double, 5.3);
	TEST_STORAGE_ENCODING(double, -5.3);
	TEST_STORAGE_ENCODING(double, numeric_limits<double>::infinity());

	TEST_STORAGE_ENCODING_NAN(float);
	TEST_STORAGE_ENCODING_NAN(double);
}
