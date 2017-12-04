#ifndef _LIBMMG_UTILS_TIME_
#define _LIBMMG_UTILS_TIME_

#include <chrono>

namespace mmg {
	template <typename Func>
	inline double timeit(Func func) {
		using namespace std::chrono;

		steady_clock::time_point start = steady_clock::now();
		func();
		steady_clock::time_point end = steady_clock::now();

		return duration_cast<duration<double>>(end - start).count();
	}
}

#endif
