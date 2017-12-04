#ifndef _LIBMMG_DISTANCES_SPARSE_COSINE_PEARSON_
#define _LIBMMG_DISTANCES_SPARSE_COSINE_PEARSON_

#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include "../utils/math.hpp"
#include "../utils/types.hpp"
#include "utils.hpp"

namespace mmg {
	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret sparse_cosine(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		extra_precision_t<common_sparse_vect_element_t<Vect1, Vect2>> dot = 0, norm1 = 0, norm2 = 0;
		iterate_sparse_pairs(vect1, vect2,
			[&dot, &norm1, &norm2](auto val1, auto val2) {
				dot += val1 * val2;
				norm1 += square(val1);
				norm2 += square(val2);
			},
			[&norm1](auto val) {
				norm1 += square(val); //
			},
			[&norm2](auto val) {
				norm2 += square(val); //
			});

		if (norm1 && norm2) {
			return 1 - (Ret(dot) / std::sqrt(norm1 * norm2));
		} else {
			return 0;
		}
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret sparse_pearson(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret dot = 0, norm1 = 0, norm2 = 0;
		Ret mean1 = sparse_mean<Ret>(vect1);
		Ret mean2 = sparse_mean<Ret>(vect2);
		size_t both = 0;

		iterate_sparse_pairs(vect1, vect2,
			[mean1, mean2, &dot, &norm1, &norm2, &both](auto val1, auto val2) {
				dot += (val1 - mean1) * (val2 - mean2);
				norm1 += square(val1 - mean1);
				norm2 += square(val2 - mean2);
				++both;
			},
			[mean1, mean2, &dot, &norm1](auto val1) {
				dot += (val1 - mean1) * (-mean2);
				norm1 += square(val1 - mean1);
			},
			[mean1, mean2, &dot, &norm2](auto val2) {
				dot += (-mean1) * (val2 - mean2);
				norm2 += square(val2 - mean2);
			});

		size_t neither = vect1.length() - (vect1.elements() + vect2.elements() - both);
		dot += neither * (mean1 * mean2);
		norm1 += (vect1.length() - vect1.elements()) * square(mean1);
		norm2 += (vect2.length() - vect2.elements()) * square(mean2);

		if (norm1 && norm2) {
			return 1 - (dot / std::sqrt(norm1 * norm2));
		} else {
			return 0;
		}
	}
}

#endif
