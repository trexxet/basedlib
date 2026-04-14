#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <stdexcept>
#include <string_view>

#include "Basedlib/Class.hpp"

namespace Basedlib {

class LogFile {
	std::ofstream f;

public:
	explicit LogFile (const std::filesystem::path& path) {
		f.open (path);
		// TODO: C++26 can format filesystem::path
		if (!f) [[unlikely]]
			throw std::runtime_error (std::format ("Can't open {} for write", path.string()));
	}
	LogFile () = delete;

	void print (std::string_view str) {
		if (!f || str.empty()) [[unlikely]] return;
		std::print (f, "{}", str);
		f.flush();
	}

	template <typename... Args>
	void print (std::format_string<Args...> fmt, Args&&... args) {
		if (!f) [[unlikely]] return;
		std::print (f, fmt, std::forward<Args>(args)...);
		f.flush();
	}

	BASED_CLASS_NO_COPY_DEFAULT_MOVE (LogFile);
};

}
