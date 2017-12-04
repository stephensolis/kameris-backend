#include <catch.hpp>

#include <sstream>
#include <vector>

#include <libmmg/io/adapters.hpp>
#include <libmmg/io/text.hpp>

#include "test_helpers.hpp"

using namespace std;
using namespace mmg;

SCENARIO("libmmg text IO", "[libmmg][io][text]") {
	GIVEN("An empty matrix") {
		const auto matr = make_matrix_adapter<unsigned>(nullptr, 0, 0);

		WHEN("calling write_matrix_text") {
			THEN("the result is blank") {
				ostringstream stream;
				write_matrix_text(stream, matr);

				CHECK(stream.str().empty());
			}
		}
	}

	GIVEN("A matrix") {
		vector<int> matr_flat = {1, 2, 3, 4, 5, 6};
		const auto matr = make_matrix_adapter(matr_flat.data(), 2, 3);

		WHEN("calling write_matrix_text") {
			THEN("the result is correct") {
				ostringstream stream;
				write_matrix_text(stream, matr);

				CHECK(stream.str() == "1,2,3\n4,5,6\n");
			}
		}
	}
}
