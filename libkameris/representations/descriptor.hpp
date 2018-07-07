#ifndef _LIBKAMERIS_REPRESENTATIONS_DESCRIPTOR_
#define _LIBKAMERIS_REPRESENTATIONS_DESCRIPTOR_

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

namespace kameris {
	template <typename Ret = double, typename Count = uint32_t, typename Img, typename Vect1, typename Vect2>
	inline std::vector<Ret> descriptor(const Img &img, const Vect1 &winsizes, const Vect2 &histbins) {
		if (img.rows() == 0 || img.cols() == 0) {
			throw std::invalid_argument("Matrix cannot be empty");
		}

		std::vector<Ret> result;

		for (auto winsize : winsizes) {
			if (winsize <= 0) {
				throw std::invalid_argument("Window size must be positive");
			}
			if (img.rows() < winsize || img.cols() < winsize) {
				throw std::invalid_argument("Matrix is too small for the given window size");
			}

			for (size_t i = 0; i <= img.rows() - winsize; i += winsize) {
				for (size_t j = 0; j <= img.cols() - winsize; j += winsize) {
					std::vector<Count> curr_counts(histbins.size(), 0);

					for (size_t m = i; m < i + winsize; ++m) {
						for (size_t n = j; n < j + winsize; ++n) {
							for (size_t b = 0; b < histbins.size() - 1; ++b) {
								if (histbins[b] <= img(m, n) && img(m, n) < histbins[b + 1]) {
									++curr_counts[b];
									goto end;
								}
							}

							++curr_counts[histbins.size() - 1];
						end:;
						}
					}

					for (size_t b = 0; b < histbins.size(); ++b) {
						result.push_back(Ret(curr_counts[b]) / (winsize * winsize));
					}
				}
			}
		}

		return result;
	}
}

#endif
