#include "Basedtest/Basedtest.hpp"

#include <print>

int f (const int& x) {
	return x ? x + 1 : 0;
}

int main () {
	Basedtest::Suite failSuite ("FailSuite", Basedtest::tests <int, int> (
		Basedtest::Test {"1", 1, 2, f},
		Basedtest::Test {"-1", -1, 0, f},
		Basedtest::Test {"0", 0, 1, f}
	));
	Basedtest::Fails fails = failSuite.run <true> ();

	return fails.rc();
}
