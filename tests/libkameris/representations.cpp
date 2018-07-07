#include <catch.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <libkameris/representations.hpp>
#include <libkameris/utils/matrix_vector_adapters.hpp>

#include "../test_helpers.hpp"

using namespace std;
using namespace kameris;

const string seq = "ATCGTT??ACGT";

SCENARIO("libkameris CGR", "[libkameris][representations]") {
	GIVEN("A string") {
		WHEN("k = 2, using default order") {
			THEN("regular CGR is correct") {
				CHECK(cgr<uint32_t>(seq, 2) == vector<uint32_t>{0, 0, 2, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1});
				CHECK(sparse_cgr<uint32_t, uint32_t>(seq, 2) ==
					map<uint32_t, uint32_t>{{2, 2}, {4, 1}, {5, 1}, {11, 2}, {13, 1}, {14, 1}, {15, 1}});
			}

			THEN("twoCGR is correct") {
				CHECK(twocgr<uint32_t>(seq, 2) == vector<uint32_t>{0, 3, 4, 2});
				CHECK(sparse_twocgr<uint32_t, uint32_t>(seq, 2) == map<uint32_t, uint32_t>{{1, 3}, {2, 4}, {3, 2}});
			}
		}

		WHEN("k = 2, using custom order") {
			THEN("regular CGR is correct") {
				CHECK(
					cgr<uint32_t>(seq, 2, "CATG") == vector<uint32_t>{0, 1, 1, 1, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 2, 0});
				CHECK(sparse_cgr<uint32_t, uint32_t>(seq, 2, "CATG") ==
					map<uint32_t, uint32_t>{{1, 1}, {2, 1}, {3, 1}, {7, 2}, {8, 1}, {9, 1}, {14, 2}});
			}

			THEN("twoCGR is correct") {
				CHECK(twocgr<uint32_t>(seq, 2, {'A', 'C', 'G'}, {'T'}) == vector<uint32_t>{3, 2, 3, 1});
				CHECK(sparse_twocgr<uint32_t, uint32_t>(seq, 2, {'A', 'C', 'G'}, {'T'}) ==
					map<uint32_t, uint32_t>{{0, 3}, {1, 2}, {2, 3}, {3, 1}});
			}
		}

		WHEN("k is too large for Index") {
			TEST_THROWS(regular CGR, cgr(seq, 33), invalid_argument);
			TEST_THROWS(sparse regular CGR, (sparse_cgr<uint32_t, uint32_t>(seq, 17)), invalid_argument);

			TEST_THROWS(twoCGR, twocgr(seq, 65), invalid_argument);
			TEST_THROWS(sparse twoCGR, (sparse_twocgr<uint32_t, uint32_t>(seq, 33)), invalid_argument);
		}
	}

	GIVEN("An empty string") {
		WHEN("k = 4") {
			THEN("regular CGR is correct") {
				CHECK(cgr<uint32_t>(string(), 4) == vector<uint32_t>(256, 0));
				CHECK(sparse_cgr<uint32_t, uint32_t>(string(), 4).empty());
			}

			THEN("twoCGR is correct") {
				CHECK(twocgr<uint32_t>(string(), 4) == vector<uint32_t>(16, 0));
				CHECK(sparse_twocgr<uint32_t, uint32_t>(string(), 4).empty());
			}
		}
	}
}

SCENARIO("libkameris descriptor", "[libkameris][representations]") {
	GIVEN("A CGR matrix") {
		vector<uint32_t> cgr_flat = cgr<uint32_t>(seq, 3);
		const auto cgr_matr = make_matrix_adapter(cgr_flat.data(), 8, 8);

		THEN("descriptor is correct") {
			CHECK(descriptor<double>(cgr_matr, vector<unsigned>{4, 8}, vector<unsigned>{0, 1}) ==
				vector<double>{0.875, 0.125, 0.875, 0.125, 0.9375, 0.0625, 0.875, 0.125, 0.890625, 0.109375});
		}

		WHEN("the matrix is too small for the given window size") {
			TEST_THROWS(descriptor, descriptor(cgr_matr, vector<unsigned>{9}, vector<unsigned>{0}), invalid_argument);
		}

		WHEN("the window size is 0") {
			TEST_THROWS(descriptor, descriptor(cgr_matr, vector<unsigned>{0}, vector<unsigned>{0}), invalid_argument);
		}
	}

	GIVEN("A matrix with no rows") {
		const auto empty_matr = make_matrix_adapter<unsigned>(nullptr, 0, 1);

		TEST_THROWS(descriptor, descriptor(empty_matr, vector<unsigned>{0}, vector<unsigned>{0}), invalid_argument);
	}

	GIVEN("A matrix with no columns") {
		const auto empty_matr = make_matrix_adapter<unsigned>(nullptr, 1, 0);

		TEST_THROWS(descriptor, descriptor(empty_matr, vector<unsigned>{0}, vector<unsigned>{0}), invalid_argument);
	}
}
