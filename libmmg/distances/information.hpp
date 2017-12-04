#ifndef _LIBMMG_DISTANCES_INFORMATION_
#define _LIBMMG_DISTANCES_INFORMATION_

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace mmg {
	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret approx_info_dist(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		size_t either = 0, both = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			if (vect1[i] && vect2[i]) {
				++both;
			}
			if (vect1[i] || vect2[i]) {
				++either;
			}
		}

		if (either) {
			return Ret(either - both) / either;
		} else {
			return 0;
		}
	}
}

#endif
