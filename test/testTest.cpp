#include "Basedtest/Basedtest.hpp"

#include <print>

int f (const int& x) {
	return x ? x + 1 : 0;
}

int main () {
	Basedtest::Suite <int, int> failSuite ("FailSuite");

	failSuite.add ({"1", 1, 2, f});
	failSuite.add ({"-1", -1, 0, f});
	failSuite.add ({"0", 0, 1, f});

	Basedtest::Fails fails = failSuite.run <true> ();

	return fails.rc();
}
