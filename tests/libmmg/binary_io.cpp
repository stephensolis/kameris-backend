#include <catch.hpp>

#include <cstdint>
#include <map>
#include <sstream>
#include <vector>

#include <libmmg/io/binary.hpp>
#include <libmmg/io/binary_collections.hpp>

#include "test_helpers.hpp"

using namespace std;
using namespace mmg;

SCENARIO("libmmg binary IO", "[libmmg][io][binary]") {
	GIVEN("An integer") {
		WHEN("calling write_binary_raw then read_binary_raw") {
			THEN("the original value is returned") {
				uint32_t val = 10;

				stringstream stream;
				write_binary_raw(stream, val);

				uint32_t read_val;
				read_binary_raw(stream, read_val);

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

				vector<uint32_t> read_arr = read_array_binary<uint32_t>(stream, arr.size());

				CHECK(arr == read_arr);
			}
		}
	}

	GIVEN("An empty map") {
		WHEN("calling write_map_binary then read_map_binary") {
			THEN("an empty map is returned") {
				stringstream stream;
				write_map_binary(stream, map<uint32_t, uint32_t>());

				auto read_map = read_map_binary<uint32_t, uint32_t>(stream);

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

				auto read_map = read_map_binary<uint32_t, uint32_t>(stream);

				CHECK(orig_map == read_map);
			}
		}
	}
}
