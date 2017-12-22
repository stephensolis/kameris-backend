#ifndef _MMG_FORMATS_ELEMENT_TYPE_
#define _MMG_FORMATS_ELEMENT_TYPE_

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#include <boost/mpl/at.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>

namespace mmg {
	// clang-format off
	enum class element_type : uint8_t {
		uint8, uint16, uint32, uint64, //unsigned integral types
		float32, float64 //float types
	};
	// clang-format on
	using element_underlying_t = typename std::underlying_type<element_type>::type;

	using type_to_element_type_map = boost::mpl::map< //
		boost::mpl::pair<uint8_t, std::integral_constant<element_type, element_type::uint8>>,
		boost::mpl::pair<uint16_t, std::integral_constant<element_type, element_type::uint16>>,
		boost::mpl::pair<uint32_t, std::integral_constant<element_type, element_type::uint32>>,
		boost::mpl::pair<uint64_t, std::integral_constant<element_type, element_type::uint64>>,
		boost::mpl::pair<float, std::integral_constant<element_type, element_type::float32>>,
		boost::mpl::pair<double, std::integral_constant<element_type, element_type::float64>>>;

	using element_type_types = typename boost::mpl::transform<type_to_element_type_map,
		boost::mpl::first<boost::mpl::_1>, boost::mpl::back_inserter<boost::mpl::vector<>>>::type;

	template <typename T>
	constexpr element_type element_type_for_type = boost::mpl::at<type_to_element_type_map, T>::type::value;

	template <typename Func>
	void dispatch_on_element_type(element_type t, Func func) {
		bool did_dispatch = false;
		boost::mpl::for_each<element_type_types>([&](auto dummy) {
			using T = decltype(dummy);
			if (element_type_for_type<T> == t) {
				func.template operator()<T>();
				did_dispatch = true;
			}
		});
		if (!did_dispatch) {
			throw std::invalid_argument("Unknown value for element_type");
		}
	}
}

#endif
