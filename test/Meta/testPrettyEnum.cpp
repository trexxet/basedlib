#include "Basedlib/Meta/PrettyEnum.hpp"
#include "Basedtest/Suite.hpp"

#include <string_view>

using namespace Basedtest;

enum class Color { Red, Green, Blue };

constexpr std::string_view to_string (Color c) {
	switch (c) {
		case Color::Red: return "Red";
		case Color::Green: return "Green";
		case Color::Blue: return "Blue";
	};
	return "";
}

constexpr std::string_view to_scoped_string (Color c) {
	switch (c) {
		case Color::Red: return "Color::Red";
		case Color::Green: return "Color::Green";
		case Color::Blue: return "Color::Blue";
	};
	return "";
}

using PrettyColor = Basedlib::PrettyEnum<Color>;
using ReflectTypeColor = PrettyColor::ReflectType;

template <Color c, bool scoped = false>
constexpr auto make_reflection_v_name_test (std::string_view name) {
	return ValueTest { name,
		[] { if constexpr (scoped) return to_scoped_string (c); return to_string (c); } (),
		[] { return ReflectTypeColor::v_name<c, scoped>(); }
	};
};

BT_SCENARIO_TEST (test_reflection) {
	BT_ASSERT_EQ (ReflectTypeColor::t_name(), "Color");

	BT_ASSERT_RC (Suite ("ReflectType::v_name", tests (
		make_reflection_v_name_test <Color::Red> ("reflecttype::v_name_r"),
		make_reflection_v_name_test <Color::Green> ("reflecttype::v_name_g"),
		make_reflection_v_name_test <Color::Blue> ("reflecttype::v_name_b")
	)).run_rc());

	BT_ASSERT_RC (Suite ("ReflectType::v_name_scoped", tests (
		make_reflection_v_name_test <Color::Red, true> ("reflecttype::v_name_scoped_r"),
		make_reflection_v_name_test <Color::Green, true> ("reflecttype::v_name_scoped_g"),
		make_reflection_v_name_test <Color::Blue, true> ("reflecttype::v_name_scoped_b")
	)).run_rc());

	BT_SUCCESS;
}

template <Color c, bool scoped = false>
constexpr auto make_prettycolor_name_case (std::string_view name) {
	return ValueCase { name, c,
		[] { if constexpr (scoped) return to_scoped_string (c); return to_string (c); } ()
	};
};

BT_SCENARIO_TEST (test_pretty_static) {
	BT_ASSERT_EQ (PrettyColor::size, 3);

	BT_ASSERT_RC (Suite ("Static PrettyColor::val", cases <input_as_constref<PrettyColor::val>> (
		ValueCase {"prettycolor::val_r", std::size_t{0}, Color::Red},
		ValueCase {"prettycolor::val_g", std::size_t{1}, Color::Green},
		ValueCase {"prettycolor::val_b", std::size_t{2}, Color::Blue}
	)).run_rc());

	BT_ASSERT_RC (Suite ("Static PrettyColor::idx", cases <input_as_constref<PrettyColor::idx>> (
		ValueCase {"prettycolor::idx_r", Color::Red, std::size_t{0}},
		ValueCase {"prettycolor::idx_g", Color::Green, std::size_t{1}},
		ValueCase {"prettycolor::idx_b", Color::Blue, std::size_t{2}}
	)).run_rc());

	BT_ASSERT_RC (Suite ("Static PrettyColor::to_string", cases <input_as_constref<PrettyColor::to_string>> (
		make_prettycolor_name_case <Color::Red> ("prettycolor::to_string_r"),
		make_prettycolor_name_case <Color::Green> ("prettycolor::to_string_g"),
		make_prettycolor_name_case <Color::Blue> ("prettycolor::to_string_b")
	)).run_rc());

	BT_ASSERT_RC (Suite ("Static PrettyColor::to_scoped_string", cases <input_as_constref<PrettyColor::to_scoped_string>> (
		make_prettycolor_name_case <Color::Red, true> ("prettycolor::to_scoped_string_r"),
		make_prettycolor_name_case <Color::Green, true> ("prettycolor::to_scoped_string_g"),
		make_prettycolor_name_case <Color::Blue, true> ("prettycolor::to_scoped_string_b")
	)).run_rc());

	BT_SUCCESS;
}

BT_SCENARIO_TEST (test_pretty_runtime) {
	volatile std::size_t i = Basedtest::black_box (2);
	volatile Color c = PrettyColor::val (i);
	BT_ASSERT_EQ (c, Color::Blue);
	BT_ASSERT_EQ (PrettyColor::idx (c), i);
	BT_ASSERT_EQ (PrettyColor::to_scoped_string(c), "Color::Blue");
	BT_ASSERT_EQ (PrettyColor::to_string(c), "Blue");
	BT_SUCCESS;
}

int main () {
	return Basedtest::Suite ("PrettyEnum", Basedtest::tests (
		BT_SUITE_SCENARIO (test_reflection),
		BT_SUITE_SCENARIO (test_pretty_static),
		BT_SUITE_SCENARIO (test_pretty_runtime)
	)).run_rc();
}
