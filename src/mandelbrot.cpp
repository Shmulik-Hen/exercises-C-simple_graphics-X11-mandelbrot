#include <mandelbrot.h>
#include <common.h>

namespace mandelbrot_ns {

#define DEF_ITERS	200
#define DEF_WIDTH	400
#define DEF_HEIGHT	300
#define DEF_LEFT	-1.5
#define DEF_RIGHT	1.0
#define DEF_TOP		1.0
#define DEF_BOTTOM	-1.0
#define DEF_LIMIT	2.0

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

uint32_t mandelbrot::is_in_set(point& t) const
{
	point c0 {t}, z{0, 0};
	uint32_t iters = 0;

	while (iters++ < _data.iterations) {
		z = z * z + c0;
		if (std::abs(z) > _data.limit)
			break;
	}

	return iters;
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

	_xstep = (_data.right - _data.left) / (double)_data.width;
	_ystep = (_data.top - _data.bottom) / (double)_data.height;

	std::cout << "iterations: " << _data.iterations << std::endl;
	std::cout << "width: " << _data.width << std::endl;
	std::cout << "height: " << _data.height << std::endl;
	std::cout << "left: " << _data.left << std::endl;
	std::cout << "right: " << _data.right << std::endl;
	std::cout << "top: " << _data.top << std::endl;
	std::cout << "bottom: " << _data.bottom << std::endl;
	std::cout << "limit: " << _data.limit << std::endl;
	std::cout << "xstep: " << _xstep << std::endl;
	std::cout << "ystep: " << _ystep << std::endl;
};

mandelbrot::~mandelbrot()
{
};

void mandelbrot::compute(plane& p)
{
	p.resize(_data.height);

	for (uint32_t y = 0; y < _data.height; y++) {
		row r = p[y];
		r.resize(_data.width);
		double y_pos = (double)y * _ystep;
		for (uint32_t x = 0; x < _data.width; x++) {
			double x_pos = (double)x * _xstep;
			point d = {x_pos, y_pos};
			r[x] = is_in_set(d);
		}
	}
};

} // namespace mandelbrot_ns
