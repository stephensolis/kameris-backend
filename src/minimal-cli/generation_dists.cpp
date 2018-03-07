#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <libmmg/distances.hpp>
#include <libmmg/utils.hpp>
#include <mmg-formats/cpp/dist_writer.hpp>
#include <mmg-formats/cpp/repr_reader.hpp>

// clang-format off
#include "../mmg-cli/progress_bar.hpp"
#include "../mmg-cli/progress_bar.cpp"
// clang-format on

using namespace std;
using namespace mmg;
namespace fs = boost::filesystem;

const unsigned num_threads = thread::hardware_concurrency();

template <typename dist_t>
SymmetricDistanceMatrixAdapter<dist_t> make_distance_matrix(size_t size) {
	dist_t *data = new dist_t[size * (size - 1) / 2];
	return make_symmetric_distance_adapter(data, size);
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "Usage: generation_dists <input file> <output file prefix> [manhat,info]" << endl;
		return 1;
	}

	string output_prefix = argv[2];
	string dists_to_do = argv[3];
	bool do_manhat = dists_to_do.find("manhat") != string::npos;
	bool do_info = dists_to_do.find("info") != string::npos;

	repr_reader reader(argv[1]);
	const repr_header &input_header = reader.header();
	if (input_header.is_sparse) {
		cout << "Sparse matrices are not supported" << endl;
		return 1;
	}

	size_t num_matrices = input_header.count;
	cout << num_matrices << " matrices, using " << num_threads << " threads" << endl << endl;

	vector<unique_ptr<decltype(reader.read_matrix(0))>> matrices;
	{
		ProgressBar bar(num_matrices);
		for (size_t i = 0; i < num_matrices; ++i) {
			matrices.push_back(make_unique<decltype(reader.read_matrix(0))>(reader.read_matrix(i)));
			bar.increment();
		}
	}

	ProgressBar bar(num_matrices * (num_matrices - 1) / 2);
	ParallelExecutor exec;
	vector<future<void>> results;

	auto manhat_dists = make_distance_matrix<uint32_t>(do_manhat ? num_matrices : 0);
	auto info_dists = make_distance_matrix<float>(do_info ? num_matrices : 0);

	for (size_t i = 0; i < num_matrices; ++i) {
		dispatch_on_element_type(input_header.value_type, [&](auto dummy_val = 0) {
			using Value = decltype(dummy_val);

			results.push_back(exec.enqueue([&, i](unsigned _ignore) {
				for (size_t j = i + 1; j < num_matrices; ++j) {
					if (do_manhat) {
						manhat_dists(i, j) = manhattan<uint32_t>(get<MatrixAdapter<Value>>(*matrices[i].get()),
							get<MatrixAdapter<Value>>(*matrices[j].get()));
					}
					if (do_info) {
						info_dists(i, j) = approx_info_dist<float>(get<MatrixAdapter<Value>>(*matrices[i].get()),
							get<MatrixAdapter<Value>>(*matrices[j].get()));
					}
				}
			}));
		});
	}

	exec.execute(thread::hardware_concurrency());

	vector<unique_ptr<dist_writer>> dist_writers(2);
	if (do_manhat) {
		dist_writers[0] = make_unique<dist_writer>(
			output_prefix + "-manhat.mm-dist", dist_header{element_type::uint32, num_matrices});
	}
	if (do_info) {
		dist_writers[1] =
			make_unique<dist_writer>(output_prefix + "-info.mm-dist", dist_header{element_type::float32, num_matrices});
	}

	for (size_t i = 0; i < num_matrices; ++i) {
		results[i].wait();
		if (do_manhat) {
			dist_writers[0]->write_next_row(manhat_dists);
		}
		if (do_info) {
			dist_writers[1]->write_next_row(info_dists);
		}
		bar.increment(num_matrices - i - 1);
	}

	return 0;
}
