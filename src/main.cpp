#include <common.h>
#include <graphics_x11.h>
#include <iostream>
#include <runner_x11.h>
#include <stdexcept>

using namespace runner_ns_x11;
using namespace graphics_ns_base::graphics_ns_x11;

int main() {
	try {
		runner r;
		r.run();
	} catch (const std::exception &e) {
		ERR("main: exception: ") << e.what();
		return -1;
	} catch (...) {
		ERR("main: unknown exception");
		throw std::runtime_error("runner: unknown exception");
	}

	return 0;
}
