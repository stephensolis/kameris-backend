#include <catch.hpp>

#include <sstream>
#include <string>

#include <libkameris/utils/fasta.hpp>

#include "../test_helpers.hpp"

using namespace std;
using namespace kameris;

SCENARIO("libkameris FASTA", "[libkameris][utils][fasta]") {
	GIVEN("An empty stream") {
		istringstream stream("");

		WHEN("calling read_fasta") {
			THEN("the result is empty") {
				CHECK(read_fasta(stream).empty());
			}
		}
	}

	GIVEN("A stream containing no sequences, only comment lines and whitespace") {
		istringstream stream("\n>foo \t\n\r    \r\n>\n\n");

		WHEN("calling read_fasta") {
			THEN("the result is empty") {
				CHECK(read_fasta(stream).empty());
			}
		}
	}

	GIVEN("A stream containing one sequence") {
		istringstream stream("\n>foo\ntest1\t\n> \r\n");

		WHEN("calling read_fasta") {
			THEN("the result contains the sequence") {
				CHECK(read_fasta(stream) == vector<string>{"test1"});
			}
		}
	}

	GIVEN("A stream containing multiple sequences separated by comment lines") {
		istringstream stream(">foo\r\ntest1\n>\ntest2  ");

		WHEN("calling read_fasta") {
			THEN("the result contains the sequences") {
				CHECK(read_fasta(stream) == vector<string>{"test1", "test2"});
			}
		}
	}

	GIVEN("A stream containing multiple sequences separated by blank lines") {
		istringstream stream("\n\ntest1 \r\n\n\r\ntest2\n");

		WHEN("calling read_fasta") {
			THEN("the result contains the sequences") {
				CHECK(read_fasta(stream) == vector<string>{"test1", "test2"});
			}
		}
	}
}
