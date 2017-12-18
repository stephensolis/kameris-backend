#ifndef _COMMON_DEFAULTS_
#define _COMMON_DEFAULTS_

#include <string>
#include <thread>
#include <utility>

#include <mmg-formats/cpp/headers.hpp>
#include "options_structs.hpp"

namespace defaults {
	static const unsigned threads = std::thread::hardware_concurrency();
	static const unsigned blocksize = 50;
	static const std::string resume_filename = "resumefile";

	static const mmg::element_type int_precision = mmg::element_type::uint64;
	static const mmg::element_type repr_float_precision = mmg::element_type::float64;
	static const mmg::element_type dist_float_precision = mmg::element_type::float32;

	static const std::string cgr_order = "ACGT";
	static const std::pair<std::string, std::string> twocgr_order = {"AG", "CT"};

	static const ::aid_formula aid_formula = ::aid_formula::A;
}

#endif
