#pragma once
#include <opencv2/opencv.hpp>
#include "px.h"

class Calc2D
{
public:
	/* 计算imgSize约束下的图片矩形 */
	static cv::Rect calcImageRect(const cv::Mat& img, cv::Size imgSize);

	/*获取图像上某个点的像素，图像与图像矩形可能不一致，pt与imageRect在同个坐标系下，imageRect与img的缩放关系一至*/
	static bool getPxFromImage(px& p, const cv::Point& pt, const cv::Mat& img, const cv::Rect& imageRect);
};

