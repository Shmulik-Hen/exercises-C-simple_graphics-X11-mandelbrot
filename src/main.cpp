#include <stdexcept>
#include <iostream>
#include <common.h>
#include <graphics_x11.h>
#include <runner_x11.h>

using namespace runner_ns_x11;
using namespace graphics_ns_base::graphics_ns_x11;

int main()
{
	try {
		runner r;
		r.run();
	}
	catch (const std::exception& e) {
		ERR("Exception: ") << e.what() << ENDL;
		return -1;
	}

	return 0;
}
