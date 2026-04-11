#pragma once

#include <concepts>
#include <expected>
#include <string_view>

#include "Basedlib/Function.hpp"
#include "Basedlib/Traits.hpp"

#include "Common.hpp"

namespace Basedtest {

template <typename T>
concept OutputT = std::equality_comparable <T>;

template <typename Input, OutputT Output>
struct ValueTestFunction_S { using Type = Basedlib::FunctionRef <Output (const Input&)>; };

template <OutputT Output>
struct ValueTestFunction_S <void, Output> { using Type = Basedlib::FunctionRef <Output ()>; };

template <typename Input, OutputT Output>
using ValueTestFunction = typename ValueTestFunction_S<Input, Output>::Type;

template <typename Fn, typename Input, typename Output>
concept ValueTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, ValueTestFunction <Input, Output>>;

template <auto Fn>
requires std::is_pointer_v<decltype(+Fn)> && std::is_function_v<std::remove_pointer_t<decltype(+Fn)>>
inline constexpr auto input_as_constref = [] {
	using Signature = Basedlib::strip_noexcept_t <std::remove_pointer_t <decltype(+Fn)>>;
	using Traits = Basedlib::FunctionTraits <Signature>;
	static_assert (Traits::argc == 1, "Function must be unary");
	using Input = std::remove_cvref_t <typename Traits::ArgType<0>>;
	using Output = Traits::ReturnType;
	return ValueTestFunction<Input, Output> ([] (const Input& input) -> Output { return Fn (input); } );
} ();

/// @brief ValueFailure stores the expected and got value for failed test
template <OutputT Output>
struct ValueFailure {
	Output expected, got;
};

template <OutputT Output>
using ValueTestResult = std::expected <void, ValueFailure <Output>>;

/// @brief ValueTest runs fn(const input&) and compares it's result with the expected one.
/// ValueTests can be used with or without the Suite.
template <typename Input, OutputT Output>
struct ValueTest {
	std::string_view name;
	Input input;
	Output expected;
	ValueTestFunction <Input, Output> fn;

	[[nodiscard]]
	ValueTestResult <Output> run () const {
		Output got = fn (input);
		if (got != expected) [[unlikely]]
			return std::unexpected (ValueFailure <Output> { expected, std::move (got) });
		return {};
	}
};

template <OutputT Output>
struct ValueTest <void, Output> {
	std::string_view name;
	Output expected;
	ValueTestFunction <void, Output> fn;

	[[nodiscard]]
	ValueTestResult <Output> run () const {
		Output got = fn();
		if (got != expected) [[unlikely]]
			return std::unexpected (ValueFailure <Output> { expected, std::move (got) });
		return {};
	}
};

template <typename Input, OutputT Output, ValueTestFunctionT <Input, Output> Fn>
ValueTest (std::string_view, Input, Output, Fn) -> ValueTest <Input, Output>;

template <OutputT Output, ValueTestFunctionT <void, Output> Fn>
ValueTest (std::string_view, Output, Fn) -> ValueTest <void, Output>;

template <typename T>
concept ValueTestT = Basedlib::specialization_of <T, ValueTest>;

/// @brief ValueCase is a special case of ValueTest. It can be used only within Suite, so
/// multiple Cases share the same test function and must have Input.
template <typename Input, OutputT Output>
struct ValueCase {
	std::string_view name;
	Input input;
	Output expected;

	using InputType = Input;
	using OutputType = Output;
	using TestType = ValueTest <Input, Output>;

	consteval auto make_test (ValueTestFunction <Input, Output> fn) const noexcept {
		return TestType {name, input, expected, fn};
	}
};

template <typename T>
concept ValueCaseT = Basedlib::specialization_of <T, ValueCase>;

template <auto Fn, typename... Ts>
concept ValueCaseFunctionT = (ValueCaseT<Ts> && ...) && ValueTestFunctionT <
	decltype (Fn),
	typename std::remove_cvref_t<Basedlib::first_variadic_t<Ts...>>::InputType,
	typename std::remove_cvref_t<Basedlib::first_variadic_t<Ts...>>::OutputType
> && Basedlib::all_same_remove_cvref<Ts...>;

}
