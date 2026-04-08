#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>

#include "Basedlib/Function.hpp"
#include "Basedlib/Traits.hpp"

#include "Failure.hpp"

namespace Basedtest {

template <typename T>
concept OutputT = std::equality_comparable <T>;

template <typename Input, OutputT Output>
using ValueTestFunction = Basedlib::FunctionRef <Output (const Input&)>;

template <typename Fn, typename Input, typename Output>
concept ValueTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, ValueTestFunction <Input, Output>>;

/// @brief ValueFailure stores the expected and got value for failed test
template <OutputT Output>
struct ValueFailure {
	Output expected, got;

	ValueFailure () = delete;
	ValueFailure (Output expected, Output got)
		: expected (std::move (expected)), got (std::move (got)) { }

	Failure bake (std::string_view testName) {
		auto to_str = [] (const Output& val) -> std::string {
			if constexpr (requires { val.to_string(); })
				return std::string (val.to_string());
			else if constexpr (std::is_arithmetic_v <Output>)
				return std::format ("{}", val);
			else return "<unprintable>";
		};
		return {
			.testName = testName,
			.msg = std::format ("expected '{}', got '{}'", to_str (expected), to_str (got))
		};
	}
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
			return std::unexpected (ValueFailure <Output> (expected, std::move (got)));
		return {};
	}
};

template <typename Input, OutputT Output, ValueTestFunctionT <Input, Output> Fn>
ValueTest (std::string_view, Input, Output, Fn) -> ValueTest <Input, Output>;

template <typename T>
concept ValueTestT = Basedlib::specialization_of <T, ValueTest>;

/// @brief ValueCase is a special case of ValueTest. It can be used only within Suite, so
/// multiple Cases share the same test function.
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
