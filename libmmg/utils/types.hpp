#ifndef _LIBMMG_UTILS_TYPES_
#define _LIBMMG_UTILS_TYPES_

#include <cstdint>
#include <type_traits>
#include <utility>

#include <boost/mpl/at.hpp>
#include <boost/mpl/map.hpp>

namespace mmg {
	namespace types_impl {
		using extra_precision_map = boost::mpl::map<
			//signed
			boost::mpl::pair<int8_t, int16_t>, //
			boost::mpl::pair<int16_t, int32_t>, //
			boost::mpl::pair<int32_t, int64_t>, //
			boost::mpl::pair<int64_t, int64_t>, //
			//unsigned
			boost::mpl::pair<uint8_t, uint16_t>, //
			boost::mpl::pair<uint16_t, uint32_t>, //
			boost::mpl::pair<uint32_t, uint64_t>, //
			boost::mpl::pair<uint64_t, uint64_t>, //
			//float
			boost::mpl::pair<float, double>, //
			boost::mpl::pair<double, double>>;
	}

	template <typename T>
	using extra_precision_t = typename boost::mpl::at<types_impl::extra_precision_map, T>::type;

	//ugly hack to work around a MSVC2015 bug
	struct vect_element_impl {
		template <typename T>
		auto operator()(const T &x) -> decltype(x[0]) {
			return x[0];
		}
	};

	template <typename Vect>
	using vect_element_t = std::decay_t<typename std::result_of<vect_element_impl(Vect)>::type>;

	template <typename... Vect>
	using common_vect_element_t = std::common_type_t<vect_element_t<Vect>...>;

	//ugly hack to work around a MSVC2015 bug
	struct sparse_vect_element_impl {
		template <typename T>
		auto operator()(const T &x) -> decltype(x.begin()->second) {
			return x.begin()->second;
		}
	};

	template <typename Vect>
	using sparse_vect_element_t = std::decay_t<typename std::result_of<sparse_vect_element_impl(Vect)>::type>;

	template <typename... Vect>
	using common_sparse_vect_element_t = std::common_type_t<sparse_vect_element_t<Vect>...>;
}

#endif
