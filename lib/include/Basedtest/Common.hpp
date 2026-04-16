#pragma once

#include <format>
#include <string>
#include <type_traits>
#include <utility>

namespace Basedtest {

template <typename T>
std::string format_value (const T& val) {
	if constexpr (std::formattable <T, char>)
		return std::format ("{}", val);
	else if constexpr (requires { val.to_string(); })
		return std::string (val.to_string());
	else if constexpr (requires { to_string (val); })
		return std::string (to_string (val));
	else if constexpr (std::is_enum_v <T>)
		return std::format ("{}", std::to_underlying (val));
	else return "<unprintable>";
}

/// @brief Barrier for compile-time optimization.
/// Effectively tells compiler to forget what's inside variable.
template <typename T> [[gnu::noinline]]
T black_box (T val) {
	asm volatile ("" : "+rm"(val) : : "memory"); // GCC SUPREMACY
	return val;
}

}
