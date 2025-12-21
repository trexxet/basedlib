#include "Basedlib/FileDebug.hpp"

#include <cstdio>
#include <format>
#include <stdexcept>

namespace Basedlib {

FileDebug::FileDebug (const char *filename) {
	if (!filename) return;
	f = fopen (filename, "w");
	if (!f) [[unlikely]]
		throw std::runtime_error (std::format ("Can't open {} for write", filename));
}

void FileDebug::print (const std::string& str) {
	if (f) {
		std::fprintf (f, str.c_str());
		fflush(f);
	}
}

FileDebug::~FileDebug () {
	if (f) {
		fclose (f);
		f = nullptr;
	}
}

}
