#pragma once

#include <concepts>
#include <format>
#include <print>
#include <string_view>
#include <tuple>
#include <vector>

#include "Basedlib/Traits.hpp"

#include "Core.hpp"
#include "AssertTest.hpp"
#include "ValueTest.hpp"

namespace Basedtest {

template <typename T>
concept TestT = AssertTestT<T> || ValueTestT <T>;

/// @brief Fails is a vector of suite's failed tests. Empty if all suite's tests passed.
struct Fails {
	std::vector <Failure> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

/// @brief Suite is a suite of multiple Tests/Cases that can be run all together or by name.
template <TestT... Tests>
struct Suite {
	std::string_view name;
	std::tuple <Tests...> tests;
	constexpr std::size_t size() const noexcept { return std::tuple_size_v <decltype (tests)>; }

	template <bool doPrints = false>
	[[nodiscard]]
	Fails run () const {
		Fails fails;
		fails.items.reserve (size());
		std::apply ([&] (const auto&... test) {
			([&] {
				auto result = test.run();
				if (!result)
					fails.items.emplace_back (std::move(result).error().bake());
			} (), ...);
		}, tests);

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

	consteval Suite (std::string_view name, std::tuple<Tests...> tests)
		: name (name), tests (std::move (tests)) { }
	Suite () = delete;
};

template <TestT... Tests>
Suite (std::string_view name, std::tuple<Tests...> tests) -> Suite <Tests...>;

/// @brief Fill suite with Tests
template <ValueTestT... Ts> requires (sizeof...(Ts) > 0)
consteval auto tests (Ts&&... args) {
	return std::tuple <Ts...> { std::forward<Ts> (args)... };
}

/// @brief Fill suite with Cases running Fn
template <auto Fn, ValueCaseT... Ts> requires ValueCaseFunctionT <Fn, Ts...>
consteval auto cases (Ts&&... args) {
	return std::tuple <typename Ts::TestType...> { std::forward<Ts> (args).make_test (Fn)... };
}

}
