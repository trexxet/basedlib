#include "Basedlib/Text/Multibyte.hpp"
#include "Basedtest/Suite.hpp"

#if !defined(__WIN32)
#include <clocale>
#endif
#include <string>

using namespace Basedtest;

BT_SCENARIO_TEST (test_mbs_to_wcs) {
	const std::string in = "Hello, World! Привет, мир! 你好, 世界! 𒁲 𒄳 🚀\n";
	const std::wstring expected = L"Hello, World! Привет, мир! 你好, 世界! 𒁲 𒄳 🚀\n";

	std::wstring got = Basedlib::Text::mbs_to_wcs (in);
	BT_ASSERT_EQ (got, expected);

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_wcs_to_mbs) {
	const std::wstring in = L"Hello, World! Привет, мир! 你好, 世界! 𒁲 𒄳 🚀\n";
	const std::string expected = "Hello, World! Привет, мир! 你好, 世界! 𒁲 𒄳 🚀\n";

	std::string got = Basedlib::Text::wcs_to_mbs (in);
	BT_ASSERT_EQ (got, expected);

	BT_SUCCESS;
}

int main () {
#if !defined(__WIN32)
	std::setlocale (LC_ALL, "C.utf8");
#endif

	return Suite ("Multibyte", tests (
		BT_SUITE_SCENARIO (test_mbs_to_wcs),
		BT_SUITE_SCENARIO (test_wcs_to_mbs)
	)).run_rc();
}
