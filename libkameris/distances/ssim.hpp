#ifndef _LIBKAMERIS_DISTANCES_SSIM_
#define _LIBKAMERIS_DISTANCES_SSIM_

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <stdexcept>

#include "../utils/matrix_vector_adapters.hpp"

namespace kameris {
	namespace ssim_impl {
		template <typename T>
		struct ssim_constants {
			//1D Gaussian, radius = 5, sigma = 1.5
			static constexpr T gaussian_kernel[11] = {0.0019501069320213926, 0.00902231675565504, 0.034029455396572636,
				0.09976753114651542, 0.21139395521260004, 0.28767326911327096, 0.21139395521260004, 0.09976753114651542,
				0.034029455396572636, 0.00902231675565504, 0.0019501069320213926};
		};

		template <typename T>
		constexpr T ssim_constants<T>::gaussian_kernel[11]; // NOLINT

		template <typename Ret = double, typename Img>
		inline Ret correlate_one_horiz_el(const Img &img, size_t yIndex, size_t xIndex) {
			Ret result = 0;

			for (size_t i = 0; i < 11; ++i) {
				result += ssim_constants<Ret>::gaussian_kernel[i] * img(yIndex, xIndex + i);
			}

			return result;
		}

		template <typename Ret = double, typename Img>
		inline Ret correlate_one_vert_el(const Img &img, size_t yIndex, size_t xIndex) {
			Ret result = 0;

			for (size_t i = 0; i < 11; ++i) {
				result += ssim_constants<Ret>::gaussian_kernel[i] * img(yIndex + i, xIndex);
			}

			return result;
		}

		template <typename Ret = double, typename Img1, typename Img2>
		inline Ret correlate_two_horiz_el(const Img1 &img1, const Img2 &img2, size_t yIndex, size_t xIndex) {
			Ret result = 0;

			for (size_t i = 0; i < 11; ++i) {
				result += ssim_constants<Ret>::gaussian_kernel[i] * img1(yIndex, xIndex + i) * img2(yIndex, xIndex + i);
			}

			return result;
		}
	}

	template <typename Ret = double, typename Img1, typename Img2>
	inline Ret ssim(const Img1 &img1, const Img2 &img2) {
		if (img1.rows() != img2.rows() || img1.cols() != img2.cols()) {
			throw std::invalid_argument("Matrices must have the same dimensions");
		}
		if (img1.rows() < 11 || img1.cols() < 11) {
			throw std::invalid_argument("Matrices must be at least 11x11");
		}

		size_t horiz_data_size = img1.rows() * (img1.cols() - 10);
		std::unique_ptr<Ret[]> horiz_data(new Ret[5 * horiz_data_size]);
		MatrixAdapter<Ret> img1_horiz = make_matrix_adapter(&horiz_data[0], img1.rows(), img1.cols() - 10);
		MatrixAdapter<Ret> img2_horiz =
			make_matrix_adapter(&horiz_data[horiz_data_size], img1.rows(), img1.cols() - 10);
		MatrixAdapter<Ret> img11_horiz =
			make_matrix_adapter(&horiz_data[2 * horiz_data_size], img1.rows(), img1.cols() - 10);
		MatrixAdapter<Ret> img22_horiz =
			make_matrix_adapter(&horiz_data[3 * horiz_data_size], img1.rows(), img1.cols() - 10);
		MatrixAdapter<Ret> img12_horiz =
			make_matrix_adapter(&horiz_data[4 * horiz_data_size], img1.rows(), img1.cols() - 10);

		for (size_t i = 0; i < img1.rows(); ++i) {
			for (size_t j = 0; j < size_t(img1.cols() - 10); ++j) {
				img1_horiz(i, j) = ssim_impl::correlate_one_horiz_el<Ret>(img1, i, j);
				img2_horiz(i, j) = ssim_impl::correlate_one_horiz_el<Ret>(img2, i, j);
				img11_horiz(i, j) = ssim_impl::correlate_two_horiz_el<Ret>(img1, img1, i, j);
				img22_horiz(i, j) = ssim_impl::correlate_two_horiz_el<Ret>(img2, img2, i, j);
				img12_horiz(i, j) = ssim_impl::correlate_two_horiz_el<Ret>(img1, img2, i, j);
			}
		}

		Ret result = 0;
		for (size_t i = 0; i < size_t(img1.rows() - 10); ++i) {
			for (size_t j = 0; j < size_t(img1.cols() - 10); ++j) {
				Ret m1 = ssim_impl::correlate_one_vert_el<Ret>(img1_horiz, i, j);
				Ret m2 = ssim_impl::correlate_one_vert_el<Ret>(img2_horiz, i, j);

				Ret m1sq = m1 * m1, m2sq = m2 * m2, m1m2 = m1 * m2;

				Ret sigma1sq = ssim_impl::correlate_one_vert_el<Ret>(img11_horiz, i, j) - m1sq;
				Ret sigma2sq = ssim_impl::correlate_one_vert_el<Ret>(img22_horiz, i, j) - m2sq;
				Ret sigma12 = ssim_impl::correlate_one_vert_el<Ret>(img12_horiz, i, j) - m1m2;

				result += ((0.0001 + (2 * m1m2)) * (0.0009 + (2 * sigma12))) /
					((0.0001 + m1sq + m2sq) * (0.0009 + sigma1sq + sigma2sq));
			}
		}

		return result / ((img1.rows() - 10) * (img1.cols() - 10));
	}
}

#endif
