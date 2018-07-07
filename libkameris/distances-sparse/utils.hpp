#ifndef _LIBKAMERIS_DISTANCES_SPARSE_UTILS_
#define _LIBKAMERIS_DISTANCES_SPARSE_UTILS_

#include <cstdint>
#include <stdexcept>

#include "../utils/types.hpp"

namespace kameris {
	template <typename Vect1, typename Vect2, typename Func1, typename Func2, typename Func3>
	inline void iterate_sparse_pairs(const Vect1 &vect1, const Vect2 &vect2, Func1 on_two_vals, Func2 on_val1,
		/**/ Func3 on_val2) {
		auto vect1_it = vect1.begin(), vect2_it = vect2.begin();

		while (vect1_it != vect1.end() && vect2_it != vect2.end()) {
			if (vect1_it->first == vect2_it->first) {
				on_two_vals(vect1_it->second, vect2_it->second);
				++vect1_it;
				++vect2_it;
			} else if (vect1_it->first < vect2_it->first) {
				on_val1(vect1_it->second);
				++vect1_it;
			} else {
				on_val2(vect2_it->second);
				++vect2_it;
			}
		}
		while (vect1_it != vect1.end()) {
			on_val1(vect1_it->second);
			++vect1_it;
		}
		while (vect2_it != vect2.end()) {
			on_val2(vect2_it->second);
			++vect2_it;
		}
	}

	template <typename Vect1, typename Vect2, typename Func1, typename Func2>
	inline void iterate_sparse_pairs(const Vect1 &vect1, const Vect2 &vect2, Func1 on_two_vals, Func2 on_one_val) {
		iterate_sparse_pairs(vect1, vect2, on_two_vals, on_one_val, on_one_val);
	}

	template <typename Vect1, typename Vect2, typename Func1>
	inline void iterate_sparse_pairs(const Vect1 &vect1, const Vect2 &vect2, Func1 on_two_vals) {
		auto vect1_it = vect1.begin(), vect2_it = vect2.begin();

		while (vect1_it != vect1.end() && vect2_it != vect2.end()) {
			if (vect1_it->first == vect2_it->first) {
				on_two_vals(vect1_it->second, vect2_it->second);
				++vect1_it;
				++vect2_it;
			} else if (vect1_it->first < vect2_it->first) {
				++vect1_it;
			} else {
				++vect2_it;
			}
		}
	}

	template <typename Ret = double, typename Vect>
	inline Ret sparse_sum(const Vect &vect) {
		extra_precision_t<sparse_vect_element_t<Vect>> total = 0;

		for (const auto &el : vect) {
			total += el.second;
		}

		return total;
	}

	template <typename Ret = double, typename Vect>
	inline Ret sparse_mean(const Vect &vect) {
		if (vect.length() == 0) {
			throw std::invalid_argument("Vector cannot be empty");
		}

		return sparse_sum<Ret>(vect) / vect.length();
	}

	template <typename Ret = int64_t, typename Vect1, typename Vect2>
	inline Ret sparse_dot_product(const Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		Ret result = 0;
		iterate_sparse_pairs(vect1, vect2, [&result](auto val1, auto val2) {
			result += val1 * val2; //
		});

		return result;
	}

	template <typename Vect1, typename Vect2>
	inline void sparse_add_to(Vect1 &vect1, const Vect2 &vect2) {
		if (vect1.length() != vect2.length()) {
			throw std::invalid_argument("Vectors must have the same length");
		}

		auto &vect1_data = vect1.data();
		for (const auto &val : vect2.data()) {
			vect1_data[val.first] += val.second;
		}
	}
}

#endif
