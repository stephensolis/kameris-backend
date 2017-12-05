#ifndef _LIBMMG_UTILS_RANDOM_
#define _LIBMMG_UTILS_RANDOM_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace mmg {
	template <typename T>
	inline std::vector<T> random_vect(size_t size, T min, T max) {
		using namespace std;

		using distribution = typename conditional<is_integral<T>::value, //
			uniform_int_distribution<T>, //
			uniform_real_distribution<T>>::type;

		static mt19937 gen(random_device{}());
		distribution dist(min, max);

		vector<T> out;
		out.reserve(size);
		generate_n(back_inserter(out), size, [&]() { return dist(gen); });

		return out;
	}

	template <typename Vect>
	inline std::string random_string(size_t size, const Vect &choices) {
		using namespace std;

		if (choices.size() == 0) {
			throw std::invalid_argument("At least one choice must be given");
		}

		static mt19937 gen(random_device{}());
		uniform_int_distribution<int> dist(0, int(choices.size()) - 1);

		string out;
		out.reserve(size);
		generate_n(back_inserter(out), size, [&]() { return choices[dist(gen)]; });

		return out;
	}
}

#endif
