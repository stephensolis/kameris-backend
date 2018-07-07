#ifndef _LIBKAMERIS_REPRESENTATIONS_CGR_
#define _LIBKAMERIS_REPRESENTATIONS_CGR_

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <vector>

namespace kameris {
	namespace cgr_impl {
		template <typename Index, typename Container, typename Seq, typename Vect>
		inline void fill_cgr(Container &result, const Seq &seq, unsigned int k, const Vect &order) {
			Index x = (Index(1) << (k - 1)); //x = 2^(k-1)
			Index y = (Index(1) << (k - 1)); //y = 2^(k-1)

			for (size_t i = 0; i < seq.size(); ++i) {
				//skip sequence entries not in order
				if (seq[i] != order[0] && seq[i] != order[1] && seq[i] != order[2] && seq[i] != order[3]) {
					continue;
				}

				x >>= 1; //x /= 2
				if (seq[i] == order[2] || seq[i] == order[3]) {
					x |= (Index(1) << (k - 1)); //x += 2^(k-1)
				}

				y >>= 1; //y /= 2
				if (seq[i] == order[0] || seq[i] == order[3]) {
					y |= (Index(1) << (k - 1)); //y += 2^(k-1)
				}

				if (i >= k - 1) {
					++result[(y << k) | x]; //++result[(2^k)*y + x]
				}
			}
		}
	}

	template <typename Count = uint32_t, typename Seq, typename Vect = const char *>
	inline std::vector<Count> cgr(const Seq &seq, unsigned int k, const Vect &order = "ACGT") {
		using Index = typename std::vector<Count>::size_type;

		if (2UL * k > CHAR_BIT * sizeof(Index)) {
			throw std::invalid_argument("k is too large to fit in the index");
		}

		std::vector<Count> result(Index(1) << (2 * k), 0); //4^k entries
		cgr_impl::fill_cgr<Index>(result, seq, k, order);

		return result;
	}

	template <typename Count = uint32_t, typename Index = uint64_t,
		template <typename, typename, typename...> class Map = std::map, typename Seq, typename Vect = const char *>
	inline Map<Index, Count> sparse_cgr(const Seq &seq, unsigned int k, const Vect &order = "ACGT") {
		if (2UL * k > CHAR_BIT * sizeof(Index)) {
			throw std::invalid_argument("k is too large to fit in the index");
		}

		Map<Index, Count> result;
		cgr_impl::fill_cgr<Index>(result, seq, k, order);

		return result;
	}
}

#endif
