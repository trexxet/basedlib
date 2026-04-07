#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>

#include "Basedlib/Function.hpp"
#include "Basedlib/Traits.hpp"

#include "Core.hpp"

#define BT_ASSERT_TEST(fn_name, InputType, inputName) \
Basedtest::AssertTestResult fn_name (const InputType& inputName)

#define BT_SUCCESS do { return {}; } while (0)

#define BT_FAIL(msg) do { \
	return std::unexpected (Basedtest::AssertFailure { "placeholder", msg }); \
} while (0)

#define BT_ASSERT(expr) do { \
	if (!(expr)) BT_FAIL(std::format ("assertion '{}' failed", #expr)); \
} while (0)

namespace Basedtest {

struct AssertFailure : Failure {
	Failure bake () const noexcept { return *this; }
};

using AssertTestResult = std::expected <void, AssertFailure>;

template <typename Input>
using AssertTestFunction = Basedlib::FunctionRef <AssertTestResult (const Input&)>;

template <typename Fn, typename Input>
concept AssertTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, AssertTestFunction <Input>>;

template <typename Input>
struct AssertTest {
	std::string_view name;
	Input input;
	AssertTestFunction <Input> fn;

	AssertTestResult run () const { return fn (input); }
};

template <typename Input, AssertTestFunctionT <Input> Fn>
AssertTest (std::string_view, Input, Fn) -> AssertTest <Input>;

template <typename T>
concept AssertTestT = Basedlib::specialization_of <T, AssertTest>;

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
