#ifndef __GRAPHICS_X11_H__
#define __GRAPHICS_X11_H__

#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <graphics_base.h>

const std::string ERR_PFX	= "Error: ";
const std::string WARN_PFX	= "Warning: ";
const std::string INFO_PFX	= "Info: ";
const std::string HEX_PFX	= "0x";
const std::string SEP		= ", ";

#define HEX(n, w) HEX_PFX << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n, w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define STR(s, w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)	  std::cout << STR(ERR_PFX, 1) << STR((s), 1) << std::endl;
#define WARN(s)	  std::cout << STR(WARN_PFX, 1) << STR((s), 1) << std::endl;
#define INFO(s)	  std::cout << STR(INFO_PFX, 1) << STR((s), 1) << std::endl;

#ifdef DEBUG_GRFX
const std::string DBG_PFX	= "Debug: ";
#define DBG(s)    std::cout << STR(DBG_PFX, 1) << STR((s), 1) << std::endl
#else
#define DBG(...)
#endif //DEBUG_GRFX

namespace graphics_ns_base {

namespace graphics_ns_x11 {

class graphics : graphics_base
{
private:
	struct _rgb {
		uint16_t r = 0;
		uint16_t g = 0;
		uint16_t b = 0;
	};

	struct color_data
	{
		std::string name;
		_rgb rgb;
		color_val val;
		bool done = false;
		bool bright = false;
	};

	typedef std::map<color_idx, color_data> color;

	Display* _display {NULL};
	GC _gc {NULL};
	Visual* _visual {NULL};
	Window _window;
	Window _root;
	int _screen;
	Colormap _cmap;
	XImage* _ximage {NULL};
	int _width;
	int _height;
	color_idx _bg {black};
	color_idx _fg {white};
	const char* _name;
	color *_colors {NULL};

	void init_colors();
	void init_graphics();

public:
	graphics();
	graphics(const char*);
	graphics(int, int);
	graphics(int, int, const char*);
	~graphics();

	// implement pure virtuals
	const bounds_status is_in_bounds(point) const;
	const bool is_valid_color(color_idx) const;
	inline const int get_num_colors() const {return __last_color__;};
	inline const int get_width() const {return _width;};
	inline const int get_height() const {return _height;};
	const color_val get_color_val(color_idx) const;
	const std::string get_color_name(color_idx) const;
	void draw_pixel(point, color_idx) const;
	void draw_line(point, point, color_idx) const;
	void draw_rect(point, size, color_idx, bool) const;
	void draw_text(point, std::string, color_idx) const;
	void refresh() const;

	inline const Display* get_display() const {return _display;};
	const bool is_bright_color(color_idx) const;
	void demo() const;
	int take_snapshot();
	int drop_snapshot();
	int show_snapshot() const;
	inline const bool snapshot_exists() const {return (_ximage != NULL);};
};

} // namespace graphics_ns_x11

} // namespace graphics_ns_base

#endif // __GRAPHICS_X11_H__