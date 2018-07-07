#ifndef _LIBKAMERIS_DISTANCES_SPARSE_INFORMATION_
#define _LIBKAMERIS_DISTANCES_SPARSE_INFORMATION_

#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "utils.hpp"

namespace kameris {
	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret sparse_approx_info_dist(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		size_t both = 0;
		iterate_sparse_pairs(vect1, vect2, [&both](auto /*val1*/, auto /*val2*/) {
			++both; //
		});
		size_t either = vect1.elements() + vect2.elements() - both;

		if (either) {
			return Ret(either - both) / either;
		} else {
			return 0;
		}
	}
}

#endif
