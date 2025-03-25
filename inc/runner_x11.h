#ifndef __RUNNER_X11_H__
#define __RUNNER_X11_H__

#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <graphics_x11.h>
#include <mandelbrot.h>

namespace runner_ns_x11 {

using namespace graphics_ns_base;
using namespace graphics_ns_base::graphics_ns_x11;
using namespace mandelbrot_ns;

class runner
{
private:
	typedef std::vector<graphics_base::color_idx> color_vec;

	graphics* _g {NULL};
	graphics_base::point _tl;
	graphics_base::size _sz;
	mandelbrot* _m {NULL};
	color_vec* _colors {NULL};
	mandelbrot::mand_data _md;
	bool _is_running{false};
	mandelbrot::plane_t _plane;
	uint32_t _xstep {0};
	uint32_t _ystep {0};
	int _num_colors {0};
	int _max_color{0};
	int _colors_step{0};

	void init_values();
	graphics_base::color_idx convert_to_color(uint32_t) const;
	void create_set();
	void display_set() const;
	void draw();
	bool get_event(XEvent&);
	bool handle_event(XEvent&);

public:
	runner();
	~runner();
	void run();
};

} // namespace runner_ns_x11

#endif // __RUNNER_X11_H__