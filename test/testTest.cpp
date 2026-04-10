#include "Basedtest/Suite.hpp"

#include <expected>
#include <format>
#include <string>

#include "Basedlib/Class.hpp"

using namespace Basedtest;

struct Point {
	int x, y;
	BASED_CLASS_DEFAULT_EQUALITY (Point);
	std::string to_string() const {
		return std::format ("{{x = {}, y = {}}}", x, y);
	}
};

Point mirror (const Point& a) { return { -a.x, -a.y }; }
int test_mirror () {
	constexpr Point pointA {-1337, -1337};
	constexpr Point mirror_pointA {1337, 1337};
	return Suite ("testMirror", cases <mirror> (
		ValueCase {.name = "{1, 1}", .input = Point {1, 1}, .expected = Point {-1, -1}},
		ValueCase {"{2, -3}", Point {2, -3}, Point {-2, 3}},
		ValueCase {"{-1337, -1337}", pointA, mirror_pointA}
	)).run<false>().rc();
}

int assert_line;
BT_ASSERT_TEST (tester_point_coord_sign, Point, d) {
	BT_ASSERT (d.x == -d.y); assert_line = __LINE__;
	BT_SUCCESS;
}
SuiteFails test_point_coord_sign () {
	return Suite ("testPointCoordSign", cases <tester_point_coord_sign> (
		AssertCase {.name = "{-1, 1}", .input = Point {-1, 1}},
		AssertCase {"{2, 3}", Point {2, 3}}
	)).run<false>();
}

BT_SCENARIO_TEST (tester_point_coord_compare) {
	Point p {-1, 2};
	if (p.y < p.x) BT_FAIL ("Wrong coordinates in p!");
	BT_ASSERT_NE (p.x, p.y);
	BT_SUCCESS;
}
int inc (const int& x) { return x + 1; }
int test_mixed () {
	return Suite ("testMixed", tests (
		ValueTest {"inc1", 1, 2, inc},
		ValueTest {"dec2", 2, 1, [] (const int& x) { return x - 1; } },
		AssertTest {"point1", Point {1, -1}, tester_point_coord_sign},
		ScenarioTest {"point2", tester_point_coord_compare},
		BT_SUITE_SCENARIO (tester_point_coord_compare)
	)).run<false>().rc();
}

ValueTestResult<Point> test_single () {
	return ValueTest {
		"testSinglePoint", Point {1, 2}, Point {21, 1},
		[] (const Point& p) { return Point {p.y, p.x}; }
	}.run();
}

int check_rc (int rc) {
	BT_CHECK_RC (rc);
	return 0;
}

BT_SCENARIO_TEST (test_check_rc) {
	BT_ASSERT_EQ (check_rc (0), 0);
	BT_ASSERT_EQ (check_rc (1), 1);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_value_case) {
	BT_ASSERT_EQ (test_mirror(), 0);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_assert_case) {
	auto fails = test_point_coord_sign();
	BT_ASSERT (fails);
	BT_ASSERT_EQ (fails.size(), 1);
	BT_ASSERT_EQ (fails.items[0].testName, "{2, 3}");
	BT_ASSERT_EQ (fails.items[0].msg, std::format ("assertion 'd.x == -d.y' failed at line {}", assert_line));
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_mixed_test_suite) {
	BT_ASSERT_EQ (test_mixed(), 0);
	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_single_value_test) {
	auto result = test_single();
	BT_ASSERT (!result);
	BT_ASSERT_EQ (result.error().expected, (Point {21, 1}));
	BT_ASSERT_EQ (result.error().got, (Point {2, 1}));
	BT_SUCCESS;
}

int main () {
	return Suite ("testTest", tests (
		BT_SUITE_SCENARIO (test_check_rc),
		BT_SUITE_SCENARIO (test_value_case),
		BT_SUITE_SCENARIO (test_assert_case),
		BT_SUITE_SCENARIO (test_mixed_test_suite),
		BT_SUITE_SCENARIO (test_single_value_test)
	)).run().rc();
}
