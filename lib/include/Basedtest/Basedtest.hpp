#pragma once

#include <array>
#include <concepts>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "Basedlib/Function.hpp"

namespace Basedtest {

template <typename T>
concept OutputT = std::equality_comparable <T>;

template <typename Input, OutputT Output>
using TestFunction = Basedlib::Function <Output (const Input&)>;

template <typename Input, typename Output, typename Fn>
concept TestFunctionT = std::convertible_to <Fn, TestFunction <Input, Output>>;

/// @brief Result stores the test result: pass flag and non-pass information
/// (message, expected & actual result)
template <OutputT Output>
struct Result {
	bool ok;
	std::string msg;

	struct Failed { Output expected, got; };
	std::optional <Failed> failed;

	explicit operator bool () const noexcept { return ok; }
	int rc () const noexcept { return static_cast <int> (!ok); }

	[[nodiscard]]
	static Result success () { return Result { .ok = true }; }
	[[nodiscard]]
	static Result fail (std::string msg) {
		return Result { .ok = false, .msg = std::move (msg) };
	}
	[[nodiscard]]
	static Result fail (std::string msg, Output expected, Output got) {
		return Result { .ok = false, .msg = std::move (msg),
			.failed = Failed { std::move (expected), std::move (got) }
		};
	}
};

/// @brief Fails is a vector of suite's failed tests. Empty if all suite's tests passed.
template <OutputT Output>
struct Fails {
	std::vector <Result <Output>> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

/// @brief Test is a self-sufficient test case, that can be used with or without the Suite.
template <typename Input, OutputT Output>
struct Test {
	std::string_view name;
	Input input;
	Output expected;
	TestFunction <Input, Output> fn;

	using ResultType = Result <Output>;

	[[nodiscard]]
	ResultType run () const {
		if (!fn) [[unlikely]]
			return ResultType::fail (std::format ("Function not specified for test case '{}'", name));
		Output got = fn (input);
		if (got == expected) [[likely]]
			return ResultType::success();

		auto to_str = [] (const Output& val) -> std::string {
			if constexpr (requires { val.to_string(); })
				return std::string (val.to_string());
			else if constexpr (std::is_arithmetic_v <Output>)
				return std::to_string (val);
			else return "<unprintable>";
		};

		return ResultType::fail (
			std::format ("Test case '{}' failed: expected '{}', got '{}'", name, to_str (expected), to_str (got)),
			expected, std::move (got)
		);
	}
};

template <typename Input, OutputT Output, typename Fn>
requires TestFunctionT <Input, Output, Fn>
Test (std::string_view, Input, Output, Fn) -> Test <Input, Output>;

/// @brief Case is a special case of Test. It can be used only within Suite, so
/// multiple Cases share the same test function.
template <typename Input, OutputT Output>
struct Case {
	std::string_view name;
	Input input;
	Output expected;

	consteval auto make_test (TestFunction <Input, Output> fn) const noexcept {
		return Test <Input, Output> {name, input, expected, fn};
	}
};

template <typename Input, OutputT Output>
Case (std::string_view, Input, Output) -> Case <Input, Output>;

/// @brief Suite is a suite of multiple Tests/Cases of same Input/Output type that
/// can be run all together or by name.
template <typename Input, OutputT Output, size_t N>
struct Suite {
	using TestType = Test <Input, Output>;
	using ResultType = TestType::ResultType;

	std::string_view name;
	std::array <TestType, N> tests;
	constexpr std::size_t size() const noexcept { return tests.size(); }

	[[nodiscard]]
	ResultType run (std::string_view testName) const {
		for (const TestType& test : tests)
			if (test.name == testName)
				return test.run();
		return ResultType::fail (std::format ("Test case '{}' not found in suite '{}'", testName, name));
	}

	template <bool doPrints = false>
	[[nodiscard]]
	Fails<Output> run () const {
		Fails<Output> fails;
		fails.items.reserve (size());
		for (const TestType& test : tests) {
			ResultType result = test.run();
			if (!result) fails.items.emplace_back (std::move (result));
		}

		if constexpr (doPrints) {
			if (fails) {
				std::print ("Suite '{}': {} tests failed out of {}:\n", name, fails.size(), size());
				for (const ResultType& result : fails)
					std::print (" - {}\n", result.msg);
			} else {
				std::print ("Suite '{}': all {} tests passed\n", name, size());
			}
		}

		return fails;
	}

	consteval Suite (std::string_view name, std::array<TestType, N> tests) : name (name), tests (std::move (tests)) { }
	Suite () = delete;
};

template <typename Input, OutputT Output, size_t N>
Suite (std::string_view name, std::array<Test <Input, Output>, N> tests) -> Suite <Input, Output, N>;

/// @brief Fill suite with Tests
template <typename Input, OutputT Output, typename... Ts>
requires (std::same_as <Ts, Test <Input, Output>> && ...)
consteval auto tests (Ts&&... args) {
	return std::array <Test <Input, Output>, sizeof... (Ts)> { std::forward<Ts> (args)... };
}

/// @brief Fill suite with Cases running Fn
template <typename Input, OutputT Output, auto Fn, typename... Ts>
requires (std::same_as <Ts, Case <Input, Output>> && ...) && TestFunctionT <Input, Output, decltype (Fn)>
consteval auto tests (Ts&&... args) {
	return std::array <Test <Input, Output>, sizeof... (Ts)> {
		std::forward<Ts> (args).make_test (Fn)...
	};
}

}
