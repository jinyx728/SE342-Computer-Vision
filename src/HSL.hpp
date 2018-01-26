#ifndef OPENCV2_PS_HSL_HPP_
#define OPENCV2_PS_HSL_HPP_

#include "opencv2/core.hpp"
using namespace cv;

namespace cv {

enum HSL_COLOR
{
	HSL_ALL,
	HSL_RED,
	HSL_YELLOW,
	HSL_GREEN,
	HSL_CYAN,
	HSL_BLUE,
	HSL_MAGENTA,
};


class HSLChannel {
public:
    int hue;
    int saturation;
    int brightness;

    int   colorIndex;
    float left_left;
    float left;
    float right;
    float right_right;
    bool defined;

	HSLChannel();
	virtual ~HSLChannel();

	void calcDefined();
	void setColorIndex(int index);
	bool match(float hue);
	void adjust(int h, float *delta_hsb);
};


class HSL {
public:
	HSL();
	virtual ~HSL();

	HSLChannel channels[7];

	int adjust(InputArray src, OutputArray dst);
};

}

#endif
