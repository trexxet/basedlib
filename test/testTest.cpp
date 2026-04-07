#include "Basedtest/Basedtest.hpp"

#include <cassert>
#include <format>
#include <string>

struct Foo {
	int x, y;
	bool operator== (const Foo&) const = default;
	std::string to_string() const {
		return std::format ("{{x = {}, y = {}}}", x, y);
	}
};

Foo bar (const int& a) { return { a, -a }; }
int f (const int& x) { return x ? x + 1 : 0; }
int g (const int& x) { return x ? x - 1 : 0; }
bool b (const int& x) { return x > 0; }

BT_ASSERT_TEST (tester, Foo, d) {
	BT_ASSERT (d.x == -d.y);
	BT_SUCCESS;
}

int main () {
	Basedtest::Suite successSuite1 ("SuccessSuite1", Basedtest::cases <bar> (
		Basedtest::ValueCase {"1", 1, Foo {1, -1}},
		Basedtest::ValueCase {"2", 2, Foo {2, -2}},
		Basedtest::ValueCase {"1337", 1337, Foo {1337, -1337}}
	));
	Basedtest::Fails successSuiteFails1 = successSuite1.run <true> ();
	if (successSuiteFails1) return successSuiteFails1.rc();

	Basedtest::Suite failSuite1 ("FailSuite1", Basedtest::cases <b> (
		Basedtest::ValueCase {"1", 1, true},
		Basedtest::ValueCase {"-1", -1, true},
		Basedtest::ValueCase {"0", 0, false}
	));
	Basedtest::Fails failSuiteFails1 = failSuite1.run <true> ();
	assert (failSuiteFails1);

	Basedtest::Suite successSuite2 ("SuccessSuite2", Basedtest::tests (
		Basedtest::ValueTest {"1", 1, true, b},
		Basedtest::ValueTest {"2", 2, true, b},
		Basedtest::ValueTest {"-1337", -1337, false, b}
	));
	Basedtest::Fails successSuiteFails2 = successSuite2.run <true> ();
	if (successSuiteFails2) return successSuiteFails2.rc();

	Basedtest::Suite successSuite3 ("SuccessSuite3", Basedtest::cases <tester> (
		Basedtest::AssertCase {"1", Foo {1, -1}},
		Basedtest::AssertCase {"2", Foo {2, -2}}
	));
	Basedtest::Fails successSuiteFails3 = successSuite3.run <true> ();
	if (successSuiteFails3) return successSuiteFails3.rc();

	Basedtest::Suite failSuite2 ("FailSuite2", Basedtest::tests (
		Basedtest::AssertTest {"a", Foo {1, 1}, tester},
		Basedtest::ValueTest {"1", 1, 2, f},
		Basedtest::ValueTest {"-1", -1, 0, f},
		Basedtest::ValueTest {"0", 0, -1, g}
	));
	Basedtest::Fails failSuiteFails2 = failSuite2.run <true> ();
	assert (failSuiteFails2);

	return failSuiteFails2.rc();
}
