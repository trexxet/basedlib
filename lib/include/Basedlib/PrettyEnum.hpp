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

template <typename T, size_t maxIndex = 32> requires std::is_enum_v<T>
struct PrettyEnum {
	using Enum = T;
	using Meta = Meta<T>;

private:
	template<size_t i>
	static consteval bool is_valid_idx () {
		constexpr char c = Meta::template v_name<val (i)>()[0];
		return !(c >= '0' && c <= '9');
	}

	template<size_t... i>
	static consteval size_t count_valid_idxs (std::index_sequence<i...>) {
		return (static_cast<size_t> (is_valid_idx<i>()) + ...);
	}

public:
	static constexpr size_t idx (T i) { return static_cast<size_t> (i); }
	static constexpr T val (size_t i) { return static_cast<T> (i); }
	static constexpr size_t size = count_valid_idxs (std::make_index_sequence<maxIndex>());
	static constexpr bool has_idx (size_t i) { return i < size; }

private:
	template<bool scoped>
	static constexpr auto get_names = [] <std::size_t... I> (std::index_sequence<I...>) {
		return std::array { Meta::template v_name<val (I), scoped>()... };
	};

	static constexpr auto validIdxSequence = std::make_index_sequence <size> ();
	static constexpr auto names = get_names<false> (validIdxSequence);
	static constexpr auto scoped_names = get_names<true> (validIdxSequence);

public:
	static constexpr std::string_view to_string (T value) { return names[idx (value)]; }
	static constexpr std::string_view to_scoped_string (T value) { return scoped_names[idx (value)]; }
};

template <typename> struct IsPrettyEnum : std::false_type {};
template <typename T, size_t N>
struct IsPrettyEnum<PrettyEnum<T, N>> : std::true_type {};

template <typename T>
concept PrettyEnumT = IsPrettyEnum<T>::value;

}
