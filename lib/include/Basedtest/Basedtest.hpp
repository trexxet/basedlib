#pragma once

#include <array>
#include <concepts>
#include <format>
#include <print>
#include <string_view>
#include <vector>

#include "AssertTest.hpp"
#include "ValueTest.hpp"

namespace Basedtest {

template <typename T>
concept TestT = AssertTestT<T> || ValueTestT <T>;

/// @brief Fails is a vector of suite's failed tests. Empty if all suite's tests passed.
template <TestT Test>
struct Fails {
	using Failure = Test::Failure;
	std::vector <Failure> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

/// @brief Suite is a suite of multiple Tests/Cases of same type that
/// can be run all together or by name.
template <TestT Test, size_t N>
struct Suite {
	std::string_view name;
	std::array <Test, N> tests;
	constexpr std::size_t size() const noexcept { return tests.size(); }

	const Test* find (std::string_view testName) const {
		for (const Test& test : tests)
			if (test.name == testName)
				return &test;
		return nullptr;
	}

	using Result = Test::Result;
	using Failure = Test::Failure;

	template <bool doPrints = false>
	[[nodiscard]]
	Fails<Test> run () const {
		Fails<Test> fails;
		fails.items.reserve (size());
		for (const Test& test : tests) {
			Result result = test.run();
			if (!result)
				fails.items.emplace_back (std::move(result).error());
		}

		if constexpr (doPrints) {
			if (fails) {
				std::print ("Suite '{}': {} tests failed out of {}:\n", name, fails.size(), size());
				for (const Failure& fail : fails)
					std::print (" - {}\n", fail.to_string());
			} else {
				std::print ("Suite '{}': all {} tests passed\n", name, size());
			}
		}

		return fails;
	}

	consteval Suite (std::string_view name, std::array<Test, N> tests) : name (name), tests (std::move (tests)) { }
	Suite () = delete;
};

template <TestT Test, size_t N>
Suite (std::string_view name, std::array<Test, N> tests) -> Suite <Test, N>;

/// @brief Fill suite with Tests
template <typename Input, OutputT Output, ValueTestT... Ts>
consteval auto tests (Ts&&... args) {
	return std::array <ValueTest <Input, Output>, sizeof... (Ts)> { std::forward<Ts> (args)... };
}

/// @brief Fill suite with Cases running Fn
template <typename Input, OutputT Output, auto Fn, ValueCaseT... Ts>
requires ValueTestFunctionT <decltype (Fn), Input, Output>
consteval auto tests (Ts&&... args) {
	return std::array <ValueTest <Input, Output>, sizeof... (Ts)> {
		std::forward<Ts> (args).make_test (Fn)...
	};
}

}
