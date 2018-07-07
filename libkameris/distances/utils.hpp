#ifndef _LIBKAMERIS_DISTANCES_UTILS_
#define _LIBKAMERIS_DISTANCES_UTILS_

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../utils/types.hpp"

namespace kameris {
	template <typename Ret = double, typename Vect>
	inline Ret sum(const Vect &vect) {
		extra_precision_t<vect_element_t<Vect>> total = 0;

		for (size_t i = 0; i < vect.size(); ++i) {
			total += vect[i];
		}

		return total;
	}

	template <typename Ret = double, typename Vect>
	inline Ret mean(const Vect &vect) {
		if (vect.size() == 0) {
			throw std::invalid_argument("Vector cannot be empty");
		}

		return sum<Ret>(vect) / vect.size();
	}

	template <typename Ret = int64_t, typename Vect1, typename Vect2>
	inline Ret dot_product(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;

		for (size_t i = 0; i < vect1.size(); ++i) {
			result += vect1[i] * vect2[i];
		}

		return result;
	}

	template <typename Vect1, typename Vect2>
	inline void add_to(Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.size() != vect2.size()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		for (size_t i = 0; i < vect1.size(); ++i) {
			vect1[i] += vect2[i];
		}
	}
}

#endif
