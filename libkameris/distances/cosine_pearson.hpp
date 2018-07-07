#ifndef _LIBKAMERIS_DISTANCES_COSINE_PEARSON_
#define _LIBKAMERIS_DISTANCES_COSINE_PEARSON_

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>

#include "../utils/math.hpp"
#include "../utils/types.hpp"
#include "utils.hpp"

namespace kameris {
	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret cosine(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		extra_precision_t<common_vect_element_t<Vect1, Vect2>> dot = 0, norm1 = 0, norm2 = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			dot += vect1[i] * vect2[i];
			norm1 += square(vect1[i]);
			norm2 += square(vect2[i]);
		}

		if (norm1 && norm2) {
			return 1 - (Ret(dot) / std::sqrt(norm1 * norm2));
		} else {
			return 0;
		}
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret pearson(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret dot = 0, norm1 = 0, norm2 = 0;
		Ret mean1 = mean<Ret>(vect1);
		Ret mean2 = mean<Ret>(vect2);

		for (size_t i = 0; i < vect1.size(); ++i) {
			dot += (vect1[i] - mean1) * (vect2[i] - mean2);
			norm1 += square(vect1[i] - mean1);
			norm2 += square(vect2[i] - mean2);
		}

		if (norm1 && norm2) {
			return 1 - (dot / std::sqrt(norm1 * norm2));
		} else {
			return 0;
		}
	}
}

#endif
