#ifndef _LIBKAMERIS_REPRESENTATIONS_TWOCGR_
#define _LIBKAMERIS_REPRESENTATIONS_TWOCGR_

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace kameris {
	namespace twocgr_impl {
		template <typename Index, template <typename, typename...> class CommonSet, typename Container, typename Seq,
			typename Set1, typename Set2>
		inline void fill_twocgr(Container &result, const Seq &seq, unsigned int k, const Set1 &order_first,
			/**/ const Set2 &order_second) {
			Index x = (Index(1) << (k - 1)); //x = 2^(k-1)

			CommonSet<std::common_type_t<typename Set1::key_type, typename Set2::key_type>> all_chars;
			all_chars.insert(order_first.cbegin(), order_first.cend());
			all_chars.insert(order_second.cbegin(), order_second.cend());

			for (size_t i = 0; i < seq.size(); ++i) {
				//skip sequence entries not in order
				if (!all_chars.count(seq[i])) {
					continue;
				}

				x >>= 1; //x /= 2
				if (!order_first.count(seq[i])) {
					x |= (Index(1) << (k - 1)); //x += 2^(k-1)
				}

				if (i >= k - 1) {
					++result[x];
				}
			}
		}
	}

	template <typename Count = uint32_t, template <typename, typename...> class CommonSet = std::unordered_set,
		typename Seq, typename Set1 = std::unordered_set<char>, typename Set2 = std::unordered_set<char>>
	inline std::vector<Count> twocgr(const Seq &seq, unsigned int k, const Set1 &order_first = {'A', 'G'},
		/**/ const Set2 &order_second = {'C', 'T'}) {
		using Index = typename std::vector<Count>::size_type;

		if (k > CHAR_BIT * sizeof(Index)) {
			throw std::invalid_argument("k is too large to fit in the index");
		}

		std::vector<Count> result(Index(1) << k, 0); //2^k entries
		twocgr_impl::fill_twocgr<Index, CommonSet>(result, seq, k, order_first, order_second);

		return result;
	}

	template <typename Count = uint32_t, typename Index = uint64_t,
		template <typename, typename, typename...> class Map = std::map,
		template <typename, typename...> class CommonSet = std::unordered_set, typename Seq,
		typename Set1 = std::unordered_set<char>, typename Set2 = std::unordered_set<char>>
	inline Map<Index, Count> sparse_twocgr(const Seq &seq, unsigned int k, const Set1 &order_first = {'A', 'G'},
		/**/ const Set2 &order_second = {'C', 'T'}) {
		if (k > CHAR_BIT * sizeof(Index)) {
			throw std::invalid_argument("k is too large to fit in the index");
		}

		Map<Index, Count> result;
		twocgr_impl::fill_twocgr<Index, CommonSet>(result, seq, k, order_first, order_second);

		return result;
	}
}

#endif
