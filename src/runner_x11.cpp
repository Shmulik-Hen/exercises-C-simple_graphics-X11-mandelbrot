#include <chrono>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <stdint.h>
// #define DEBUG_GRFX
#include <common.h>
#include <runner_x11.h>

namespace runner_ns_x11 {

const int DEFAULT_WIDTH  = 1024;
const int DEFAULT_HEIGHT = 768;
const int DEFAULT_ITERS = 400;
const char* DEFAULT_NAME = "Mandelbrot set";

#define KEY_ESCAPE     9
#define KEY_SPACEBAR  65
#define KEY_UP       111
#define KEY_RIGHT    114
#define KEY_DOWN     116
#define KEY_LEFT     113

void runner::init_values()
{
	_plane.resize(_g->get_height());
	for (uint32_t i = 0; i < _g->get_height(); i++) {
		_plane[i].resize(_g->get_width());
	}

	_md = {
		.iterations = DEFAULT_ITERS,
		.width = _g->get_width(),
		.height = _g->get_height(),
	};

	_colors = new color_vec {
		graphics_base::dark_purple,
		graphics_base::purple,
		graphics_base::bright_purple,
		graphics_base::dark_blue,
		graphics_base::blue,
		graphics_base::bright_blue,
		graphics_base::dark_green,
		graphics_base::green,
		graphics_base::bright_green,
		graphics_base::dark_yellow,
		graphics_base::yellow,
		graphics_base::bright_yellow,
		graphics_base::dark_orange,
		graphics_base::orange,
		graphics_base::bright_orange,
		graphics_base::dark_red,
		graphics_base::red,
		graphics_base::bright_red,
		graphics_base::black
	};

	_num_colors = _colors->size();
	_colors_step = _md.iterations / _num_colors;

	_xstep = _g->get_width() / 10;
	_ystep = _g->get_height() / 10;
	_sz = {_xstep, _ystep};
};

runner::runner() :
	_is_running{true}
{
	_g = new graphics(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_NAME);
	if (!_g) {
		throw std::runtime_error("failed to create the graphic context");
	}

	_m = new mandelbrot(_md);
	if (!_m) {
		throw std::runtime_error("failed to create the mandelbrot set");
	}

	init_values();
};

runner::~runner()
{
	if (_colors)
		delete _colors;

	if (_m)
		delete _m;

	if (_g)
		delete _g;
};

graphics_base::color_idx runner::convert_to_color(uint32_t v) const
{
	return _colors->at(v / _colors_step);
};

void runner::create_set()
{
	_m->compute(_plane);
};

void runner::display_set() const
{
	for (uint32_t y = 0; y < _plane.size(); y++) {
		for (uint32_t x = 0; x < _plane[y].size(); x++) {
			graphics_base::color_idx c = convert_to_color(_plane[y][x]);
			graphics_base::point pt = {x, y};
			_g->draw_pixel(pt, c);
			// rc = _g->put_pixel(pt, c);
		}
	}
};

void runner::draw()
{
#if 1
	if (_g->snapshot_exists()) {
		_g->show_snapshot();
	}
	else {
		create_set();
		display_set();
		// _g->take_snapshot();
	}
	_g->draw_rect(_tl, _sz, graphics_base::bright_red, false);
#else
	create_set();
	display_set();
#endif
};

bool runner::get_event(XEvent& event)
{
	if (XPending((Display*)_g->get_display())) {
		XNextEvent((Display*)_g->get_display(), &event);
		return true;
	}

	return false;
};

bool runner::handle_event(XEvent& event)
{
	bool ret = true, st1, st2;
	graphics_base::point tl, br;

	switch (event.type) {
	case Expose:
		DBG("Got expose event");
		if (event.xexpose.count == 0) {
			draw();
		}
		break;
	case KeyPress:
		DBG("Got key press event");

		switch (event.xkey.keycode) {
		case KEY_SPACEBAR:
			DBG("Got space key");
			_g->refresh();
			break;
		case KEY_LEFT:
			tl = {_tl.x-_xstep, _tl.y};
			br = {tl.x+_sz.w, tl.y+_sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_g->refresh();
			}
			break;
		case KEY_RIGHT:
			tl = {_tl.x+_xstep, _tl.y};
			br = {tl.x+_sz.w, tl.y+_sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_g->refresh();
			}
			break;
		case KEY_UP:
			tl = {_tl.x, _tl.y-_ystep};
			br = {tl.x+_sz.w, tl.y+_sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_g->refresh();
			}
			break;
		case KEY_DOWN:
			tl = {_tl.x, _tl.y+_ystep};
			br = {tl.x+_sz.w, tl.y+_sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_g->refresh();
			}
			break;
		case KEY_ESCAPE:
			DBG("Got escape key");
			ret = false;
		}
		break;
	case ButtonPress:
		DBG("Got button press event");
		break;
	default:
		DBG("Got unsupported event");
		ret = false;
		break;
	}

	return ret;
};

void runner::run()
{
	XEvent event;

	while (_is_running) {
		if (get_event(event)) {
			_is_running = handle_event(event);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
};

} // namespace runner_ns_x11
