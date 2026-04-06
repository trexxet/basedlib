#pragma once

#include <expected>
#include <format>
#include <string>
#include <string_view>

namespace Basedtest {

struct Failure {
	std::string_view testName;
	std::string msg;

	std::string to_string () const {
		return std::format ("Test case '{}' failed: {}", testName, msg);
	}
};

using Result = std::expected <void, Failure>;

template <typename T>
Result bake_result (T) = delete;

}
