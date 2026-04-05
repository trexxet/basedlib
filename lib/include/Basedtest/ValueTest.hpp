#pragma once

#include <concepts>
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace Basedtest {

template <typename T>
concept OutputT = std::equality_comparable <T>;

template <typename Input, OutputT Output>
using ValueTestFunction = Basedlib::FunctionRef <Output (const Input&)>;

template <typename Fn, typename Input, typename Output>
concept ValueTestFunctionT = std::convertible_to <Fn, ValueTestFunction <Input, Output>>;

/// @brief ValueFailure stores the expected and got value for failed test
template <OutputT Output>
struct ValueFailure {
	std::string_view testName;
	Output expected, got;

	ValueFailure () = delete;
	ValueFailure (std::string_view testName, Output expected, Output got)
		: testName (testName), expected (std::move (expected)), got (std::move (got)) { }

	std::string to_string () const {
		auto to_str = [] (const Output& val) -> std::string {
			if constexpr (requires { val.to_string(); })
				return std::string (val.to_string());
			else if constexpr (std::is_arithmetic_v <Output>)
				return std::format ("{}", val);
			else return "<unprintable>";
		};

		return std::format ("Test case '{}' failed: expected '{}', got '{}'",
			testName, to_str (expected), to_str (got));
	}
};

template <OutputT Output>
using ValueTestResult = std::optional <ValueFailure <Output>>;

/// @brief ValueTest runs fn(const input&) and compares it's result with the expected one.
/// ValueTests can be used with or without the Suite.
template <typename Input, OutputT Output>
struct ValueTest {
	std::string_view name;
	Input input;
	Output expected;
	ValueTestFunction <Input, Output> fn;

	using Failure = ValueFailure <Output>;

	[[nodiscard]]
	ValueTestResult <Output> run () const {
		Output got = fn (input);
		if (got == expected) [[likely]] return std::nullopt;
		return Failure (name, expected, std::move (got));
	}
};

template <typename Input, OutputT Output, ValueTestFunctionT <Input, Output> Fn>
ValueTest (std::string_view, Input, Output, Fn) -> ValueTest <Input, Output>;

template <typename T, typename Input, typename Output>
concept ValueTestT = std::same_as <T, ValueTest<Input, Output>>;

/// @brief ValueCase is a special case of ValueTest. It can be used only within Suite, so
/// multiple Cases share the same test function.
template <typename Input, OutputT Output>
struct ValueCase {
	std::string_view name;
	Input input;
	Output expected;

	consteval auto make_test (ValueTestFunction <Input, Output> fn) const noexcept {
		return ValueTest <Input, Output> {name, input, expected, fn};
	}
};

template <typename Input, OutputT Output>
ValueCase (std::string_view, Input, Output) -> ValueCase <Input, Output>;

template <typename T, typename Input, typename Output>
concept ValueCaseT = std::same_as <T, ValueCase<Input, Output>>;

}
