#include "Basedlib/RingBuffer.hpp"

#include <cstdio>
#include <print>

using Rb = Basedlib::RingBuffer<int>;

void print_rb (const Rb& rb) {
	for (int x : rb)
		std::print ("{} ", x);
	std::putchar ('\n');
}

int main () {
	Rb rb (3);

	rb.push (1);
	rb.push (2);
	print_rb (rb);

	rb.push (3);
	rb.push (4);
	print_rb (rb);

	return 0;
}
