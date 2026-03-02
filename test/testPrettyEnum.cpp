#include "Basedlib/Meta.hpp"
#include "Basedlib/PrettyEnum.hpp"

#include <print>

Bl_PrettyEnum (Color, Red, Green, Blue);

using PrettyColor = Basedlib::PrettyEnum<Color>;
using MetaColor = PrettyColor::Meta; // same as Basedlib::Meta<Color>;

/* Meta can be used for compile-time values only, but for many types (I suppose).
 * PrettyEnum can be used for dense enums created with Bl_PrettyEnum both in 
 * compile-time and runtime.
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

	std::print ("Type: {}, size: {}\n", MetaColor::t_name(), PrettyColor::size());
	std::print ("Runtime scoped/unscoped: {}, {}\n", PrettyColor::to_scoped_string(c), PrettyColor::to_string(c));
	std::print ("Static scoped/unscoped using PrettyColor: {}, {}\n", PrettyColor::to_scoped_string(Color::Red), PrettyColor::to_string(Color::Red));
	std::print ("Same but using MetaColor: {}, {}\n", MetaColor::v_name<Color::Green, true>(), MetaColor::v_name<Color::Green>());

	return 0;
}
