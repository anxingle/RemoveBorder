#ifndef _IMAGE_PROCESS_H_
#define _IMAGE_PROCESS_H_
#include <opencv2/opencv.hpp>

struct SegPosition {
	int top_left_x;
	int top_left_y;
	int down_right_x;
	int down_right_y;
	SegPosition(int t = 0, int d = 0, int l = 0, int r = 0) : 
		top_left_x(t), top_left_y(d), down_right_x(l), down_right_y(r) {}
};

int GetOtsuThreshold(const IplImage *frame);

void GetsegPosition(const IplImage *img, int threshold, SegPosition& seg_position);

IplImage* extractValidPart(IplImage *img_color, SegPosition& seg_position);

#endif // !_IMAGE_PROCESS_H_
