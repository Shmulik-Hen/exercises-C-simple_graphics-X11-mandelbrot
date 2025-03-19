#ifndef __GRAPHICS_BASE_H__
#define __GRAPHICS_BASE_H__

#include <string>
#include <stdint.h>

namespace graphics_ns_base
{

typedef unsigned long color_val;

class graphics_base
{
private:
	virtual void init_graphics() = 0;

public:
	enum color_idx
	{
		__first_color__  = 0, // Dummy, not an qctual color

		black = __first_color__,
		white,
		grey,
		dark_grey,

		bright_red,
		red,
		dark_red,

		bright_orange,
		orange,
		dark_orange,

		bright_yellow,
		yellow,
		dark_yellow,

		bright_green,
		green,
		dark_green,

		bright_blue,
		blue,
		dark_blue,

		bright_purple,
		purple,
		dark_purple,

		__last_color__ // Dummy, not an qctual color
	};

	enum bounds_status
	{
		BOUNDS_OK   	= 0x00000000,
		BOUNDS_X_OUT    = 0x00000001,
		BOUNDS_Y_OUT    = 0x00000002,
		BOUNDS_BOTH_OUT	= (BOUNDS_X_OUT | BOUNDS_Y_OUT)
	};

	struct point
	{
		int x = 0;
		int y = 0;
	};

	struct size
	{
		int w = 0;
		int h = 0;
	};

	virtual const bounds_status is_in_bounds(point) const = 0;
	virtual const bool is_valid_color(color_idx) const = 0;
	virtual const int get_num_colors() const = 0;
	virtual const int get_width() const = 0;
	virtual const int get_height() const = 0;
	virtual const color_val get_color_val(color_idx) const = 0;
	virtual const std::string get_color_name(color_idx) const = 0;
	virtual void draw_pixel(point, color_idx) const = 0;
	virtual void draw_line(point, point, color_idx) const = 0;
	virtual void draw_rect(point, size, color_idx, bool) const = 0;
	virtual void draw_text(point, std::string, color_idx) const = 0;
	virtual void refresh() const = 0;
};

} // namespace graphics_ns_base

#endif // __GRAPHICS_BASE_H__
