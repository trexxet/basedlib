#pragma once

#include <type_traits>

namespace Basedlib {

// TODO: one day this would be standartized...
// Note: can't handle NTTP

template <typename T, template <typename...> typename Template>
inline constexpr bool is_specialization_of = false;

template <template <typename...> typename Template, typename... Args>
inline constexpr bool is_specialization_of <Template<Args...>, Template> = true;

template <typename T, template <typename...> typename Template>
concept specialization_of = is_specialization_of <std::remove_cvref_t<T>, Template>;

}
