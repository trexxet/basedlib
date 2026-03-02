#include "Basedlib/PrettyEnum.hpp"

#include <print>

Bl_PrettyEnum (Color, Red, Green, Blue );

/* Meta can be used for compile-time values only, but for many types (I suppose).
 * PrettyEnum can be used for dense enums created with Bl_PrettyEnum both in 
 * compile-time and runtime.
 */

int main () {
	Color c;

	int ch = std::getchar();
	switch (ch) {
		case '2': c = Color::Green; break;
		case '3': c = Color::Blue; break;
		default: c = Color::Red; break;
	}

	std::print ("Runtime {}: {}\n",
		Basedlib::Meta<Color>::t_name(),
		Basedlib::PrettyEnum<Color>::to_string(c)
	);

	std::print ("Static and scoped: {}\n",
		Basedlib::PrettyEnum<Color, true>::to_string(Color::Red)
	);

	std::print ("Another static but unscoped: {}\n",
		Basedlib::Meta<Color>::template v_name<Color::Green>()
	);

	return 0;
}
