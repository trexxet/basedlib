#pragma once

#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>

namespace Basedlib {

// TODO: use std::function_ptr when available

/// @brief Function is a compile-time nullable function pointer wrapper, also
/// capable of handling non-capturing lambdas.
template <typename Signature> requires std::is_function_v<Signature>
class Function;

template <typename Ret, typename... Args>
struct Function <Ret (Args...)> {
	using Signature = Ret (Args...);

	Signature* ptr = nullptr;

	constexpr Function () = default;
	template<typename F> requires std::is_convertible_v<std::decay_t<F>, Signature*>
	constexpr Function (F fn) noexcept : ptr (fn) { }

	template<typename F> requires std::is_convertible_v<std::decay_t<F>, Signature*>
	constexpr Function& operator= (F fn) noexcept { ptr = fn; return *this; }

	constexpr Ret operator() (Args... args) const {
		if constexpr (std::is_void_v<Ret>) ptr (std::forward<Args> (args)...);
		else return ptr (std::forward<Args> (args)...);
	}

	constexpr explicit operator bool () const noexcept { return ptr != nullptr; }
};

/// @brief FunctionRef is similar to Function, but non-nullable.
template <typename Signature> requires std::is_function_v<Signature>
class FunctionRef;

template <typename Ret, typename... Args>
struct FunctionRef <Ret (Args...)> {
	using Signature = Ret (Args...);

	Signature* ptr;

	FunctionRef () = delete;
	FunctionRef (std::nullptr_t) = delete;
	FunctionRef& operator= (std::nullptr_t) = delete;

	template<typename F>
	requires std::is_convertible_v<std::decay_t<F>, Signature*> && (!std::same_as<std::decay_t<F>, std::nullptr_t>)
	constexpr FunctionRef (F fn) noexcept : ptr (fn) { assert (ptr); }

	template<typename F>
	requires std::is_convertible_v<std::decay_t<F>, Signature*> && (!std::same_as<std::decay_t<F>, std::nullptr_t>)
	constexpr FunctionRef& operator= (F fn) noexcept { ptr = fn; assert (ptr); return *this; }

	constexpr Ret operator() (Args... args) const {
		if constexpr (std::is_void_v<Ret>) ptr (std::forward<Args> (args)...);
		else return ptr (std::forward<Args> (args)...);
	}
};

}
