#ifndef _COMMON_OPTIONS_STRUCTS_
#define _COMMON_OPTIONS_STRUCTS_

#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <mmg-formats/cpp/element_type.hpp>

struct run_options {
	std::string in1_path;
	std::string in2_path;
	std::string out_path;

	unsigned threads;

	mmg::element_type int_precision;
	mmg::element_type float_precision;

	bool quiet;

	bool was_resumed;
	std::string resume_filename;

	//currently unused
	unsigned blocksize;
	bool use_cuda;
	bool use_opencl;
};
template <class Archive>
void serialize(Archive &archive, run_options &data) {
	//intentionally don't export threads, quiet, blocksize, use_cuda, use_opencl, was_resumed, resume_filename
	//  since they are overridden on resume anyway, see job_execution
	archive(data.in1_path, data.in2_path, data.out_path, data.int_precision, data.float_precision);
}


enum class repr_type : uint8_t { cgr, twocgr, krap, twokrap, cv, twocv, descriptor };

struct base_cgrlike_options {
	repr_type repr;

	boost::variant<uint8_t, std::vector<bool>> k_or_spacedseed;
	bool use_freq;
};

struct base_sparse_cgrlike_options {
	//currently unused
	double freqcut = 0;
};

struct regular_cgrlike_options : base_cgrlike_options {
	std::string order;
};
template <class Archive>
void serialize(Archive &archive, regular_cgrlike_options &data) {
	archive(data.repr, data.k_or_spacedseed, data.use_freq, data.order);
}

struct sparse_regular_cgrlike_options : regular_cgrlike_options, base_sparse_cgrlike_options {};
template <class Archive>
void serialize(Archive &archive, sparse_regular_cgrlike_options &data) {
	archive(data.repr, data.k_or_spacedseed, data.use_freq, data.order, data.freqcut);
}

struct twocgrlike_options : base_cgrlike_options {
	std::pair<std::string, std::string> order;
};
template <class Archive>
void serialize(Archive &archive, twocgrlike_options &data) {
	archive(data.repr, data.k_or_spacedseed, data.use_freq, data.order);
}

struct sparse_twocgrlike_options : twocgrlike_options, base_sparse_cgrlike_options {};
template <class Archive>
void serialize(Archive &archive, sparse_twocgrlike_options &data) {
	archive(data.repr, data.k_or_spacedseed, data.use_freq, data.order, data.freqcut);
}

struct descr_options {
	std::vector<double> bins;
	std::vector<uint64_t> windows;
};
template <class Archive>
void serialize(Archive &archive, descr_options &data) {
	archive(data.bins, data.windows);
}

using repr_options = boost::variant<regular_cgrlike_options, twocgrlike_options, sparse_regular_cgrlike_options,
	sparse_twocgrlike_options, descr_options>;


enum class dist_type : uint8_t { euclidean, sqeuclidean, manhattan, cosine, pearson, ssim, aid, usm };

struct otherdist_options {
	dist_type dist;
};
template <class Archive>
void serialize(Archive &archive, otherdist_options &data) {
	archive(data.dist);
}

enum class aid_formula : uint8_t { A, B };
struct aid_options {
	aid_formula formula;
};
template <class Archive>
void serialize(Archive &archive, aid_options &data) {
	archive(data.formula);
}

//currently unused
enum class usm_formula : uint8_t { ucd, ncd, ncd2, cd, cdm };
enum class usm_compressor : uint8_t { paq8, zpaq, gzip, bzip2, xz, mfcompress };
struct usm_options {
	usm_formula est;
	usm_compressor compr;
	boost::optional<uint8_t> level;
};
template <class Archive>
void serialize(Archive &archive, usm_options &data) {
	archive(data.est, data.compr, data.level);
}

using dist_options = boost::variant<otherdist_options, aid_options, usm_options>;

#endif
