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

	typedef std::vector<uint32_t> row_t;
	typedef std::vector<row_t> plane_t;

	mandelbrot();
	mandelbrot(mand_data&);
	~mandelbrot(){};
	void compute(plane_t&);
	inline const uint32_t get_x_center() const { return _xcenter; };
	inline const uint32_t get_y_center() const { return _ycenter; };

      private:
	typedef std::complex<double> point;

	mand_data _data;
	double _xstep;
	double _ystep;
	double _xrange;
	double _yrange;
	uint32_t _xcenter;
	uint32_t _ycenter;

	uint32_t is_in_set(point&) const;
};

} // namespace mandelbrot_ns

#endif // __MANDELBROT_X11_H__