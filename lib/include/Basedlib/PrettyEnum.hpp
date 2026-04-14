#pragma once

#include <array>
#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Meta.hpp"

/* This would work only for dense enums (values go from 0 to N - 1) */

/* TODO: use C++26 static reflection when available */

namespace Basedlib {

template <typename T, std::size_t maxIndex = 32> requires std::is_enum_v<T>
struct PrettyEnum {
	using Enum = T;
	using Reflection = Reflection<T>;

private:
	template<std::size_t i>
	static consteval bool is_valid_idx () {
		constexpr char c = Reflection::template v_name<val (i)>()[0];
		return !(c >= '0' && c <= '9');
	}

	template<std::size_t... i>
	static consteval std::size_t count_valid_idxs (std::index_sequence<i...>) {
		return (static_cast<std::size_t> (is_valid_idx<i>()) + ...);
	}

public:
	static constexpr std::size_t idx (T i) noexcept { return static_cast<std::size_t> (i); }
	static constexpr T val (std::size_t i) noexcept { return static_cast<T> (i); }
	static constexpr std::size_t size = count_valid_idxs (std::make_index_sequence<maxIndex>());
	static constexpr bool has_idx (std::size_t i) noexcept { return i < size; }

private:
	template<bool scoped>
	static constexpr auto get_names = [] <std::size_t... I> (std::index_sequence<I...>) {
		return std::array { Reflection::template v_name<val (I), scoped>()... };
	};

	static constexpr auto validIdxSequence = std::make_index_sequence <size> ();
	static constexpr auto names = get_names<false> (validIdxSequence);
	static constexpr auto scoped_names = get_names<true> (validIdxSequence);

public:
	static constexpr std::string_view to_string (T value) { return names[idx (value)]; }
	static constexpr std::string_view to_scoped_string (T value) { return scoped_names[idx (value)]; }
};

template <typename>
inline constexpr bool is_PrettyEnum = false;
template <typename T, std::size_t N>
inline constexpr bool is_PrettyEnum <PrettyEnum<T, N>> = true;
template <typename T>
concept PrettyEnumT = is_PrettyEnum <std::remove_cvref_t<T>>;

}
