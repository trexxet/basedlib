#pragma once

#include <concepts>
#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "Basedlib/Function.hpp"

namespace Basedtest {

class AssertTestResult {
	AssertTestResult (std::string_view testName) : testName (testName) { }
	AssertTestResult (std::string_view testName, std::string_view where)
		: testName (testName), where (where) { }
public:
	std::string_view testName;
	std::optional <std::string> where;

	explicit operator bool () const noexcept { return !where; }

	AssertTestResult () = delete;
	static AssertTestResult success (std::string_view testName) {
		return AssertTestResult (testName);
	}
	static AssertTestResult fail (std::string_view testName, std::string_view where) {
		return AssertTestResult (testName, where);
	}
};

template <typename Input>
using AssertTestFunction = Basedlib::FunctionRef <AssertTestResult (const Input&)>;

template <typename Fn, typename Input>
concept AssertTestFunctionT = std::convertible_to <std::remove_cvref_t <Fn>, AssertTestFunction <Input>>;

template <typename Input>
struct AssertTest {
	std::string_view name;
	Input input;
	AssertTestFunction <Input> fn;

	AssertTestResult run () const {
		return fn (input);
	}
};

}
