#ifndef _LIBKAMERIS_DISTANCES_EUCLIDEAN_MANHATTAN_
#define _LIBKAMERIS_DISTANCES_EUCLIDEAN_MANHATTAN_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include "../utils/math.hpp"

namespace kameris {
	template <typename Ret = uint64_t, typename Vect1, typename Vect2>
	inline Ret squared_euclidean(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			result += square(vect1[i] - vect2[i]);
		}

		return result;
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret euclidean(const Vect1 &vect1, const Vect2 &vect2) {
		return std::sqrt(squared_euclidean<Ret>(vect1, vect2));
	}

	template <typename Ret = double, typename Vect1, typename Vect2>
	inline Ret chi_squared(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			Ret sum = Ret(vect1[i] + vect2[i]);

			if (sum) {
				result += square(vect1[i] - vect2[i]) / sum;
			}
		}

		return result;
	}

	template <typename Ret = uint64_t, typename Vect1, typename Vect2>
	inline Ret manhattan(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			result += absdiff(vect1[i], vect2[i]);
		}

		return result;
	}
}

#endif
