// #define DEBUG_GRFX
#include <common.h>
#include <mandelbrot.h>

namespace mandelbrot_ns {

const uint32_t DEF_ITERS  = 25;
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

	if (d.left)
		_data.left = d.left;

	if (d.right)
		_data.right = d.right;

	if (d.top)
		_data.top = d.top;

	if (d.bottom)
		_data.bottom = d.bottom;

	if (d.limit)
		_data.limit = d.limit;

	_xrange = _data.right - _data.left;
	_yrange = _data.top - _data.bottom;

	DBG("mandelbrot:" << ENDL
		<< STR("  iterations:", 14) << DEC(_data.iterations, 4) << ENDL
		<< STR("  width:", 14) << DEC(_data.width, 4) << ENDL
		<< STR("  height:", 14) << DEC(_data.height, 4) << ENDL
		<< STR("  left:", 14) << DBL(_data.left, 6) << ENDL
		<< STR("  right:", 14) << DBL(_data.right, 6) << ENDL
		<< STR("  top:", 14) << DBL(_data.top, 6) << ENDL
		<< STR("  bottom:", 14) << DBL(_data.bottom, 6) << ENDL
		<< STR("  limit:", 14) << DBL(_data.limit, 6) << ENDL);

	if (_xrange <= 0.0 || _yrange <= 0.0 || _data.iterations < 1
		|| _data.width < 1 || _data.height < 1 || _data.limit <= 0.0) {
		throw std::runtime_error("mandelbrot: illegal values");
	}

	_xstep = _xrange / (double)_data.width;
	_ystep = _yrange / (double)_data.height;

	_xcenter=0;
	while (_xcenter<(uint32_t)_data.width) {
		if ((_data.left + _xcenter * _xstep) >= 0.0)
			break;
		_xcenter++;
	}

	_ycenter=0;
	while (_ycenter<(uint32_t)_data.height) {
		if ((_data.bottom + _ycenter * _ystep) >= 0.0)
			break;
		_ycenter++;
	}

	DBG("mandelbrot:" << ENDL
		<< STR("  x range:", 14) << DBL(_xrange, 15) << ENDL
		<< STR("  y range:", 14) << DBL(_yrange, 15) << ENDL
		<< STR("  x step:", 14) << DBL(_xstep, 18) << ENDL
		<< STR("  y step:", 14) << DBL(_ystep, 18) << ENDL);
};

uint32_t mandelbrot::is_in_set(point& z0) const
{
	point z{0.0, 0.0};
	uint32_t it = 0;

	while (it < _data.iterations) {
		z = z * z + z0;
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
				<< SEP << DBL(y_pos, 6) << SEP << DBL(x_pos, 6));
		}
	}
};

int mandelbrot::translate_position(mand_pos& p) const
{
	p.dx = _data.left + p.ix * _xstep;
	p.dy = _data.top - p.iy * _ystep;
	return 0;
};

} // namespace mandelbrot_ns
