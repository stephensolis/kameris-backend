#ifndef _COMMON_OPTIONS_STRUCTS_
#define _COMMON_OPTIONS_STRUCTS_

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "output_headers.hpp"

struct program_options {
	std::string in1_path;
	std::string in2_path;
	std::string out_path;

	unsigned threads;
	unsigned blocksize;

	bool use_cuda;
	bool use_opencl;
	element_type int_precision;
	element_type float_precision;
};


struct cgrlike_options {
	repr_type repr;

	boost::variant<unsigned, std::vector<bool>> k_or_spacedseed;
	bool use_freq;
	std::string order;
};

struct sparse_cgrlike_options : cgrlike_options {
	double freqcut = 0;
};

struct descr_options {
	std::vector<double> bins;
	std::vector<unsigned> windows;
};

using repr_options = boost::variant<cgrlike_options, sparse_cgrlike_options, descr_options>;


enum class dist_type : uint8_t { euclidean, sqeuclidean, manhattan, cosine, pearson, ssim, aid, usm };

struct otherdist_options {
	dist_type dist;
};

enum class aid_formula : uint8_t { A, B };
struct aid_options {
	aid_formula formula;
};

enum class usm_formula : uint8_t { ucd, ncd, ncd2, cd, cdm };
enum class usm_compressor : uint8_t { paq8, zpaq, gzip, bzip2, xz, mfcompress };
struct usm_options {
	usm_formula est;
	usm_compressor compr;
	boost::optional<unsigned> level;
};

using dist_options = boost::variant<otherdist_options, aid_options, usm_options>;

#endif
