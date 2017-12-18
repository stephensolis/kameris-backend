#ifndef _MMG_FORMATS_COMMON_PSEUDO_CAST_
#define _MMG_FORMATS_COMMON_PSEUDO_CAST_

#include <cstring>

namespace mmg {
	template <typename To, typename From>
	inline To pseudo_cast(const From &from) {
		static_assert(sizeof(From) == sizeof(To), "Types given to pseudo_cast must be of the same size");

		To to;
		std::memcpy(&to, &from, sizeof(To));
		return to;
	}
}

#endif
