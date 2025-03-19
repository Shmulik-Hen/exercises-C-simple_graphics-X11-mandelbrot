#include <stdexcept>
#include <iostream>
#include <graphics_x11.h>
#include <runner_x11.h>

using namespace runner_ns_x11;
using namespace graphics_ns_base::graphics_ns_x11;

int main()
{
	try {
		graphics g;
		runner r(g);
		r.run();
	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
