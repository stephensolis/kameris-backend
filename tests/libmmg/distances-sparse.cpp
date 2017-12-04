#include <catch.hpp>

#include <cstdint>
#include <map>

#include <libmmg/distances-sparse.hpp>
#include <libmmg/io/adapters.hpp>

#include "test_helpers.hpp"

using namespace std;
using namespace mmg;

const map<uint64_t, double> vect1_map = {
	{0, 4.683466861851057}, {1, 5.686983973114705}, {2, 8.438974067654303}, {3, 2.066034805708277}, {6, 1}};
const map<uint64_t, double> vect2_map = {
	{0, 2.791099277582326}, {1, 1.658589795039715}, {2, 9.050008948479139}, {3, 3.5626466924860285}, {8, 1}};
const map<uint64_t, double> vect3_map = {
	{0, 2.791099277582326}, {1, 1.658589795039715}, {2, 9.050008948479139}, {3, 3.5626466924860285}, {5, 1}};
const auto vect1_sparse = make_sparse_vector_adapter(const_cast<map<uint64_t, double> &>(vect1_map), 10); // NOLINT
const auto vect2_sparse = make_sparse_vector_adapter(const_cast<map<uint64_t, double> &>(vect2_map), 10); // NOLINT
const auto vect3_sparse = make_sparse_vector_adapter(const_cast<map<uint64_t, double> &>(vect3_map), 10); // NOLINT

map<uint64_t, double> vect1_map_copy = vect1_map;
auto vect1_sparse_copy = make_sparse_vector_adapter(vect1_map_copy, 10);

const map<uint64_t, double> zero_vect_map;
const auto zero_vect_sparse =
	make_sparse_vector_adapter(const_cast<map<uint64_t, double> &>(zero_vect_map), 4); // NOLINT
const auto empty_vect_sparse =
	make_sparse_vector_adapter(const_cast<map<uint64_t, double> &>(zero_vect_map), 0); // NOLINT

#define RANDOM_VECTOR_DISTANCE_TESTS(vect1, vect2)                                                                     \
	TEST_CLOSE_TO_DOUBLE(Euclidean distance, sparse_euclidean(vect1, vect2), 4.941884811806992);                       \
	TEST_CLOSE_TO_DOUBLE(                                                                                              \
		squared Euclidean distance, sparse_squared_euclidean<double>(vect1, vect2), 24.422225493168632);               \
	TEST_CLOSE_TO_DOUBLE(Manhattan distance, sparse_manhattan<double>(vect1, vect2), 10.028408529946308);              \
	TEST_CLOSE_TO_DOUBLE(chi squared distance, sparse_chi_squared(vect1, vect2), 5.1075979037503298);                  \
	TEST_CLOSE_TO_DOUBLE(cosine distance, sparse_cosine(vect1, vect2), 0.0982065602458535);                            \
	TEST_CLOSE_TO_DOUBLE(Pearson distance, sparse_pearson(vect1, vect2), 0.14529408441528024);                         \
	TEST_CLOSE_TO_DOUBLE(approximate information distance, sparse_approx_info_dist(vect1, vect2), 0.3333333333333333); \
	TEST_CLOSE_TO_DOUBLE(dot product, sparse_dot_product<double>(vect1, vect2), 106.23773745242816);

#define ZERO_VECTOR_DISTANCE_TESTS(vect1, vect2)                                                         \
	TEST_CLOSE_TO_DOUBLE(Euclidean distance, sparse_euclidean(vect1, vect2), 0);                         \
	TEST_CLOSE_TO_DOUBLE(squared Euclidean distance, sparse_squared_euclidean<double>(vect1, vect2), 0); \
	TEST_CLOSE_TO_DOUBLE(Manhattan distance, sparse_manhattan<double>(vect1, vect2), 0);                 \
	TEST_CLOSE_TO_DOUBLE(chi squared distance, sparse_chi_squared(vect1, vect2), 0);                     \
	TEST_CLOSE_TO_DOUBLE(cosine distance, sparse_cosine(vect1, vect2), 0);                               \
	TEST_CLOSE_TO_DOUBLE(Pearson distance, sparse_pearson(vect1, vect2), 0);                             \
	TEST_CLOSE_TO_DOUBLE(approximate information distance, sparse_approx_info_dist(vect1, vect2), 0);    \
	TEST_CLOSE_TO_DOUBLE(dot product, sparse_dot_product<double>(vect1, vect2), 0);

#define VECTOR_LENGTH_TESTS(vect1, vect2)                                                                      \
	TEST_THROWS(Euclidean distance, sparse_euclidean(vect1, vect2), invalid_argument);                         \
	TEST_THROWS(squared Euclidean distance, sparse_squared_euclidean<double>(vect1, vect2), invalid_argument); \
	TEST_THROWS(Manhattan distance, sparse_manhattan<double>(vect1, vect2), invalid_argument);                 \
	TEST_THROWS(chi squared distance, sparse_chi_squared(vect1, vect2), invalid_argument);                     \
	TEST_THROWS(cosine distance, sparse_cosine(vect1, vect2), invalid_argument);                               \
	TEST_THROWS(Pearson distance, sparse_pearson(vect1, vect2), invalid_argument);                             \
	TEST_THROWS(approximate information distance, sparse_approx_info_dist(vect1, vect2), invalid_argument);    \
	TEST_THROWS(dot product, sparse_dot_product<double>(vect1, vect2), invalid_argument);

SCENARIO("libmmg sparse distances (normal)", "[libmmg][distances-sparse]") {
	GIVEN("Two random sparse vectors where the second is longer than the first") {
		WHEN("using as mmg::SparseVectorAdapters") {
			RANDOM_VECTOR_DISTANCE_TESTS(vect1_sparse, vect2_sparse);

			TEST_CLOSE_TO_DOUBLE(sum, sparse_sum(vect1_sparse), 21.875459708328342);
			TEST_CLOSE_TO_DOUBLE(mean, sparse_mean(vect1_sparse), 2.187545970832834);
			TEST_CLOSE_TO_DOUBLE(add to,
				(sparse_add_to(vect1_sparse_copy, vect2_sparse), sparse_sum(vect1_sparse_copy)), 39.937804421915553);
		}
	}

	GIVEN("Two random sparse vectors where the first is longer than the second") {
		WHEN("using as mmg::SparseVectorAdapters") {
			RANDOM_VECTOR_DISTANCE_TESTS(vect1_sparse, vect3_sparse);
		}
	}

	GIVEN("Two zero sparse vectors") {
		WHEN("using as mmg::SparseVectorAdapters") {
			ZERO_VECTOR_DISTANCE_TESTS(zero_vect_sparse, zero_vect_sparse);
		}
	}
}

SCENARIO("libmmg sparse distances (exceptions)", "[libmmg][distances-sparse]") {
	GIVEN("Two sparse vectors of different size") {
		WHEN("using as mmg::SparseVectorAdapters") {
			VECTOR_LENGTH_TESTS(vect1_sparse, zero_vect_sparse);

			TEST_THROWS(add to, sparse_add_to(vect1_sparse_copy, zero_vect_sparse), invalid_argument);
		}
	}

	GIVEN("An empty sparse vector") {
		WHEN("using as mmg::SparseVectorAdapters") {
			TEST_THROWS(mean, sparse_mean(empty_vect_sparse), invalid_argument);
		}
	}
}
