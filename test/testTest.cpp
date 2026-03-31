#include "Basedtest/Basedtest.hpp"

#include <format>
#include <string>

struct Foo {
	int x, y;
	bool operator== (const Foo&) const = default;
	std::string to_string() const {
		return std::format ("{{x = {}, y = {}}}", x, y);
	}
};

Foo bar (const int& a) {
	return { a, -a };
}

int f (const int& x) {
	return x ? x + 1 : 0;
}

int g (const int& x) {
	return x ? x - 1 : 0;
}

int main () {
	Basedtest::Suite successSuite ("SuccessSuite", Basedtest::tests <int, Foo, bar> (
		Basedtest::Case {"1", 1, Foo {1, -1}},
		Basedtest::Case {"2", 2, Foo {2, -2}},
		Basedtest::Case {"1337", 1337, Foo {1337, -1337}}
	));

	Basedtest::Fails successSuiteFails = successSuite.run <true> ();
	if (successSuiteFails) return successSuiteFails.rc();

	Basedtest::Suite failSuite ("FailSuite", Basedtest::tests <int, int> (
		Basedtest::Test {"1", 1, 2, f},
		Basedtest::Test {"-1", -1, 0, f},
		Basedtest::Test {"0", 0, -1, g}
	));

	Basedtest::Fails failSuiteFails = failSuite.run <true> ();
	return failSuiteFails.rc();
}
