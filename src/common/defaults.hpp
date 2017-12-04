#ifndef _COMMON_DEFAULTS_
#define _COMMON_DEFAULTS_

#include <string>
#include <thread>

#include "options_structs.hpp"
#include "output_headers.hpp"

namespace defaults {
	static const unsigned threads = std::thread::hardware_concurrency();
	static const unsigned blocksize = 50;
	static const std::string cgr_order = "ACGT";
	static const std::string resume_filename = "resumefile";

	static const element_type int_precision = element_type::uint64;
	static const element_type repr_float_precision = element_type::float64;
	static const element_type dist_float_precision = element_type::float32;

	static const ::aid_formula aid_formula = ::aid_formula::A;
}

#endif
