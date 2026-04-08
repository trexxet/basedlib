#include "Basedtest/Suite.hpp"

#include <cassert>
#include <expected>
#include <format>
#include <string>

struct Point {
	int x, y;

	// To be used in value tests as output, the type must satisfy std::equality_comparable
	bool operator== (const Point&) const = default;

	// To be printable in a suite run, the type must satisfy std::formattable
	// or have std::string to_string() method
	std::string to_string() const {
		return std::format ("{{x = {}, y = {}}}", x, y);
	}
};

// 1) Multiple value test cases for the same function
Point mirror (const Point& a) { return { -a.x, -a.y }; }
int test_mirror () {
	// Note that suite initialization is consteval
	constexpr Point pointA {-1337, -1337};
	constexpr Point mirror_pointA {1337, 1337};
	Basedtest::Suite testMirror ("testMirror", Basedtest::cases <mirror> (
		// Cases can be constructed with designated initializers
		Basedtest::ValueCase {.name = "{1, 1}", .input = Point {1, 1}, .expected = Point {-1, -1}},
		Basedtest::ValueCase {"{2, -3}", Point {2, -3}, Point {-2, 3}},
		Basedtest::ValueCase {"{-1337, -1337}", pointA, mirror_pointA}
	));
	// run<true> means that the suite will print the result to the stdout
	Basedtest::SuiteFails testMirrorFails = testMirror.run<true>();
	// rc() returns 0 if SuiteFails is empty
	return testMirrorFails.rc();
}

// 2) Multiple assertion test cases for the same BT_ASSERT_TEST function
BT_ASSERT_TEST (tester_point_coord_sign, Point, d) {
	BT_ASSERT (d.x == -d.y);
	BT_SUCCESS;
}
Basedtest::SuiteFails test_point_coord_sign () {
	Basedtest::Suite testPointCoordSign ("testPointCoordSign", Basedtest::cases <tester_point_coord_sign> (
		Basedtest::AssertCase {.name = "{-1, 1}", .input = Point {-1, 1}},
		Basedtest::AssertCase {"{2, 3}", Point {2, 3}} // this one would fail!
	));
	// run<false> (default) is a silent run
	Basedtest::SuiteFails fails = testPointCoordSign.run();
	assert (fails); // SuiteFails is true when not empty
	return fails;
}

// 3) Mixed test suite for different functions and even test types
int inc (const int& x) { return x + 1; }
int test_mixed () {
	using namespace Basedtest;
	return Suite ("testMixed", tests (
		ValueTest {"inc1", 1, 2, inc}, // Tests can't be constructed with designated initializers as they rely on CTAD
		ValueTest {"dec2", 2, 1, [] (const int& x) { return x - 1; } }, // Lambdas can be used
		AssertTest {"point", Point {1, -1}, tester_point_coord_sign}
	)).run<true>().rc();
}

// 4) Tests can be run without suite. In that case ValueTest can return mismatched values.
int test_single () {
	using namespace Basedtest;
	const std::string name ("testSinglePoint");
	ValueTest test {
		name, Point {1, 2}, Point {21, 1},
		[] (const Point& p) { return Point {p.y, p.x}; } // would fail
	};
	ValueTestResult result = test.run();
	if (!result)
		std::print (
			"{} failed on purpose: expected '{}', got '{}'\n",
			name, result.error().expected.to_string(), result.error().got.to_string()
		);
	return 0;
}

int main () {
	// This macro would return the int passed if != 0
	BT_CHECK_RC (test_mirror());

	auto fails = test_point_coord_sign();
	for (const Basedtest::SuiteFail& fail : fails) {
		std::print ("Iterating over fails: '{}' failed on purpose with '{}'\n", fail.testName, fail.msg);
	}

	int rcMixed = test_mixed();
	BT_CHECK_RC (rcMixed);

	BT_CHECK_RC (test_single());

	return 0;
}
