#include <catch.hpp>

#include <stdexcept>

#include <libkameris/utils/random.hpp>

#include "../test_helpers.hpp"

using namespace std;
using namespace kameris;

SCENARIO("libkameris utils random", "[libkameris][utils][random]") {
	WHEN("Generating a random vector of ints") {
		const auto vect = random_vect<int>(10, 0, 10);

		THEN("the result is of the requested size") {
			CHECK(vect.size() == 10);
		}
		THEN("all elements in the result are between the lower and upper bounds, inclusive") {
			for (int x : vect) {
				CHECK((x >= 0 && x <= 10));
			}
		}
	}

	WHEN("Generating a random vector of doubles") {
		const auto vect = random_vect<double>(10, 0, 10);

		THEN("the result is of the requested size") {
			CHECK(vect.size() == 10);
		}
		THEN("all elements in the result are between the lower bound, inclusive, and the upper bound, exclusive") {
			for (double x : vect) {
				CHECK((x >= 0 && x < 10));
			}
		}
	}

	WHEN("Generating a random string") {
		const auto str = random_string(10, "ABC"s);

		THEN("the result is of the requested size") {
			CHECK(str.size() == 10);
		}
		THEN("all elements in the result are in the list of choices") {
			for (char c : str) {
				CHECK((c == 'A' || c == 'B' || c == 'C'));
			}
		}
	}

	WHEN("Generating a random string with no choices given") {
		TEST_THROWS(it, random_string(10, ""s), invalid_argument);
	}
}
