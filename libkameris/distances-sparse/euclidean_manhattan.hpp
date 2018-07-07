#ifndef _LIBKAMERIS_DISTANCES_SPARSE_EUCLIDEAN_MANHATTAN_
#define _LIBKAMERIS_DISTANCES_SPARSE_EUCLIDEAN_MANHATTAN_

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "../utils/math.hpp"
#include "utils.hpp"

namespace kameris {
	template <typename Ret = uint64_t, typename Vect1, typename Vect2>
	inline Ret sparse_squared_euclidean(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;
		iterate_sparse_pairs(vect1, vect2,
			[&result](auto val1, auto val2) {
				result += square(val1 - val2); //
			},
			[&result](auto val) {
				result += square(val); //
			});

		return result;
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret sparse_euclidean(const Vect1 &vect1, const Vect2 &vect2) {
		return std::sqrt(sparse_squared_euclidean<Ret>(vect1, vect2));
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret sparse_chi_squared(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;
		iterate_sparse_pairs(vect1, vect2,
			[&result](auto val1, auto val2) {
				Ret sum = Ret(val1 + val2);

				if (sum) {
					result += square(val1 - val2) / sum;
				}
			},
			[&result](auto val) {
				result += val; //
			});

		return result;
	}

	template <typename Ret = uint64_t, typename Vect1, typename Vect2>
	inline Ret sparse_manhattan(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;
		iterate_sparse_pairs(vect1, vect2,
			[&result](auto val1, auto val2) {
				result += absdiff(val1, val2); //
			},
			[&result](auto val) {
				if (is_unsigned(val)) {
					result += val;
				} else {
					result += std::abs(val);
				}
			});

		return result;
	}
}

#endif
