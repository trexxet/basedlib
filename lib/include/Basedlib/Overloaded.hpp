#pragma once

namespace Basedlib {

template<class... Ts>
struct Overloaded : Ts... {
	using Ts::operator()...;
};

}
