#pragma once

#include <concepts>
#include <tuple>
#include <type_traits>

namespace Basedlib {

//// Specializaton
// TODO: one day this would be standartized...
// Note: can't handle NTTP

template <typename T, template <typename...> typename Template>
inline constexpr bool is_specialization_of = false;

template <template <typename...> typename Template, typename... Args>
inline constexpr bool is_specialization_of <Template<Args...>, Template> = true;

template <typename T, template <typename...> typename Template>
concept specialization_of = is_specialization_of <std::remove_cvref_t<T>, Template>;

//// Fetch variadic template types

template <std::size_t N, typename... Ts> requires (sizeof...(Ts) > N)
using variadic_t = std::tuple_element_t <N, std::tuple<Ts...>>;

template <typename... Ts>
using first_variadic_t = variadic_t <0, Ts...>;

//// Check if all types in variadic pack are the same

template <typename... Ts>
concept all_same = (std::same_as <first_variadic_t<Ts...>, Ts> && ...);

template <typename... Ts>
concept all_same_remove_cvref = (std::same_as <
	std::remove_cvref_t <first_variadic_t<Ts...>>,
	std::remove_cvref_t <Ts>
> && ...);

}
