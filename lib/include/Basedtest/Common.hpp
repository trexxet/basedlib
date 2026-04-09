#pragma once

#include <format>
#include <string>

namespace Basedtest {

template <typename T>
std::string format_value (const T& val) {
	if constexpr (std::formattable <T, char>)
		return std::format ("{}", val);
	else if constexpr (requires { val.to_string(); })
		return std::string (val.to_string());
	else if constexpr (requires { to_string (val); })
		return std::string (to_string (val));
	else return "<unprintable>";
}

}
