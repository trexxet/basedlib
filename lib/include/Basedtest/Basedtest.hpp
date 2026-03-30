#pragma once

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
	std::string name;
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

template <typename Input, std::equality_comparable Output>
struct Suite {
	using TestType = Test <Input, Output>;

	std::string name;
	std::vector <TestType> tests;

	void add (TestType test) {
		tests.emplace_back (std::move (test));
	}

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

	Suite (std::string_view name) : name (name) { }
	Suite () = delete;
};

}
