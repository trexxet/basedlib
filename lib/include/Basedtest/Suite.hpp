#pragma once

#include <concepts>
#include <format>
#include <print>
#include <string_view>
#include <tuple>
#include <vector>

#include "Basedlib/Traits.hpp"

#include "AssertTest.hpp"
#include "ValueTest.hpp"

#define BT_CHECK_RC(f) do { \
	int rc = static_cast<int> ((f)); \
	if (rc != 0) return rc; \
} while (0)

namespace Basedtest {

template <typename T>
concept TestT = AssertTestT<T> || ScenarioTestT<T> || ValueTestT<T>;
template <typename T>
concept CaseT = AssertCaseT<T> || ValueCaseT<T>;
template <auto Fn, typename... Ts>
concept CaseFunctionT = AssertCaseFunctionT<Fn, Ts...> || ValueCaseFunctionT<Fn, Ts...>;

struct SuiteFail {
	std::string_view testName;
	std::string msg;

	static SuiteFail make (std::string_view testName, const AssertFailure& f) {
		return { .testName = testName, .msg = f.msg };
	}

	template <Basedlib::specialization_of<ValueFailure> VF>
	static SuiteFail make (std::string_view testName, const VF& f) {
		return {
			.testName = testName,
			.msg = std::format ("expected '{}', got '{}'", format_value_output (f.expected), format_value_output (f.got))
		};
	}
};

/// @brief SuiteFails is a vector of suite's failed tests. Empty if all suite's tests passed.
struct SuiteFails {
	std::vector <SuiteFail> items;

	explicit operator bool () const noexcept { return !items.empty(); }
	int rc () const noexcept { return static_cast <int> (!items.empty()); }

	std::size_t size() const noexcept { return items.size(); }
	auto begin () noexcept { return items.begin(); }
	auto end ()   noexcept { return items.end(); }
	auto begin () const noexcept { return items.begin(); }
	auto end ()   const noexcept { return items.end(); }
};

/// @brief Suite is a suite of multiple Tests/Cases that can be run all together.
template <TestT... Tests>
struct Suite {
	std::string_view name;
	std::tuple <Tests...> tests;
	constexpr std::size_t size() const noexcept { return std::tuple_size_v <decltype (tests)>; }

	template <bool doPrints = true>
	[[nodiscard]]
	SuiteFails run () const {
		SuiteFails fails;
		fails.items.reserve (size());
		// TODO: use template for when available
		std::apply ([&] (const auto&... test) {
			([&] {
				auto result = test.run();
				if (!result)
					fails.items.emplace_back (SuiteFail::make (test.name, std::move(result).error()));
			} (), ...);
		}, tests);

		if constexpr (doPrints) {
			if (fails) {
				std::print ("Suite '{}': {} tests failed out of {}:\n", name, fails.size(), size());
				for (const SuiteFail& fail : fails)
					std::print (" - {}\n", fail);
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
template <TestT... Ts> requires (sizeof...(Ts) > 0)
consteval auto tests (Ts&&... args) {
	return std::tuple <Ts...> { std::forward<Ts> (args)... };
}

/// @brief Fill suite with Cases running Fn
template <auto Fn, CaseT... Ts> requires CaseFunctionT <Fn, Ts...>
consteval auto cases (Ts&&... args) {
	return std::tuple <typename Ts::TestType...> { std::forward<Ts> (args).make_test (Fn)... };
}

}

template<>
struct std::formatter <Basedtest::SuiteFail> {
	constexpr auto parse (std::format_parse_context& ctx) const noexcept { return ctx.begin(); }
	auto format (const Basedtest::SuiteFail& f, std::format_context& ctx) const noexcept {
		return std::format_to (ctx.out(), "Test case '{}' failed: {}", f.testName, f.msg);
	}
};
