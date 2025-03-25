#include <iostream>
#include <stdexcept>
#include <vector>
#include <stdint.h>
// #define DEBUG_GRFX
#include <common.h>
#include <runner_x11.h>

namespace runner_ns_x11 {

const int DEFAULT_WIDTH  = 1024;
const int DEFAULT_HEIGHT = DEFAULT_WIDTH * 9 / 16;
const int DEFAULT_ITERS  = 30;
const char* DEFAULT_NAME = "Mandelbrot set";

#define KEY_ESC		9
#define KEY_SPACE	65
#define KEY_UP		111
#define KEY_RIGHT	114
#define KEY_DOWN	116
#define KEY_LEFT	113

void runner::init_values()
{
	uint32_t wdt = _g->get_width();
	uint32_t hgt = _g->get_height();

	_plane.resize(hgt);
	for (uint32_t i = 0; i < hgt; i++) {
		_plane[i].resize(wdt);
	}

	_md = {
		.iterations = DEFAULT_ITERS,
		.width = wdt,
		.height = hgt,
		.left = -2.6,
		.right = 1.2,
		.top = 1.2,
		.bottom = -1.2,
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
	_max_color = _num_colors - 1;
	_colors_step = _md.iterations / _num_colors;

	_xstep = _g->get_width() / 10;
	_ystep = _g->get_height() / 10;
	_sz = {_xstep, _ystep};
	double aspect_ratio = ((double(hgt) / (double)wdt));

	INFO(ENDL
		<< STR("width", 13) << DEC(wdt, 4) << ENDL
		<< STR("height", 13) << DEC(hgt, 4) << ENDL
		<< STR("aspect ratio", 13) << FLT(aspect_ratio, 3) << ENDL
		<< STR("num colors", 13) << DEC(_num_colors, 2) << ENDL
		<< STR("max color", 13) << DEC(_max_color, 2) << ENDL
		<< STR("color step", 13) << DEC(_max_color, 2) << ENDL
		<< STR("xstep", 13) << DEC(_xstep, 2) << ENDL
		<< STR("ystep", 13) << DEC(_ystep, 2) << ENDL);
};

runner::runner() :
	_is_running{true}
{
	_g = new graphics(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_NAME);
	if (!_g) {
		throw std::runtime_error("failed to create the graphic context");
	}

	// this must be called after graphics initialization
	// but before the mandelbrot initialization.
	init_values();

	_m = new mandelbrot(_md);
	if (!_m) {
		throw std::runtime_error("failed to create the mandelbrot set");
	}
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
	uint32_t c = std::min((v / _colors_step), _max_color);
	DBG(STR("val:", 3) << DEC(v, 3) << STR(", step:", 2) << DEC(_colors_step, 3)
		<< STR(", max:", 2) << DEC(_max_color, 3)
		<< STR(", final color:", 2) << DEC(c, 2));
	return _colors->at(c);
};

void runner::create_set()
{
	DBG("compute started");
	_m->compute(_plane);
	DBG("compute finished");
};

void runner::display_set() const
{
	DBG("display started");
	for (uint32_t y = 0; y < _plane.size(); y++) {
		for (uint32_t x = 0; x < _plane[y].size(); x++) {
			graphics_base::color_idx c = convert_to_color(_plane[y][x]);
			DBG("pixel: p[" << DEC(y, 1) << "][" << DEC(x, 1)
				<< "] = " << _g->get_color_name(c));
			graphics_base::point pt = {x, y};
			_g->put_pixel(pt, c);
		}
	}
	DBG("display finished");
};

void runner::draw()
{
	if (_g->snapshot_exists()) {
		_g->show_snapshot();
	}
	else {
#if 1
		create_set();
		_g->take_snapshot();
		display_set();
		_g->show_snapshot();
#else
		_g->demo();
		_g->take_snapshot();
#endif
	}
	_g->flush();

	uint32_t ofstx = (uint32_t)_m->get_x_center();
	uint32_t ofsty = (uint32_t)_m->get_y_center();

	graphics_base::point p, q, r, s;
	p = {ofstx, 0};
	q = {ofstx, _md.height};
	r = {0, ofsty};
	s = {_md.width, ofsty};
	_g->draw_line(p, q, graphics_base::bright_red);
	_g->draw_line(r, s, graphics_base::bright_red);
	_g->draw_rect(_tl, _sz, graphics_base::bright_red, false);
	_g->flush();
};

bool runner::get_event(XEvent& event)
{
	return _g->wait_event(&event);
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
		case KEY_SPACE:
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
		case KEY_ESC:
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
	}
};

} // namespace runner_ns_x11
