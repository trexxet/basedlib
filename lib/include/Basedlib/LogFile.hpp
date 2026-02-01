#pragma once

#include <format>
#include <fstream>
#include <print>
#include <stdexcept>
#include <string_view>

#include "Class.hpp"

namespace Basedlib {

class LogFile {
	std::ofstream f;

public:
	LogFile (const char *filename) {
		if (!filename) return;
		f.open (filename);
		if (!f) [[unlikely]]
			throw std::runtime_error (std::format ("Can't open {} for write", filename));
	}

	void print (std::string_view str) {
		if (!f || str.empty()) [[unlikely]] return;
		std::print (f, "{}", str);
		f.flush();
	}
	void print (const char* str) { print (std::string_view (str)); }
	void print (const std::string& str) { print (std::string_view (str)); }

	BASED_CLASS_NO_COPY_DEFAULT_MOVE (LogFile);
};

}
