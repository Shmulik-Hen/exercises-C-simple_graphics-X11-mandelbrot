#ifndef __GRAPHICS_X11_H__
#define __GRAPHICS_X11_H__

#include <map>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <graphics_base.h>


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
	color *_colors {NULL};
	uint32_t _bgc {0};
	uint32_t _fgc {0};
	uint32_t _brc {0};
	uint32_t _brw {0};
	int32_t _em {0};
	uint32_t _width;
	uint32_t _height;
	const char* _name;

	// window geometry
	int32_t _geo_x {0};
	int32_t _geo_y {0};
	uint32_t _geo_w {0};
	uint32_t _geo_h {0};
	uint32_t _geo_b {0};
	uint32_t _geo_d {0};

	void init_colors();
	void init_graphics();

public:
	graphics();
	graphics(const char*);
	graphics(uint32_t, uint32_t);
	graphics(uint32_t, uint32_t, const char*);
	~graphics();

	// implement pure virtuals
	const bounds_status is_in_bounds(point) const;
	const bool is_valid_color(color_idx) const;
	inline const uint32_t get_num_colors() const {return __last_color__;};
	inline const uint32_t get_width() const {return _geo_w;};
	inline const uint32_t get_height() const {return _geo_h;};
	const color_val get_color_val(color_idx) const;
	const std::string get_color_name(color_idx) const;
	void draw_pixel(point, color_idx) const;
	void draw_line(point, point, color_idx) const;
	void draw_rect(point, size, color_idx, bool) const;
	void draw_text(point, std::string, color_idx) const;
	void refresh() const;

	inline const bool snapshot_exists() const { return (_ximage != NULL); };
	const bool wait_event(XEvent*) const;
	const void flush() const;
	const bool is_bright_color(color_idx) const;
	int put_pixel(point, color_idx) const;
	int take_snapshot();
	int drop_snapshot();
	int show_snapshot() const;
	void demo() const;
};

} // namespace graphics_ns_x11

} // namespace graphics_ns_base

#endif // __GRAPHICS_X11_H__
