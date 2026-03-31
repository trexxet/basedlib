#pragma once

#include <array>
#include <concepts>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "Basedlib/Function.hpp"

namespace Basedtest {

struct Result {
	bool ok;
	std::string msg;

	explicit operator bool () const noexcept { return ok; }
	int rc () const noexcept { return static_cast <int> (!ok); }

	[[nodiscard]]
	static Result success () { return Result { .ok = true }; }
	[[nodiscard]]
	static Result fail (std::string msg) {
		return Result { .ok = false, .msg = std::move (msg) };
	}
};

struct Fails {
	std::vector <Result> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

template <typename Input, std::equality_comparable Output>
struct Test {
	std::string_view name;
	Input input;
	Output expected;
	Basedlib::Function <Output (const Input&)> fn;

	[[nodiscard]]
	Result run () const {
		if (!fn) [[unlikely]]
			return Result::fail (std::format ("Function not specified for test case '{}'", name));
		if (fn (input) == expected) [[likely]]
			return Result::success();
		return Result::fail (std::format ("Test case '{}' failed", name));
	}
};

template <typename Input, std::equality_comparable Output, typename Fn>
requires std::is_convertible_v<Fn, Basedlib::Function <Output (const Input&)>>
Test (std::string_view, Input, Output, Fn) -> Test <Input, Output>;

template <typename Input, std::equality_comparable Output, size_t N>
struct Suite {
	using TestType = Test <Input, Output>;

	std::string_view name;
	std::array <TestType, N> tests;
	std::size_t size() const noexcept { return tests.size(); }

	[[nodiscard]]
	Result run (std::string_view testName) const {
		for (const TestType& test : tests)
			if (test.name == testName)
				return test.run();
		return Result::fail (std::format ("Test case '{}' not found in suite '{}'", testName, name));
	}

	template <bool doPrints = false>
	[[nodiscard]]
	Fails run () const {
		Fails fails;
		fails.items.reserve (size());
		for (const TestType& test : tests) {
			Result result = test.run();
			if (!result) fails.items.emplace_back (std::move (result));
		}

		if constexpr (doPrints) {
			if (fails) {
				std::print ("Suite '{}': {} tests failed out of {}:\n", name, fails.size(), size());
				for (const Result& result : fails)
					std::print (" - {}\n", result.msg);
			} else {
				std::print ("Suite '{}': all {} tests passed\n", name, size());
			}
		}

		return fails;
	}

	Suite (std::string_view name, std::array<TestType, N> tests) : name (name), tests (std::move (tests)) { }
	Suite () = delete;
};

template <typename Input, std::equality_comparable Output, size_t N>
Suite (std::string_view name, std::array<Test <Input, Output>, N> tests) -> Suite <Input, Output, N>;

template <typename Input, std::equality_comparable Output, typename... Ts>
consteval auto tests (Ts&&... args) -> std::array <Test <Input, Output>, sizeof... (Ts)> {
	return std::array <Test <Input, Output>, sizeof... (Ts)> { std::forward<Ts> (args)... };
}

}
