#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace Basedlib {

// TODO: use std::function_ptr when available
template<typename Signature> requires std::is_function_v<Signature>
class Function;

template<typename Ret, typename... Args>
struct Function<Ret(Args...)> {
	using Signature = Ret(Args...);

	Signature* ptr = nullptr;

	constexpr Function () = default;
	template<typename F> requires std::is_convertible_v<F, Signature*>
	constexpr Function (F fn) : ptr (fn) { }

	template<typename F> requires std::is_convertible_v<F, Signature*>
	constexpr Function& operator= (F fn) { ptr = fn; return *this; }

	constexpr Ret operator() (Args... args) const {
		if constexpr (std::is_void_v<Ret>) ptr (std::forward<Args> (args)...);
		else return ptr (std::forward<Args> (args)...);
	}

	constexpr explicit operator bool() const { return ptr != nullptr; }
};

}
