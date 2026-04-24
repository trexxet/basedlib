#include "Basedlib/Text/Multibyte.hpp"
#include "Basedlib/Text/Widechar.hpp"

#include <cstdio>
#include <print>
#include <string>

#if defined(__WIN32)
# define RESET_STDOUT_ORIENTATION()
#else
# define RESET_STDOUT_ORIENTATION() { std::freopen (nullptr, "w", stdout); }
#endif

int main () {
#if !defined(__WIN32)
	std::setlocale (LC_ALL, "C.utf8");
#endif

	// UTF-8 string
	const std::string mbstr0 = "Hello, World! Привет, мир! 你好, 世界! 𒁲 𒄳 🚀\n";
	std::print ("{}", mbstr0);

	// Convert UTF-8 string to wchar string and print
	RESET_STDOUT_ORIENTATION();
	std::wstring wcstr = Basedlib::Text::mbs_to_wcs (mbstr0);
	Basedlib::Text::print_wcs (wcstr);

	// Convert wchar string back to UTF-8 string
	RESET_STDOUT_ORIENTATION();
	std::string mbstr1 = Basedlib::Text::wcs_to_mbs (wcstr);
	std::print ("{}", mbstr1);

	std::print ("End!\n");

	return 0;
}
