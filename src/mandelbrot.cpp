// #define DEBUG_GRFX
#include <common.h>
#include <mandelbrot.h>

namespace mandelbrot_ns {

const uint32_t DEF_ITERS  = 200;
const uint32_t DEF_WIDTH  = 400;
const uint32_t DEF_HEIGHT = 300;
const double DEF_LEFT	= -2.5;
const double DEF_RIGHT	= 1.0;
const double DEF_TOP	= 1.0;
const double DEF_BOTTOM	= -1.0;
const double DEF_LIMIT	= 2.0;

mandelbrot::mand_data def_data = {
	DEF_ITERS,
	DEF_WIDTH,
	DEF_HEIGHT,
	DEF_LEFT,
	DEF_RIGHT,
	DEF_TOP,
	DEF_BOTTOM,
	DEF_LIMIT
};

mandelbrot::mandelbrot()
{
	_data = def_data;
};

mandelbrot::mandelbrot(mand_data &d)
{
	_data = def_data;

	if (d.iterations)
		_data.iterations = d.iterations;

	if (d.width)
		_data.width = d.width;

	if (d.height)
		_data.height = d.height;

	// if (d.left)
	// 	_data.left = d.left;

	// if (d.right)
	// 	_data.right = d.right;

	// if (d.top)
	// 	_data.top = d.top;

	// if (d.bottom)
	// 	_data.bottom = d.bottom;

	if (d.limit)
		_data.limit = d.limit;

	_xstep = (_data.right - _data.left) / (double)_data.width;
	_ystep = (_data.top - _data.bottom) / (double)_data.height;

	INFO(STR("iterations: ", 12) << DEC(_data.iterations, 3));
	INFO(STR("width:", 12)  << DEC(_data.width, 3));
	INFO(STR("height:", 12)  << DEC(_data.height, 3));
	INFO(STR("left:", 12)  << FLT(_data.left, 6));
	INFO(STR("right:", 12)  << FLT(_data.right, 6));
	INFO(STR("top:", 12)  << FLT(_data.top, 6));
	INFO(STR("bottom:", 12)  << FLT(_data.bottom, 6));
	INFO(STR("limit:", 12)  << FLT(_data.limit, 6));
	INFO(STR("xstep:", 12)  << FLT(_xstep, 6));
	INFO(STR("ystep:", 12)  << FLT(_ystep, 6));
};

uint32_t mandelbrot::is_in_set(point& c0) const
{
	point z{0.0, 0.0};
	uint32_t it = 0;

	while (it < _data.iterations) {
		z = z * z + c0;
		if (std::abs(z) > _data.limit)
			break;
		it++;
	}

	return it;
};

void mandelbrot::compute(plane_t& p)
{
	uint32_t x, y;
	double x_pos, y_pos;

	for (y=0, y_pos=_data.top; y<p.size() && y_pos>_data.bottom; y++, y_pos-=_ystep) {
		for (x=0, x_pos=_data.left; x<p[y].size() && x_pos<_data.right; x++, x_pos+=_xstep) {
			point pt = {x_pos, y_pos};
			p[y][x] = is_in_set(pt);
			DBG("iter: p[" << DEC(y, 1) << "][" << DEC(x, 1) << "] = " << DEC(p[y][x], 4)
				<< SEP << FLT(y_pos, 6) << SEP << FLT(x_pos, 6));
		}
	}
};

} // namespace mandelbrot_ns
