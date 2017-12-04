#ifndef _LIBMMG_IO_TEXT_
#define _LIBMMG_IO_TEXT_

#include <cstddef>
#include <ostream>

namespace mmg {
	template <typename Matr>
	inline void write_matrix_text(std::ostream &stream, const Matr &matr) {
		if (matr.cols() == 0) {
			return;
		}

		for (size_t i = 0; i < matr.rows(); ++i) {
			for (size_t j = 0; j < matr.cols() - 1; ++j) {
				stream << matr(i, j) << ",";
			}
			stream << matr(i, matr.cols() - 1) << '\n';
		}
	}
}

#endif
