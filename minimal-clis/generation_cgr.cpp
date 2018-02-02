#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <libmmg/representations.hpp>
#include <libmmg/utils.hpp>
#include <mmg-formats/cpp/repr_writer.hpp>

// clang-format off
#include <src/mmg-cli/progress_bar.hpp>
#include <src/mmg-cli/progress_bar.cpp>
// clang-format on

using namespace std;
using namespace mmg;
namespace fs = boost::filesystem;

const unsigned num_threads = thread::hardware_concurrency();

template <typename cgr_element_t>
inline void run(const vector<string> &filenames, const string &output_filename, const string &cgr_type, unsigned k,
	const pair<unordered_set<char>, unordered_set<char>> &twocgr_chars) {
	size_t num_seqs = filenames.size();
	vector<vector<cgr_element_t>> cgrs(num_seqs);

	ProgressBar bar(num_seqs);
	ParallelExecutor exec;
	vector<future<void>> results;
	size_t cgr_length;

	for (size_t i = 0; i < num_seqs; ++i) {
		if (cgr_type == "cgr") {
			cgr_length = size_t(1) << (2 * k);
			results.push_back(exec.enqueue([&, i](unsigned) {
				ifstream file(filenames[i]);
				vector<string> seqs = read_fasta(file);
				cgrs[i] = cgr<cgr_element_t>(seqs.front(), k);
			}));
		} else {
			cgr_length = size_t(1) << k;
			results.push_back(exec.enqueue([&, i](unsigned) {
				ifstream file(filenames[i]);
				vector<string> seqs = read_fasta(file);
				cgrs[i] = twocgr<cgr_element_t>(seqs.front(), k, twocgr_chars.first, twocgr_chars.second);
			}));
		}
	}
	exec.execute(thread::hardware_concurrency());

	repr_writer writer(output_filename,
		{//
			/* is_sparse */ false,
			/* key_type */ element_type::uint64,
			/* value_type */ element_type_for_type<cgr_element_t>,
			/* count */ num_seqs,
			/* rows */ 1,
			/* cols */ cgr_length});
	for (size_t i = 0; i < num_seqs; ++i) {
		results[i].wait();
		writer.write_matrix(make_vector_adapter(cgrs[i].data(), cgr_length));
		bar.increment();
	}
}

int main(int argc, char *argv[]) {
	if (argc != 6 && argc != 7) {
		cout << "Usage: generation_cgr <cgr type: cgr/twocgr> <input directory> <output filename> <k> "
				"<bits per element: 16/32> <if twocgr: chars for: first,second>"
			 << endl;
		return 1;
	}

	pair<unordered_set<char>, unordered_set<char>> twocgr_chars;
	//TODO: implement splitting of argument

	vector<string> filenames;
	for (const auto &entry : boost::make_iterator_range(fs::directory_iterator(argv[2]), {})) {
		filenames.push_back(absolute(entry.path()).string());
	}
	sort(filenames.begin(), filenames.end());

	cout << filenames.size() << " files, using " << num_threads << " threads" << endl << endl;

	if ("16"s == argv[5]) {
		run<uint16_t>(filenames, argv[3], argv[1], atoi(argv[4]), twocgr_chars);
	} else if ("32"s == argv[5]) {
		run<uint32_t>(filenames, argv[3], argv[1], atoi(argv[4]), twocgr_chars);
	}

	return 0;
}
