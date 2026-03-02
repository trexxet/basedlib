#pragma once

#include <array>
#include <concepts>
#include <string_view>
#include <utility>

#include "Meta.hpp"

/* This would work only for enums that are:
 * 1) Dense (values go from 0 to N - 1)
 * 2) Have the last value _bl_count = N
 */

#define Bl_PrettyEnum(name, ...) enum class name { __VA_ARGS__, _bl_count }

namespace Basedlib {

template <typename T>
concept PrettyEnumT = std::is_scoped_enum_v<T> && requires { T::_bl_count; };

template <PrettyEnumT T>
struct PrettyEnum {
	using Meta = Meta<T>;

	static constexpr size_t idx (T i) { return static_cast<size_t> (i); }
	static constexpr T val (size_t i) { return static_cast<T> (i); }
	static constexpr size_t size () { return idx (T::_bl_count); }
	static constexpr bool has_idx (size_t i) { return i < size(); }

private:
	template<bool scoped>
	static constexpr auto get_names = [] <std::size_t... I> (std::index_sequence<I...>) {
		return std::array { Meta::template v_name<val (I), scoped>()... };
	};
	static constexpr auto idx_sequence = std::make_index_sequence <size()> ();
	static constexpr auto names = get_names<false> (idx_sequence);
	static constexpr auto scoped_names = get_names<true> (idx_sequence);

public:
	static constexpr std::string_view to_string (T value) { return names[idx (value)]; }
	static constexpr std::string_view to_scoped_string (T value) { return scoped_names[idx (value)]; }
};

}
