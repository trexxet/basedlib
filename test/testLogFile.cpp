#include "Basedlib/LogFile.hpp"
#include "Basedtest/Suite.hpp"

#include <filesystem>
#include <fstream>
#include <system_error>

using namespace Basedtest;

void try_remove (const std::filesystem::path& path) noexcept {
	std::error_code ec;
	std::filesystem::remove (path, ec);
}

BT_SCENARIO_TEST(test_logfile) {
	const std::filesystem::path path = "basedlib_logfile_test.txt";
	const std::string str = "Hello, World! Привет, мир! 你好, 世界! 🚀\n";

	{
		Basedlib::LogFile lf (path);
		lf.print (str);
	}

	{
		std::ifstream in (path);
		BT_ASSERT (in);

		std::string written {
			std::istreambuf_iterator <char> (in),
			std::istreambuf_iterator <char> ()
		};
		BT_ASSERT_EQ (written, str);
	}

	try_remove (path);

	BT_SUCCESS;
}

int main () {
	return Suite ("LogFile", tests (
		BT_SUITE_SCENARIO (test_logfile)
	)).run_rc();
}
