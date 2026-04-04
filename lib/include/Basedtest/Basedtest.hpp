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
#include "Basedlib/PrettyEnum.hpp"

namespace Basedtest {

enum class ResultError {
	OK,
	WRONG_VALUE,
	NO_FN
};
using PrettyResultError = Basedlib::PrettyEnum <ResultError>;

template <typename T>
concept OutputT = std::equality_comparable <T>;

template <typename Input, OutputT Output>
using ValueTestFunction = Basedlib::Function <Output (const Input&)>;

template <typename Fn, typename Input, typename Output>
concept ValueTestFunctionT = std::convertible_to <Fn, ValueTestFunction <Input, Output>>;

/// @brief Result stores the test result: pass flag and non-pass information
/// (message, expected & actual result)
template <OutputT Output>
struct Result {
	ResultError err;

	struct Failed { Output expected, got; };
	std::optional <Failed> failed;

	explicit operator bool () const noexcept { return err == ResultError::OK; }
	int rc () const noexcept { return PrettyResultError::idx (err); }

	[[nodiscard]]
	static Result success () { return Result { .err = ResultError::OK }; }
	[[nodiscard]]
	static Result fail (ResultError err) {
		return Result { .err = err };
	}
	[[nodiscard]]
	static Result fail (ResultError err, Output expected, Output got) {
		return Result {
			.err = err,
			.failed = Failed { std::move (expected), std::move (got) }
		};
	}

	std::string to_string() const {
		switch (err) {
			case ResultError::WRONG_VALUE: {
				auto to_str = [] (const Output& val) -> std::string {
					if constexpr (requires { val.to_string(); })
						return std::string (val.to_string());
					else if constexpr (std::is_arithmetic_v <Output>)
						return std::format ("{}", val);
					else return "<unprintable>";
				};
				return std::format ("FAILED: expected '{}', got '{}'", to_str (failed->expected), to_str (failed->got));
			}
			case ResultError::NO_FN: return "FAILED: function not specified";
		}
		return "OK";
	}
};

template <OutputT Output>
struct Failure {
	Result <Output> result;
	std::string_view testName;

	std::string to_string() const {
		return std::format ("Test case '{}': {}", testName, result.to_string());
	}
};

/// @brief Fails is a vector of suite's failed tests. Empty if all suite's tests passed.
template <OutputT Output>
struct Fails {
	std::vector <Failure <Output>> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

/// @brief ValueTest is a test that can be used with or without the Suite.
/// ValueTests runs fn(const input&) and compares it's result with expected.
template <typename Input, OutputT Output>
struct ValueTest {
	std::string_view name;
	Input input;
	Output expected;
	ValueTestFunction <Input, Output> fn;

	using ResultType = Result <Output>;

	[[nodiscard]]
	ResultType run () const {
		if (!fn) [[unlikely]]
			return ResultType::fail (ResultError::NO_FN);
		Output got = fn (input);
		if (got == expected) [[likely]]
			return ResultType::success();
		return ResultType::fail (ResultError::WRONG_VALUE, expected, std::move (got));
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

/// @brief Suite is a suite of multiple Tests/Cases of same Input/Output type that
/// can be run all together or by name.
template <typename Input, OutputT Output, size_t N>
struct Suite {
	using TestType = ValueTest <Input, Output>;
	using ResultType = TestType::ResultType;
	using FailureType = Failure <Output>;

	std::string_view name;
	std::array <TestType, N> tests;
	constexpr std::size_t size() const noexcept { return tests.size(); }

	[[nodiscard]]
	std::optional<ResultType> run (std::string_view testName) const {
		for (const TestType& test : tests)
			if (test.name == testName)
				return test.run();
		return std::nullopt;
	}

	template <bool doPrints = false>
	[[nodiscard]]
	Fails<Output> run () const {
		Fails<Output> fails;
		fails.items.reserve (size());
		for (const TestType& test : tests) {
			ResultType result = test.run();
			if (!result) fails.items.emplace_back (
				Failure { .result = std::move (result), .testName = test.name }
			);
		}

		if constexpr (doPrints) {
			if (fails) {
				std::print ("Suite '{}': {} tests failed out of {}:\n", name, fails.size(), size());
				for (const FailureType& fail : fails)
					std::print (" - {}\n", fail.to_string());
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
Suite (std::string_view name, std::array<ValueTest <Input, Output>, N> tests) -> Suite <Input, Output, N>;

/// @brief Fill suite with Tests
template <typename Input, OutputT Output, ValueTestT <Input, Output>... Ts>
consteval auto tests (Ts&&... args) {
	return std::array <ValueTest <Input, Output>, sizeof... (Ts)> { std::forward<Ts> (args)... };
}

/// @brief Fill suite with Cases running Fn
template <typename Input, OutputT Output, auto Fn, ValueCaseT <Input, Output>... Ts>
requires ValueTestFunctionT <decltype (Fn), Input, Output>
consteval auto tests (Ts&&... args) {
	return std::array <ValueTest <Input, Output>, sizeof... (Ts)> {
		std::forward<Ts> (args).make_test (Fn)...
	};
}

}
