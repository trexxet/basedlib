#pragma once

#include <concepts>
#include <expected>
#include <string>
#include <string_view>

#include "Basedlib/Function.hpp"
#include "Basedlib/Specialization.hpp"

#include "Core.hpp"

namespace Basedtest {

struct AssertFailure : Failure {
	Failure bake () const noexcept { return *this; }
};

using AssertTestResult = std::expected <void, AssertFailure>;

Result bake_result (AssertTestResult result) {
	if (result) return {};
	return std::unexpected (std::move(result).error().bake());
}

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

template <typename T>
concept AssertTestT = Basedlib::specialization_of <T, AssertTest>;

}
