#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <libkameris/distances.hpp>
#include <libkameris/representations.hpp>
#include <libkameris/utils.hpp>

// clang-format off
#include <src/kameris-cli/progress_bar.hpp>
#include <src/kameris-cli/progress_bar.cpp>
// clang-format on

using namespace std;
using namespace kameris;

int main() {
	//setup

	const size_t num_seqs = 100;
	vector<string> seqs;
	seqs.reserve(num_seqs);

	cout << "Random generation (" << num_seqs << " seqs): " << timeit([&]() {
		generate_n(back_inserter(seqs), num_seqs, [&]() { return random_string(1e7, "ACGT"s); });
	}) << endl;

	//serial tests

	cout << endl << "Serial tests:" << endl;
	vector<vector<uint32_t>> cgrs;
	cgrs.reserve(num_seqs);
	double canary_serial = 0;

	cout << "CGR: " << timeit([&]() {
		for (const string &a : seqs) {
			cgrs.push_back(cgr(a, 9));
		}
	}) << endl;

	cout << "Descriptors: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			auto img = make_matrix_adapter(a.data(), 512, 512);

			auto desc = descriptor(img, vector<size_t>{8}, vector<unsigned>{0, 1, 2, 5, 20});
			canary_serial += desc[0];
		}
	}) << endl;

	cout << "Euclidean: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				canary_serial += euclidean(a, b);
			}
		}
	}) << endl;

	cout << "Manhattan: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				canary_serial += manhattan(a, b);
			}
		}
	}) << endl;

	cout << "Info-dist: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				canary_serial += approx_info_dist(a, b);
			}
		}
	}) << endl;

	cout << "Cosine: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				canary_serial += cosine(a, b);
			}
		}
	}) << endl;

	cout << "Pearson: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				canary_serial += pearson(a, b);
			}
		}
	}) << endl;

	cout << "SSIM: " << timeit([&]() {
		for (const vector<uint32_t> &a : cgrs) {
			for (const vector<uint32_t> &b : cgrs) {
				auto img1 = make_matrix_adapter(a.data(), 512, 512);
				auto img2 = make_matrix_adapter(b.data(), 512, 512);

				canary_serial += ssim(img1, img2);
			}
		}
	}) << endl;

	//parallel tests

	cout << endl << "Parallel tests (" << thread::hardware_concurrency() << " threads):" << endl;
	cgrs.clear();
	cgrs.reserve(num_seqs);
	double canary_parallel = 0;

	cout << "CGR: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<vector<uint32_t>>> results;
		results.reserve(num_seqs);

		for (const string &a : seqs) {
			results.push_back(exec.enqueue([&](unsigned) {
				return cgr(a, 9); //
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<vector<uint32_t>> &x : results) {
			cgrs.push_back(x.get());
		}
	}) << endl;

	cout << "Descriptors: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				auto img = make_matrix_adapter(a.data(), 512, 512);
				auto desc = descriptor(img, vector<size_t>{8}, vector<unsigned>{0, 1, 2, 5, 20});
				return desc[0];
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "Euclidean: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					result += euclidean(a, cgrs[i]);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "Manhattan: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					result += manhattan(a, cgrs[i]);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "Info-dist: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					result += approx_info_dist(a, cgrs[i]);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "Cosine: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					result += cosine(a, cgrs[i]);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "Pearson: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					result += pearson(a, cgrs[i]);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	cout << "SSIM: " << timeit([&]() {
		ParallelExecutor exec;
		vector<future<double>> results;
		results.reserve(num_seqs);

		for (const vector<uint32_t> &a : cgrs) {
			results.push_back(exec.enqueue([&](unsigned) {
				double result = 0;
				for (size_t i = 0; i < num_seqs; ++i) {
					auto img1 = make_matrix_adapter(a.data(), 512, 512);
					auto img2 = make_matrix_adapter(cgrs[i].data(), 512, 512);
					result += ssim(img1, img2);
				}
				return result;
			}));
		}

		exec.execute(thread::hardware_concurrency());
		for (future<double> &x : results) {
			canary_parallel += x.get();
		}
	}) << endl;

	//canary

	cout << endl
		 << "If these two numbers are not the same, something went wrong: " << canary_serial << ", " << canary_parallel
		 << endl
		 << (abs(canary_serial - canary_parallel) < 1e-5 ? "All OK" : "Something went wrong")
		 << ", delta = " << abs(canary_serial - canary_parallel) << endl;
	return 0;
}
