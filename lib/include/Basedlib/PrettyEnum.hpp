#pragma once

#include <array>
#include <concepts>
#include <string_view>
#include <utility>

#include "Basedlib/Meta.hpp"

/* This would work only for enums that are:
 * 1) Dense (values go from 0 to N - 1)
 * 2) Have the last value COUNT = N
 */

#define Bl_PrettyEnum(name, ...) enum class name { __VA_ARGS__, COUNT }

namespace Basedlib {

template <typename T>
concept PrettyEnumT = std::is_scoped_enum_v<T> && requires { T::COUNT; };

template <PrettyEnumT T, bool scoped = false>
class PrettyEnum {
public:
	static constexpr size_t idx (T i) { return static_cast<size_t> (i); }
	static constexpr T val (size_t i) { return static_cast<T> (i); }

private:
	static constexpr auto names = [] <std::size_t... I> (std::index_sequence<I...>) {
		return std::array { Meta<T>::template v_name<val (I), scoped>()... };
	} (std::make_index_sequence <idx (T::COUNT)>());

public:
	static constexpr std::string_view to_string (T value) { return names[idx (value)]; }
};

}
