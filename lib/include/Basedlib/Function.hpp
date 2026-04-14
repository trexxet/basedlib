#pragma once

#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>

#include "Traits.hpp"

namespace Basedlib {

//// Strip noexcept from function signature

template <typename T>
struct strip_noexcept { using Type = T; };

template <typename Ret, typename... Args>
struct strip_noexcept <Ret (Args...) noexcept> { using Type = Ret (Args...); };

template <typename T>
using strip_noexcept_t = typename strip_noexcept<T>::Type;

// TODO: use std::function_ptr when available

/// @brief Function is a compile-time nullable function pointer wrapper, also
/// capable of handling non-capturing lambdas.
template <typename Signature> requires std::is_function_v<Signature>
struct Function;

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
struct FunctionRef;

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

/// @brief FunctionTraits can be used to deduce argument/return types of functions & non-capturing lambdas
template <typename>
struct FunctionTraits;

template <typename Ret, typename... Args>
struct FunctionTraits <Ret (Args...)> {
	using ReturnType = Ret;
	static constexpr std::size_t argc = sizeof...(Args);
	template <std::size_t i>
	using ArgType = variadic_t<i, Args...>;
};

}
