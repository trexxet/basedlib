#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>

#include "Basedlib/Function.hpp"

namespace Basedtest {

struct AssertFailure {
	std::string_view testName;
	std::string where;

	AssertFailure () = delete;
	AssertFailure (std::string_view testName, std::string_view where)
		: testName (testName), where (where) { }
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

}
