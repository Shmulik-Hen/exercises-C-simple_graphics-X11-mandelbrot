#ifndef __RUNNER_X11_H__
#define __RUNNER_X11_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <graphics_x11.h>

namespace runner_ns_x11 {

using namespace graphics_ns_base;
using namespace graphics_ns_base::graphics_ns_x11;

class runner
{
private:
	graphics* _g {NULL};
	bool _is_running {false};
	graphics_base::point _tl;
	graphics_base::size _sz;
	int _xstep {0};
	int _ystep {0};

	bool get_event(XEvent&);
	bool handle_event(XEvent&);
	void draw() const;

public:

	runner(graphics&);
	void run();
};

} // namespace runner_ns_x11

#endif // __RUNNER_X11_H__