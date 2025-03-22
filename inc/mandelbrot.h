#ifndef __MANDELBROT_X11_H__
#define __MANDELBROT_X11_H__

#include <vector>
#include <complex>
#include <stdint.h>

namespace mandelbrot_ns {

class mandelbrot
{
public:
	struct mand_data {
		uint32_t iterations {0};
		uint32_t width {0};
		uint32_t height {0};
		double left {0};
		double right {0};
		double top {0};
		double bottom {0};
		double limit {0};
	};

	typedef uint32_t cell;
	typedef std::vector<cell> row;
	typedef std::vector<row> plane;

	mandelbrot();
	mandelbrot(mand_data&);
	~mandelbrot();
	void compute(plane&);

private:
	typedef std::complex<double> point;

	mand_data _data;
	double _xstep;
	double _ystep;

	uint32_t is_in_set(point&) const;
};

} // namespace mandelbrot_ns

#endif // __MANDELBROT_X11_H__