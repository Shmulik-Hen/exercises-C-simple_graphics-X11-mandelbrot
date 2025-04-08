#include <iostream>
#include <stdexcept>
#include <stdint.h>
#include <vector>
// #define DEBUG_GRFX
#include <common.h>
#include <runner_x11.h>

namespace runner_ns_x11 {

const int DEFAULT_WIDTH = 1024;
const int DEFAULT_HEIGHT = 640; // 16:10
const uint32_t DEFAULT_ITERS = 200;
const uint32_t MAX_ITERS = 400;
const uint32_t BIG_STEP = 50;
const uint32_t SMALL_STEP = 5;
const double DEF_ASPECT_RATIO = (double)DEFAULT_HEIGHT / (double)DEFAULT_WIDTH;
const double ITERS_FACTOR = 0.9;
const double DEFAULT_LEFT = -2.6;
const double DEFAULT_RIGHT = 1.2;
const double DEFAULT_TOP = 1.2;
const double DEFAULT_BOTTOM = -1.2;
const double DEFAULT_LIMIT = 2.0;
const double BOX_RATIO = 25.0;
const char *DEFAULT_NAME = "Mandelbrot set";

#define KEY_ESC 9
#define KEY_BACKSPACE 22
#define KEY_ENTER 36
#define KEY_SPACE 65
#define KEY_UP 111
#define KEY_RIGHT 114
#define KEY_DOWN 116
#define KEY_LEFT 113

void runner::init_values() {
	// use actual values - in case resolution
	// is bigger than the whole screen
	_actual_width = _g->get_width();
	_actual_height = _g->get_height();

	if (((double)_actual_height / (double)_actual_width) != _aspect_ratio) {
		WARN("runner: aspect ratio mismatch: "
		     << DBL(((double)_actual_height / (double)_actual_width), 6)
		     << " != " << DBL(_aspect_ratio, 6));
		_aspect_ratio = (double)_actual_height / (double)_actual_width;
	}

	_plane.resize(_actual_height);
	for (uint32_t i = 0; i < _actual_height; i++) {
		_plane[i].resize(_actual_width);
	}

	_md = {
	    .iterations = DEFAULT_ITERS,
	    .width = _actual_width,
	    .height = _actual_height,
	    .left = DEFAULT_LEFT,
	    .right = DEFAULT_RIGHT,
	    .top = DEFAULT_TOP,
	    .bottom = DEFAULT_BOTTOM,
	    .limit = DEFAULT_LIMIT,
	};

	_colors = new color_vec{
	    graphics_base::dark_purple,	  graphics_base::purple,
	    graphics_base::bright_purple, graphics_base::dark_blue,
	    graphics_base::blue,	  graphics_base::bright_blue,
	    graphics_base::dark_green,	  graphics_base::green,
	    graphics_base::bright_green,  graphics_base::dark_yellow,
	    graphics_base::yellow,	  graphics_base::bright_yellow,
	    graphics_base::dark_orange,	  graphics_base::orange,
	    graphics_base::bright_orange, graphics_base::dark_red,
	    graphics_base::red,		  graphics_base::bright_red,
	    graphics_base::black};

	_num_colors = _colors->size();
	_max_color = _num_colors - 1;
	_colors_step = _md.iterations / _num_colors;

	_initial_xstep = BIG_STEP;
	_initial_ystep = (uint32_t)((double)BIG_STEP * _aspect_ratio);
	_small_xstep = SMALL_STEP;
	_small_ystep = (uint32_t)((double)SMALL_STEP * _aspect_ratio);
	_xstep = _initial_xstep;
	_ystep = _initial_ystep;
	_sz.w = (uint32_t)((double)_actual_width / BOX_RATIO);
	_sz.h = (uint32_t)((double)_sz.w * _aspect_ratio);
	_tl = {300, 300};
	_br = {_tl.x + _sz.w, _tl.y + _sz.h};

	DBG("runner:" << ENDL << STR("  initial values:", 1) << ENDL
		      << STR("    ratio:", 17) << DBL(DEF_ASPECT_RATIO, 6)
		      << ENDL << STR("    width:", 17) << DEC(_initial_width, 4)
		      << ENDL << STR("    height:", 17)
		      << DEC(_initial_height, 4) << ENDL
		      << STR("  final values:", 1) << ENDL
		      << STR("    ratio:", 17) << DBL(_aspect_ratio, 6) << ENDL
		      << STR("    width:", 17) << DEC(_actual_width, 4) << ENDL
		      << STR("    height:", 17) << DEC(_actual_height, 4)
		      << ENDL << STR("    xstep:", 17) << DEC(_xstep, 4) << ENDL
		      << STR("    ystep:", 17) << DEC(_ystep, 4) << ENDL
		      << STR("    num colors:", 17) << DEC(_num_colors, 4)
		      << ENDL << STR("    max color:", 17) << DEC(_max_color, 4)
		      << ENDL << STR("    color step:", 17)
		      << DEC(_colors_step, 4) << ENDL);
};

runner::runner()
    : _aspect_ratio(DEF_ASPECT_RATIO), _initial_width(DEFAULT_WIDTH),
      _initial_height(DEFAULT_HEIGHT) {
	DBG("runner: creating graphics context");
	try {
		_g =
		    new graphics(_initial_width, _initial_height, DEFAULT_NAME);
	} catch (const std::exception &e) {
		ERR("runner: failed to create the graphic context" << e.what());
		throw;
	} catch (...) {
		ERR("runner: unknown exception");
		throw std::runtime_error("runner: unknown exception");
	}

	// this must be called after graphics initialization
	// but before the mandelbrot initialization.
	init_values();

	try {
		_m = new mandelbrot(_md);
	} catch (const std::exception &e) {
		ERR("runner: failed to create the mandelbrot set" << e.what());
		throw;
	} catch (...) {
		ERR("runner: unknown exception");
		throw std::runtime_error("runner: unknown exception");
	}

	try {
		_mdstk = new mand_stack();
	} catch (const std::exception &e) {
		ERR("runner: failed to create the mandelbrot stack"
		    << e.what());
		throw;
	} catch (...) {
		ERR("runner: unknown exception");
		throw std::runtime_error("runner: unknown exception");
	}

	_is_running = true;
};

runner::~runner() {
	if (_colors)
		delete _colors;

	if (_mdstk)
		delete _mdstk;

	if (_m)
		delete _m;

	if (_g)
		delete _g;
};

graphics_base::color_idx runner::convert_to_color(uint32_t v) const {
	uint32_t c = std::min((v / _colors_step), _max_color);
	DBG(STR("val: ", 1)
	    << DEC(v, 3) << STR(", step:", 1) << DEC(_colors_step, 3)
	    << STR(", max:", 1) << DEC(_max_color, 3)
	    << STR(", final color:", 1) << DEC(c, 2));
	return _colors->at(c);
};

void runner::create_set() { _m->compute(_plane); };

void runner::display_set() const {
	for (uint32_t y = 0; y < _plane.size(); y++) {
		for (uint32_t x = 0; x < _plane[y].size(); x++) {
			graphics_base::color_idx c =
			    convert_to_color(_plane[y][x]);
			DBG("pixel: p[" << DEC(y, 1) << "][" << DEC(x, 1)
					<< "] = " << _g->get_color_name(c));
			graphics_base::point pt = {x, y};
			_g->put_pixel(pt, c);
		}
	}
};

void runner::draw() {
	if (_g->snapshot_exists()) {
		_g->show_snapshot();
	} else {
#ifdef DEMO
		_g->demo();
		_g->take_snapshot();
#else
		// create the buffer
		_g->take_snapshot();
		create_set();
		// override the buffer data
		display_set();
		// show the buffer
		_g->show_snapshot();
#endif
	}

	if (_seq_num == 0) {
		uint32_t ofstx = (uint32_t)_m->get_x_center();
		uint32_t ofsty = (uint32_t)_m->get_y_center();

		graphics_base::point p, q;
		p = {ofstx, 0};
		q = {ofstx, _md.height};
		_g->draw_line(p, q, graphics_base::bright_red);

		p = {0, ofsty};
		q = {_md.width, ofsty};
		_g->draw_line(p, q, graphics_base::bright_red);
	}

	_g->draw_rect(_tl, _sz, graphics_base::white, false);
};

bool runner::get_event(XEvent &event) { return _g->wait_event(&event); };

bool runner::handle_event(XEvent &event) {
	bool ret = true, st1, st2;
	graphics_base::point tl, br;
	mandelbrot::mand_pos pos_tl, pos_br;
	s_entry s_ent;

	switch (event.type) {
	case Expose:
		DBG("Got expose event");
		if (event.xexpose.count == 0) {
			draw();
		}
		break;
	case KeyPress:
		DBG("Got key press event");
		DBG("keycode: " << DEC(event.xkey.keycode, 3));
		switch (event.xkey.keycode) {
		case KEY_SPACE:
			DBG("Got space key");
			_xstep = (_xstep == _initial_xstep) ? _small_xstep
							    : _initial_xstep;
			_ystep = (_ystep == _initial_ystep) ? _small_ystep
							    : _initial_ystep;
			DBG(STR("xstep:", 12)
			    << DEC(_xstep, 2) << SEP << STR("ystep:", 12)
			    << DEC(_ystep, 2) << ENDL);
			break;
		case KEY_LEFT:
			DBG("Got left arrow key");
			tl = {_tl.x - _xstep, _tl.y};
			br = {tl.x + _sz.w, tl.y + _sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_br = {br.x, br.y};
				_g->refresh();
			}
			break;
		case KEY_RIGHT:
			DBG("Got right arrow key");
			tl = {_tl.x + _xstep, _tl.y};
			br = {tl.x + _sz.w, tl.y + _sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_br = {br.x, br.y};
				_g->refresh();
			}
			break;
		case KEY_UP:
			DBG("Got up arrow key");
			tl = {_tl.x, _tl.y - _ystep};
			br = {tl.x + _sz.w, tl.y + _sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_br = {br.x, br.y};
				_g->refresh();
			}
			break;
		case KEY_DOWN:
			DBG("Got down arrow key");
			tl = {_tl.x, _tl.y + _ystep};
			br = {tl.x + _sz.w, tl.y + _sz.h};
			st1 = _g->is_in_bounds(tl) == graphics_base::BOUNDS_OK;
			st2 = _g->is_in_bounds(br) == graphics_base::BOUNDS_OK;
			if (st1 && st2) {
				_tl = {tl.x, tl.y};
				_br = {br.x, br.y};
				_g->refresh();
			}
			break;
		case KEY_ESC:
			DBG("Got escape key");
			ret = false;
			break;
		case KEY_ENTER:
			DBG("Got enter key");
			s_ent = {_seq_num++, _md};
			_mdstk->push(s_ent);
			pos_tl.ix = _tl.x;
			pos_tl.iy = _tl.y;
			pos_br.ix = _br.x;
			pos_br.iy = _br.y;
			_m->translate_position(pos_tl);
			_m->translate_position(pos_br);
			DBG("runner:"
			    << ENDL << STR("  pos_tl: ", 1) << DEC(pos_tl.ix, 3)
			    << SEP << DEC(pos_tl.iy, 3) << SEP
			    << DBL(pos_tl.dx, 6) << SEP << DBL(pos_tl.dy, 6)
			    << ENDL << STR("  pos_br: ", 1) << DEC(pos_br.ix, 3)
			    << SEP << DEC(pos_br.iy, 3) << SEP
			    << DBL(pos_br.dx, 6) << SEP << DBL(pos_br.dy, 6)
			    << ENDL);
			_md.iterations = std::min(
			    (uint32_t)((double)_md.iterations * ITERS_FACTOR),
			    MAX_ITERS);
			// _md.iterations = DEFAULT_ITERS;
			_md.width = _actual_width;
			_md.height = _actual_height;
			_md.left = pos_tl.dx;
			_md.right = pos_br.dx;
			_md.top = pos_tl.dy;
			_md.bottom = pos_br.dy;
			_md.limit = DEFAULT_LIMIT;
			_xstep = _initial_xstep;
			_ystep = _initial_ystep;

			try {
				if (_m) {
					delete _m;
					_m = NULL;
				}

				_m = new mandelbrot(_md);
			} catch (const std::exception &e) {
				ERR("runner: failed to create new mandelbrot "
				    "set"
				    << e.what());
				throw;
			} catch (...) {
				ERR("runner: unknown exception");
				throw std::runtime_error(
				    "runner: unknown exception");
			}

			_g->drop_snapshot();
			_g->refresh();
			break;
		case KEY_BACKSPACE:
			DBG("Got backspace key");
			if (_mdstk->empty())
				break;

			s_ent = _mdstk->top();
			_mdstk->pop();
			_seq_num = std::get<0>(s_ent);
			_md = std::get<1>(s_ent);
			_xstep = _initial_xstep;
			_ystep = _initial_ystep;

			try {
				if (_m) {
					delete _m;
					_m = NULL;
				}

				_m = new mandelbrot(_md);
			} catch (const std::exception &e) {
				ERR("runner: failed to create new mandelbrot "
				    "set"
				    << e.what());
				throw;
			} catch (...) {
				ERR("runner: unknown exception");
				throw std::runtime_error(
				    "runner: unknown exception");
			}

			_g->drop_snapshot();
			_g->refresh();
			break;
		default:
			DBG("Got unsupported key");
			DBG(STR("keycode:", 3) << DEC(event.xkey.keycode, 3));
			break;
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

void runner::run() {
	XEvent event;

	while (_is_running) {
		if (get_event(event)) {
			_is_running = handle_event(event);
		}
	}
};

} // namespace runner_ns_x11
