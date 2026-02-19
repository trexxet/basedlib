#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "Class.hpp"

namespace Basedlib {

// TODO: use std::function_ptr when available
template<typename Signature> requires std::is_function_v<Signature>
class Function;

template<typename Ret, typename... Args>
struct Function<Ret(Args...)> {
	using Signature = Ret(Args...);

	Signature* ptr = nullptr;

	Function () = default;
	template<typename F> requires std::is_convertible_v<F, Signature*>
	Function (F fn) : ptr (fn) { }

	template<typename F> requires std::is_convertible_v<F, Signature*>
	Function& operator= (F fn) { ptr = fn; return *this; }

	Ret operator() (Args... args) const {
		if constexpr (std::is_void_v<Ret>) ptr (std::forward<Args> (args)...);
		else return ptr (std::forward<Args> (args)...);
	}

	explicit operator bool() const { return ptr != nullptr; }
};

}
