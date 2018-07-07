#include <catch.hpp>

#include <stdexcept>

#include <libkameris/utils/parallel.hpp>

#include "../test_helpers.hpp"

using namespace std;
using namespace kameris;

SCENARIO("libkameris utils parallel", "[libkameris][utils][parallel]") {
	WHEN("Running jobs in a ParallelExecutor") {
		ParallelExecutor exec;

		THEN("job results are correct") {
			vector<int> values = {1, 2, 3, 4, 5};

			vector<future<int>> results;
			results.reserve(values.size());

			for (int x : values) {
				results.push_back(exec.enqueue([=](unsigned) {
					return x; //
				}));
			}

			exec.execute(2);
			for (size_t i = 0; i < values.size(); ++i) {
				CHECK(results[i].get() == values[i]);
			}
		}

		THEN("thread IDs are passed to the launched functions") {
			vector<future<unsigned>> results;
			results.reserve(5);

			for (size_t i = 0; i < 5; ++i) {
				results.push_back(exec.enqueue([&](unsigned thread_id) {
					return thread_id; //
				}));
			}

			exec.execute(2);
			for (size_t i = 0; i < 5; ++i) {
				unsigned result = results[i].get();
				CHECK((result == 0 || result == 1));
			}
		}
	}

	WHEN("Attempting to run a ParallelExecutor with no workers") {
		ParallelExecutor exec;
		TEST_THROWS(it, exec.execute(0), invalid_argument);
	}

	WHEN("Adding jobs to a started ParallelExecutor") {
		ParallelExecutor exec;
		exec.execute(1);
		TEST_THROWS(it, exec.enqueue([](unsigned) {}), logic_error);
	}

	WHEN("Starting a started ParallelExecutor") {
		ParallelExecutor exec;
		exec.execute(1);
		TEST_THROWS(it, exec.execute(1), logic_error);
	}
}
