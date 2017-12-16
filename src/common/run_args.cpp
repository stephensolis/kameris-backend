#include <cstdint>
#include <istream>
#include <ostream>
#include <typeinfo>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/boost_optional.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include "options_structs.hpp"
#include "run_args.hpp"

using namespace std;
using namespace boost;

namespace cereal {
	void save(cereal::PortableBinaryOutputArchive &archive, const bool &val) { // NOLINT
		archive(static_cast<uint8_t>(val)); // NOLINT
	}
	void load(cereal::PortableBinaryInputArchive &archive, bool &val) { // NOLINT
		uint8_t store_val;
		archive(store_val); // NOLINT
		val = !!store_val; // NOLINT
	}
}

template <typename To, typename From>
vector<To> convert_vector_type(const vector<From> &vect) {
	vector<To> result(vect.size());

	for (const From &val : vect) {
		result.push_back(static_cast<To>(val));
	}

	return result;
}

template <class Archive>
void serialize(Archive &archive, run_args &data) { // NOLINT
	archive(data.mode, data.options, data.jobs); // NOLINT
}

template <class Archive>
void save(Archive &archive, const run_options &data) {
	archive(data.in1_path, data.in2_path, data.out_path, static_cast<uint64_t>(data.threads), // NOLINT
		static_cast<uint64_t>(data.blocksize), data.use_cuda, data.use_opencl, data.int_precision, // NOLINT
		data.float_precision); // NOLINT
}
template <class Archive>
void load(Archive &archive, run_options &data) { // NOLINT
	uint64_t store_threads;
	uint64_t store_blocksize;

	archive(data.in1_path, data.in2_path, data.out_path, store_threads, store_blocksize, data.use_cuda, // NOLINT
		data.use_opencl, data.int_precision, data.float_precision); // NOLINT
	data.threads = static_cast<unsigned>(store_threads);
	data.blocksize = static_cast<unsigned>(store_blocksize);
}

template <class Archive>
void save(Archive &archive, const cgrlike_options &data) {
	archive(data.repr, static_cast<variant<uint64_t, vector<bool>>>(data.k_or_spacedseed), data.use_freq, // NOLINT
		data.order); // NOLINT
}
template <class Archive>
void load(Archive &archive, cgrlike_options &data) { // NOLINT
	variant<uint64_t, vector<bool>> store_k_or_spacedseed;

	archive(data.repr, store_k_or_spacedseed, data.use_freq, data.order); // NOLINT
	if (store_k_or_spacedseed.type() == typeid(uint64_t)) {
		data.k_or_spacedseed = static_cast<unsigned>(get<uint64_t>(store_k_or_spacedseed));
	} else {
		data.k_or_spacedseed = get<vector<bool>>(store_k_or_spacedseed);
	}
}

template <class Archive>
void save(Archive &archive, const sparse_cgrlike_options &data) {
	archive(data.repr, static_cast<variant<uint64_t, vector<bool>>>(data.k_or_spacedseed), data.use_freq, // NOLINT
		data.order, data.freqcut); // NOLINT
}
template <class Archive>
void load(Archive &archive, sparse_cgrlike_options &data) { // NOLINT
	variant<uint64_t, vector<bool>> store_k_or_spacedseed;

	archive(data.repr, store_k_or_spacedseed, data.use_freq, data.order, data.freqcut); // NOLINT
	if (store_k_or_spacedseed.type() == typeid(uint64_t)) {
		data.k_or_spacedseed = static_cast<unsigned>(get<uint64_t>(store_k_or_spacedseed));
	} else {
		data.k_or_spacedseed = get<vector<bool>>(store_k_or_spacedseed);
	}
}

template <class Archive>
void save(Archive &archive, const descr_options &data) {
	archive(data.bins, convert_vector_type<uint64_t>(data.windows)); // NOLINT
}
template <class Archive>
void load(Archive &archive, descr_options &data) { // NOLINT
	vector<uint64_t> store_windows;

	archive(data.bins, store_windows); // NOLINT
	data.windows = convert_vector_type<unsigned>(store_windows);
}

template <class Archive>
void serialize(Archive &archive, otherdist_options &data) { // NOLINT
	archive(data.dist); // NOLINT
}

template <class Archive>
void serialize(Archive &archive, aid_options &data) { // NOLINT
	archive(data.formula); // NOLINT
}

template <class Archive>
void save(Archive &archive, const usm_options &data) {
	archive(data.est, data.compr, static_cast<optional<uint64_t>>(data.level)); // NOLINT
}
template <class Archive>
void load(Archive &archive, usm_options &data) { // NOLINT
	optional<uint64_t> store_level;

	archive(data.est, data.compr, store_level); // NOLINT
	if (store_level) {
		data.level = static_cast<unsigned>(*store_level);
	}
}

run_args read_run_args(istream &stream) {
	cereal::PortableBinaryInputArchive input_archive(stream);

	run_args data;
	input_archive(data);
	return data;
}

void write_run_args(ostream &stream, const run_args &options) {
	cereal::PortableBinaryOutputArchive output_archive(stream);

	output_archive(options);
}
