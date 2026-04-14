#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>

#include "Basedlib/Core/Function.hpp"
#include "Basedlib/Meta/Traits.hpp"

#include "Common.hpp"

#define BT_ASSERT_TEST(fn_name, InputType, inputName) \
Basedtest::AssertTestResult fn_name (const InputType& inputName)

#define BT_SCENARIO_TEST(fn_name) Basedtest::AssertTestResult fn_name ()

#define BT_SUITE_SCENARIO(fn) Basedtest::ScenarioTest {#fn, fn}

#define BT_SUCCESS do { return {}; } while (0)

#define BT_FAIL(msg) do { \
	return std::unexpected (Basedtest::AssertFailure { msg }); \
} while (0)

#define BT_ASSERT(expr) do { \
	if (!(expr)) BT_FAIL (std::format ("assertion '{}' failed at line {}", #expr, __LINE__)); \
} while (0)

#define BT_ASSERT_EQ_CMP(lhs, rhs, op) do { \
	const auto& __bt_lhs = (lhs); \
	const auto& __bt_rhs = (rhs); \
	static_assert (std::equality_comparable_with <decltype(__bt_lhs), decltype(__bt_rhs)>, \
		"BT_ASSERT_EQ_CMP requires equality-comparable operands"); \
	if (!(__bt_lhs op __bt_rhs)) BT_FAIL (std::format ( \
		"assertion {} {} {} failed at line {}: lhs ({}), rhs ({})", #lhs, #op, #rhs, __LINE__, \
		Basedtest::format_value (__bt_lhs), Basedtest::format_value (__bt_rhs) \
	)); \
} while (0)

#define BT_ASSERT_EQ(lhs, rhs) BT_ASSERT_EQ_CMP (lhs, rhs, ==)
#define BT_ASSERT_NE(lhs, rhs) BT_ASSERT_EQ_CMP (lhs, rhs, !=)

#define BT_ASSERT_Z(expr) BT_ASSERT_EQ(expr, 0)
#define BT_ASSERT_RC(expr) BT_ASSERT_Z(expr)

namespace Basedtest {

struct AssertFailure {
	std::string msg;
};

using AssertTestResult = std::expected <void, AssertFailure>;

template <typename Input>
using AssertTestFunction = Basedlib::FunctionRef <AssertTestResult (const Input&)>;

template <typename Fn, typename Input>
concept AssertTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, AssertTestFunction <Input>>;

/// @brief AssertTest runs fn(const input&) until BT_SUCCESS or failed BT_ASSERT.
/// AssertTests can be used with or without the Suite.
template <typename Input>
struct AssertTest {
	std::string_view name;
	Input input;
	AssertTestFunction <Input> fn;

	[[nodiscard]]
	AssertTestResult run () const { return fn (input); }
};

template <typename Input, AssertTestFunctionT <Input> Fn>
AssertTest (std::string_view, Input, Fn) -> AssertTest <Input>;

template <typename T>
concept AssertTestT = Basedlib::specialization_of <T, AssertTest>;

using ScenarioTestFunction = Basedlib::FunctionRef <AssertTestResult()>;

template <typename Fn>
concept ScenarioTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, ScenarioTestFunction>;

/// @brief ScenarioTest is similar to AssertTest, but runs fn() without input.
/// ScenarioTests can be used with or without the Suite.
struct ScenarioTest {
	std::string_view name;
	ScenarioTestFunction fn;

	[[nodiscard]]
	AssertTestResult run () const { return fn(); }
};

template <typename T>
concept ScenarioTestT = std::same_as <std::remove_cvref_t<T>, ScenarioTest>;

/// @brief AssertCase is a special case of AssertTest. It can be used only within Suite, so
/// multiple Cases share the same test function.
template <typename Input>
struct AssertCase {
	std::string_view name;
	Input input;

	using InputType = Input;
	using TestType = AssertTest <Input>;

	consteval auto make_test (AssertTestFunction <Input> fn) const noexcept {
		return TestType {name, input, fn};
	}
};

template <typename T>
concept AssertCaseT = Basedlib::specialization_of <T, AssertCase>;

template <auto Fn, typename... Ts>
concept AssertCaseFunctionT = (AssertCaseT<Ts> && ...) && AssertTestFunctionT <
	decltype (Fn),
	typename std::remove_cvref_t<Basedlib::first_variadic_t<Ts...>>::InputType
> && Basedlib::all_same_remove_cvref<Ts...>;

}
