#include <stdexcept>
#include <utility>
#include <string.h>
#include <graphics_x11.h>

namespace graphics_ns_base {

namespace graphics_ns_x11 {

const int DEFAULT_WIDTH  = 800;
const int DEFAULT_HEIGHT = 600;
const char* DEFAULT_NAME = "Graphics_X11";

#define GMASK 0x00FFFFFF
#define CMASK 0xFF
#define R(x) (((x) & CMASK) << 16)
#define G(x) (((x) & CMASK) << 8)
#define B(x) ((x) & CMASK)
#define RGB(r, g, b) ((R(r) + G(g) + B(b)) & GMASK)

#ifdef DEBUG_GRFX
const std::string PIXEL_PFX	= "pixel: ";
const std::string LINE_PFX	= "line: ";
const std::string RECT_PFX	= "rect: ";
const std::string TEXT_PFX	= "text: ";
#endif //DEBUG_GRFX

void graphics::init_colors()
{
	const char *tmp_col[get_num_colors()] =  {
		"black",
		"white",
		"light grey",
		"dim grey",
		"red1",
		"red4",
		"red4",
		"orange1",
		"orange3",
		"orange4",
		"yellow1",
		"yellow4",
		"yellow4",
		"green1",
		"green4",
		"green4",
		"blue1",
		"blue4",
		"blue4",
		"purple1",
		"purple3",
		"purple4"
	};

	_colors = new color;
	_colors->clear();

	int errors = 0;
	for (int i = __first_color__; i < __last_color__; i++) {
		color_idx idx = (color_idx)i;
		const char *s = tmp_col[i];

		if (!strlen((s ? s : ""))) {
			WARN("empty string");
			errors++;
			continue;
		}

		XColor xc, def;
		int rc = XLookupColor(_display, _cmap, s, &xc, &def);
		if (rc != 1) {
			WARN("color not found");
			errors++;
			continue;
		}

		// fine tuning
		switch (idx) {
		case dark_red:
			xc.red -= 66;
			break;
		case dark_yellow:
			xc.red -= 66;
			xc.green -= 66;
			break;
		case dark_green:
			xc.green -= 66;
			break;
		case dark_blue:
			xc.blue -= 66;
			break;
		default:
			break;
		}

		color_data d;
		d.name = std::string{s};
		d.rgb = {xc.red, xc.green, xc.blue};
		d.val = RGB(d.rgb.r, d.rgb.g, d.rgb.b);
		d.bright = is_bright_color(idx);
		d.done = true;

		std::pair p = {idx, d};
		_colors->insert(p);
	}

	if (errors || _colors->empty()) {
		throw std::runtime_error("there were color initialization errors");
	}

#ifdef DEBUG_GRFX
	std::cout << _colors->size() << std::endl;
	for (color::iterator it = _colors->begin(); it != _colors->end(); it++) {
	  color_idx c = it->first;
	  color_data d = it->second;
	  std::string s = d.name + ',';

	  std::cout << "-----" << std::endl
			<< DEC(c, 2) << std::endl
			<< STR(s, 15) << HEX(d.val, 8) << std::endl
			<< HEX(d.rgb.r, 2) << SEP
			<< HEX(d.rgb.g, 2) << SEP
			<< HEX(d.rgb.g, 2) << SEP << std::endl
			<< d.bright << SEP << d.done << std::endl;
	}

	std::cout << "-----" << std::endl;
#endif
};

void graphics::init_graphics()
{
	int rc;

	_display = XOpenDisplay(NULL);
	if (!_display) {
		throw std::runtime_error("Unable to open the display");
	}

	_screen = DefaultScreen(_display);
	_visual = DefaultVisualOfScreen(ScreenOfDisplay(_display, _screen));
	_root = RootWindow(_display, _screen);
	_cmap = DefaultColormap(_display, _screen);

	try {
		init_colors();
	}
	catch (std::runtime_error& e) {
		throw;
	}

	_window = XCreateSimpleWindow(_display, _root, 0, 0, _width, _height, 1,
								  get_color_val(_fg), get_color_val(_bg));
	if (!_window) {
		throw std::runtime_error("Unable to create window");
	}

	_gc = XCreateGC(_display, _window, 0, 0);
	if (!_gc) {
		throw std::runtime_error("Unable to create graphic context");
	}

	rc = XSetStandardProperties(_display, _window, _name, NULL, None, NULL, 0, NULL);
	if (!rc) {
		throw std::runtime_error("Unable to set properties");
	}

	rc = XSelectInput(_display, _window, KeyPressMask | ButtonPressMask | ExposureMask);
	if (!rc) {
		throw std::runtime_error("Unable to select input");
	}

	rc = XSetBackground(_display, _gc, _bg);
	if (!rc) {
		throw std::runtime_error("Unable to set background");
	}

	rc = XSetForeground(_display, _gc, _fg);
	if (!rc) {
		throw std::runtime_error("Unable to set forground");
	}

	rc = XClearWindow(_display, _window);
	if (!rc) {
		throw std::runtime_error("Unable to clear window");
	}

	rc = XMapRaised(_display, _window);
	if (!rc) {
		throw std::runtime_error("Unable to map window");
	}
};

graphics::graphics() :
	_width{DEFAULT_WIDTH},
	_height{DEFAULT_HEIGHT},
	_name{DEFAULT_NAME}
{
	try {
		init_graphics();
	}
	catch (const std::exception& e) {
		throw;
	}
};

graphics::graphics(const char* s) :
	_width{DEFAULT_WIDTH},
	_height{DEFAULT_HEIGHT},
	_name{s}
{
	try {
		init_graphics();
	}
	catch (const std::exception& e) {
		throw;
	}
};

graphics::graphics(int w, int h) :
	_width{w},
	_height{h},
	_name{DEFAULT_NAME}
{
	try {
		init_graphics();
	}
	catch (const std::exception& e) {
		throw;
	}
};

graphics::graphics(int w, int h, const char *s) :
	_width{w},
	_height{h},
	_name{s}
{
	try {
		init_graphics();
	}
	catch (const std::exception& e) {
		throw;
	}
};

graphics::~graphics()
{

	if (_gc) {
		XFreeGC(_display, _gc);
	}

	if (_ximage) {
		XDestroyImage(_ximage);
	}

	XDestroyWindow(_display, _window);

	if (_colors) {
		delete _colors;
	}

	if (_display) {
		XCloseDisplay(_display);
	}
};

const graphics_base::bounds_status graphics::is_in_bounds(point p) const
{
	int rc = BOUNDS_OK;

	if (p.x < 0 || p.x > _width)
		rc |= BOUNDS_X_OUT;

	if (p.y < 0 || p.y > _height)
		rc |= BOUNDS_Y_OUT;

	return (graphics_base::bounds_status)rc;
};

const bool graphics::is_valid_color(color_idx i) const
{
	return (i >= __first_color__ && i < __last_color__);
};

const bool graphics::is_bright_color(color_idx i) const
{
	switch (i) {
	case white:
	case grey:
	case bright_yellow:
	case yellow:
	case bright_green:
	case green:
		return true;
	default:
		return false;
	}
};

const color_val graphics::get_color_val(color_idx i) const
{
	return _colors->find(i)->second.val;
};

const std::string graphics::get_color_name(color_idx i) const
{
	return _colors->find(i)->second.name;
};

void graphics::draw_pixel(point p, color_idx i) const
{
#ifdef DEBUG_GRFX
	std::string s = get_color_name(i) + SEP;
	int len = std::max((int)s.length(), 15);

	std::cout << DBG_PFX << PIXEL_PFX
		<< DEC(i, 2) << SEP << STR(s, len)
		<< DEC(p.x, 4) << SEP << DEC(p.y, 4) << SEP
		<< HEX(get_color_val(i), 8) << std::endl;
#endif //DEBUG_GRFX

	if (is_in_bounds(p) != BOUNDS_OK) {
		WARN("Out of bounds");
		return;
	}

	XSetForeground(_display, _gc, get_color_val(i));
	XDrawPoint(_display, _window, _gc, p.x, p.y);
};

void graphics::draw_line(point tl, point br, color_idx i) const
{
#ifdef DEBUG_GRFX
	std::string s = get_color_name(i) + SEP;
	int len = std::max((int)s.length(), 15);

	std::cout << DBG_PFX << LINE_PFX
		<< DEC(i, 2) << SEP << STR(s, len)
		<< DEC(tl.x, 4) << SEP << DEC(tl.y, 4) << SEP
		<< DEC(br.x, 4) << SEP << DEC(br.y, 4) << SEP
		<< HEX(get_color_val(i), 8) << std::endl;
#endif //DEBUG_GRFX

	if (is_in_bounds(tl) != BOUNDS_OK || is_in_bounds(br) != BOUNDS_OK) {
		WARN("Out of bounds");
		return;
	}

	XSetForeground(_display, _gc, get_color_val(i));
	XDrawLine(_display, _window, _gc, tl.x, tl.y, br.x, br.y);
};

void graphics::draw_rect(point tl, size sz, color_idx i, bool fill) const
{
#ifdef DEBUG_GRFX
	std::string s = get_color_name(i) + SEP;
	int len = std::max((int)s.length(), 15);

	std::cout << DBG_PFX << RECT_PFX
		<< DEC(i, 2) << SEP << STR(s, len)
		<< DEC(tl.x, 4) << SEP << DEC(tl.y, 4) << SEP
		<< DEC(sz.w, 4) << SEP << DEC(sz.h, 4) << SEP
		<< HEX(get_color_val(i), 8) << std::endl;
#endif //DEBUG_GRFX

	if (is_in_bounds(tl) != BOUNDS_OK || is_in_bounds({tl.x+sz.w, tl.y+sz.h}) != BOUNDS_OK) {
		WARN("Out of bounds");
		return;
	}

	XSetForeground(_display, _gc, get_color_val(i));
	if (fill) {
		XFillRectangle(_display, _window, _gc, tl.x, tl.y, sz.w, sz.h);
	}
	else {
		XDrawRectangle(_display, _window, _gc, tl.x, tl.y, sz.w, sz.h);
	}
};

void graphics::draw_text(point p, std::string s, color_idx i) const
{
#ifdef DEBUG_GRFX
	std::string s2 = get_color_name(i) + SEP;
	int len = std::max((int)s2.length(), 15);

	std::cout << DBG_PFX << TEXT_PFX
		<< DEC(i, 2) << SEP << STR(s2, len)
		<< DEC(p.x, 4) << SEP << DEC(p.y, 4) << SEP
		<< HEX(get_color_val(i), 8) << std::endl;
#endif //DEBUG_GRFX

	if (is_in_bounds(p) != BOUNDS_OK) {
		WARN("Out of bounds");
		return;
	}

	XTextItem txt{(char*)s.c_str(), (int)s.length(), 1, None};
	XSetForeground(_display, _gc, get_color_val(i));
	XDrawText(_display, _window, _gc, p.x, p.y, &txt, 1);
};

void graphics::refresh() const
{
	XEvent ev;
	Window root;
	unsigned int border_width, depth;

	XGetGeometry(_display, _window, &root,
				 &ev.xexpose.x, &ev.xexpose.y,
				 (unsigned int*)&ev.xexpose.width,
				 (unsigned int*)&ev.xexpose.height,
				 &border_width, &depth);

	ev.xexpose.type = Expose;
	ev.xexpose.display = _display;
	ev.xexpose.window = _window;
	ev.xexpose.count = 0;

	XClearWindow(_display, _window);
	XSendEvent(_display, _window, false, ExposureMask, &ev);
};

void graphics::demo() const
{
	int x = 0, y = 0, gap = 50, rc;
	point tl, br, p;
	size sz;

#ifdef DEBUG_GRFX
	std::cout << DBG_PFX << STR("Number of colors: ", 1) << DEC(get_num_colors(), 2) << std::endl;
#endif

	// filled rects
	for (color::const_iterator it = _colors->cbegin(); it != _colors->cend(); x+=gap, it++) {
		std::string s = std::to_string(it->first);
		color_idx c = white;

		if (is_bright_color(it->first)) {
			c = black;
		}

		tl = {x, y};
		sz = {gap, gap};
		br = {x+gap, y+gap};

		rc = is_in_bounds(br);
		switch (rc)
		{
		case BOUNDS_OK:
			DBG("BOUNDS_OK");
			break;
		case BOUNDS_X_OUT:
			DBG("BOUNDS_X_OUT");
			// start in a new row
			y += gap;
			x = 0;
			tl = {x, y};
			break;
		case BOUNDS_Y_OUT:
			DBG("BOUNDS_Y_OUT");
			return;
		case BOUNDS_BOTH_OUT:
			DBG("BOUNDS_BOTH_OUT");
			return;
		default:
			DBG("BOUNDS_BOTH_OUT");
			return;
		}

		draw_rect(tl, sz, it->first, true);
		p = {x + 10, y + 10};
		draw_text(p, s, c);
	}

	// empty rects
	y += gap;
	x = 0;

	for (color::const_iterator it = _colors->cbegin(); it != _colors->cend(); x += gap, it++) {
		std::string s = std::to_string(it->first);
		color_idx c = white;

		tl = {x, y};
		sz = {gap, gap};
		br = {x+gap, y+gap};

		rc = is_in_bounds(br);
		switch (rc) {
		case BOUNDS_OK:
			DBG("BOUNDS_OK");
			break;
		case BOUNDS_X_OUT:
			DBG("BOUNDS_X_OUT");
			// start in a new row
			y += gap;
			x = 0;
			tl = {x, y};
		break;
		case BOUNDS_Y_OUT:
			DBG("BOUNDS_Y_OUT");
			return;
		case BOUNDS_BOTH_OUT:
			DBG("BOUNDS_BOTH_OUT");
			return;
		default:
			DBG("BOUNDS_BOTH_OUT");
			return;
	  }

		draw_rect(tl, sz, it->first, false);
		p = {x + 10, y + 10};
		draw_text(p, s, c);
	}

	// lines
	y += gap*2;
	x = gap;
	tl = {x, y};
	br = {x+6*gap, y+gap};
	draw_line(tl, br, bright_yellow);

	// pixels
	y += gap;
	x = gap;
	p = {x, y};
	draw_pixel(p, bright_yellow);

	x += gap;
	for (int i = 0; i < 300; i++) {
		color_idx c = (color_idx)(i % get_num_colors());
#ifdef DEBUG_GRFX
		std::cout << DBG_PFX << STR("color: ", 1) << DEC(c, 2) << std::endl;
#endif

		if (!is_valid_color(c)) {
			WARN("Not a valid color");
			continue;
		}

		if (c == black) {
			continue;
		}

		p = {x+i*2, y+i};
		if (is_in_bounds(p) != BOUNDS_OK) {
			break;
		}

		draw_pixel(p, c);
	}
};

int graphics::take_snapshot()
{
	XImage *tmp_image = XGetImage(_display, _window, 0, 0, _width, _height, AllPlanes, ZPixmap);
	if (!tmp_image) {
		return -1;
	}

	if (_ximage) {
		drop_snapshot();
	}

	_ximage = tmp_image;
	return 0;
};

int graphics::drop_snapshot()
{
	XDestroyImage(_ximage);
	_ximage = NULL;
	return 0;
};

int graphics::show_snapshot() const
{
	if (!_ximage) {
		return -1;
	}

	XPutImage(_display, _window, _gc, _ximage, 0, 0, 0, 0, _width, _height);
	return 0;
};

} // namespace graphics_ns_x11

} // namespace graphics_ns_base
