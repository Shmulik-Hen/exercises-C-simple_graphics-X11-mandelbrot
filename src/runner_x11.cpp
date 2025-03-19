#include <chrono>
#include <thread>
#include <iostream>
#include <runner_x11.h>

namespace runner_ns_x11 {

// using namespace graphics_ns_base;
// using namespace graphics_ns_base::graphics_ns_x11;

#define KEY_ESCAPE     9
#define KEY_SPACEBAR  65
#define KEY_UP       111
#define KEY_RIGHT    114
#define KEY_DOWN     116
#define KEY_LEFT     113

runner::runner(graphics& g) :
	_g{&g},
	_is_running{true}
{
	_xstep = _g->get_width()  / 10;
	_ystep = _g->get_height() / 10;
	_sz = {_xstep, _ystep};
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

void runner::draw() const
{
	if (_g->snapshot_exists()) {
		_g->show_snapshot();
	}
	else {
		_g->demo();
		_g->take_snapshot();
	}
	_g->draw_rect(_tl, _sz, graphics_base::bright_red, false);
};

void runner::run()
{
	XEvent event;
	XFlush((Display*)_g->get_display());

	while (_is_running) {
		if (get_event(event)) {
			_is_running = handle_event(event);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
};

} // namespace runner_ns_x11
