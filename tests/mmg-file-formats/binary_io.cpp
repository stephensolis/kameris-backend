#include <catch.hpp>

#include <cstdint>
#include <map>
#include <sstream>
#include <vector>

#include <mmg-formats/cpp/common/binary_collection_io.hpp>
#include <mmg-formats/cpp/common/binary_io.hpp>

#include "../test_helpers.hpp"

using namespace std;
using namespace mmg;

SCENARIO("mmg-formats binary IO", "[mmg-formats][common][binary]") {
	GIVEN("An integer") {
		WHEN("calling write_binary then read_binary") {
			THEN("the original value is returned") {
				uint32_t val = 10;

				stringstream stream;
				write_binary(stream, val);

				uint32_t read_val;
				read_binary(stream, read_val);

				CHECK(val == read_val);
			}
		}
	}

	GIVEN("An empty array") {
		WHEN("calling write_array_binary") {
			THEN("nothing is written") {
				ostringstream stream;
				write_array_binary<uint32_t>(stream, nullptr, 0);

				CHECK(stream.str().empty());
			}
		}
	}

	GIVEN("An array") {
		WHEN("calling write_array_binary then read_array_binary") {
			THEN("the original value is returned") {
				vector<uint32_t> arr = {1, 2, 3, 4};

				stringstream stream;
				write_array_binary(stream, arr.data(), arr.size());

				auto *read_data = read_array_binary<uint32_t>(stream, arr.size());
				vector<uint32_t> read_arr(read_data, read_data + arr.size());

				CHECK(arr == read_arr);
			}
		}
	}

	GIVEN("An empty map") {
		WHEN("calling write_map_binary then read_map_binary") {
			THEN("an empty map is returned") {
				stringstream stream;
				write_map_binary(stream, map<uint32_t, uint32_t>());

				auto read_map = read_map_binary<uint32_t, uint32_t>(stream, 0);

				CHECK(read_map.empty());
			}
		}
	}

	GIVEN("A map") {
		WHEN("calling write_map_binary then read_map_binary") {
			THEN("the original value is returned") {
				map<uint32_t, uint32_t> orig_map = {{1, 2}, {3, 4}};

				stringstream stream;
				write_map_binary(stream, orig_map);

				auto read_map = read_map_binary<uint32_t, uint32_t>(stream, 2);

				CHECK(orig_map == read_map);
			}
		}
	}
}
