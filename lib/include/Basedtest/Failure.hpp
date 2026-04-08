#pragma once

#include <expected>
#include <format>
#include <string>
#include <string_view>

namespace Basedtest {

struct Failure {
	std::string_view testName;
	std::string msg;
};

}

template<>
struct std::formatter <Basedtest::Failure> {
	constexpr auto parse (std::format_parse_context& ctx) const noexcept { return ctx.begin(); }
	auto format (const Basedtest::Failure& f, std::format_context& ctx) const noexcept {
		return std::format_to (ctx.out(), "Test case '{}' failed: {}", f.testName, f.msg);
	}
};
