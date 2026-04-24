#include "Basedlib/Meta/PrettyEnum.hpp"

#include <cstdio>
#include <print>

// Works for both scoped and unscoped enums. Requires enum to be contignous (no explicit values)
enum class Color {
	Red,
	Green,
	Blue
};

// If enum has > 32 fields, the range should be increased in the PrettyEnum template arg
using PrettyColor = Basedlib::PrettyEnum<Color>;
using ReflectTypeColor = PrettyColor::ReflectType; // same as Basedlib::ReflectType<Color>;

/* ReflectType can be used for compile-time values only, but for many types (I suppose).
 * PrettyEnum can be used for dense enums both in compile-time and runtime.
 */

int main () {
	Color c;

	int input = std::getchar() - '0';
	if (PrettyColor::has_idx (input)) {
		c = PrettyColor::val (input);
	} else {
		std::print ("Wrong input index!\n");
		return 0;
	}

	std::print ("Type: {}, size: {}\n", ReflectTypeColor::t_name(), PrettyColor::size);
	std::print ("Runtime scoped/unscoped: {}, {}\n", PrettyColor::to_scoped_string(c), PrettyColor::to_string(c));
	std::print ("Static scoped/unscoped using PrettyColor: {}, {}\n", PrettyColor::to_scoped_string(Color::Red), PrettyColor::to_string(Color::Red));
	std::print ("Same but using ReflectTypeColor: {}, {}\n", ReflectTypeColor::v_name<Color::Green, true>(), ReflectTypeColor::v_name<Color::Green>());

	return 0;
}
