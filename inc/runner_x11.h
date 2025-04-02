#ifndef __RUNNER_X11_H__
#define __RUNNER_X11_H__

#include <vector>
#include <tuple>
#include <stack>
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
	typedef std::tuple<uint32_t, mandelbrot::mand_data> s_entry;
	typedef std::stack<s_entry> mand_stack;

	graphics* _g {NULL};
	mandelbrot* _m {NULL};
	color_vec* _colors {NULL};
	mand_stack* _mdstk {NULL};
	graphics_base::point _tl;
	graphics_base::point _br;
	graphics_base::size _sz;
	mandelbrot::mand_data _md;
	mandelbrot::plane_t _plane;
	double _aspect_ratio {0.0};
	uint32_t _initial_width {0};
	uint32_t _initial_height {0};
	uint32_t _actual_width {0};
	uint32_t _actual_height {0};
	uint32_t _initial_xstep {0};
	uint32_t _initial_ystep {0};
	uint32_t _small_xstep {0};
	uint32_t _small_ystep {0};
	uint32_t _xstep {0};
	uint32_t _ystep {0};
	uint32_t _num_colors {0};
	uint32_t _max_color{0};
	uint32_t _colors_step{0};
	uint32_t _seq_num {0};
	bool _is_running {false};

	void init_values();
	void create_set();
	void display_set() const;
	void draw();
	bool get_event(XEvent&);
	bool handle_event(XEvent&);
	graphics_base::color_idx convert_to_color(uint32_t) const;

public:
	runner();
	~runner();
	void run();
};

} // namespace runner_ns_x11

#endif // __RUNNER_X11_H__