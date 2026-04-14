#pragma once

#include <string_view>

/* Probably works only for GCC/clang, don't care about other compilers */

namespace Basedlib {

template <typename T>
struct Meta {
	static consteval std::string_view t_name () {
		std::string_view name = __PRETTY_FUNCTION__;
		std::size_t start = name.find ("T = ") + 4;
		std::size_t end = name.find_first_of (";]", start);
		return name.substr (start, end - start);
	}

	template <T V, bool scoped = false>
	static consteval std::string_view v_name () {
		std::string_view name = __PRETTY_FUNCTION__;
		std::size_t start = name.find ("V = ") + 4;
		std::size_t end = name.find_first_of (";]", start);
		name = name.substr (start, end - start);

		// Cut type conversion, if any
		start = name.rfind (')');
		if (start != std::string_view::npos)
			name = name.substr (start + 1);

		if constexpr (!scoped) {
			std::size_t pos = name.rfind("::");
			if (pos != std::string_view::npos)
				name.remove_prefix (pos + 2);
		}

		return name;
	}
};

}
