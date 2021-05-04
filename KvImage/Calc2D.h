#pragma once
#include <opencv2/opencv.hpp>
#include "px.h"
#include <QDebug>

class Calc2D
{
public:
	/* 计算viewSize约束下的图片矩形 */
	static cv::Rect2d calcImageRect(const cv::Mat& img, const cv::Size2d& viewSize);

	/*获取图像上某个点的像素，图像与图像矩形可能不一致，pt与imageRect在同个坐标系下，imageRect与img的缩放关系一至*/
	static bool getPxFromImage(px& p, const cv::Point& pt, const cv::Mat& img, const cv::Rect& imageRect);

	/*要在显示区viewSize显示图上的一部分区域subRect，反算出整体的矩形大小*/
	static cv::Rect2d calcImageRectWithViewRect(const cv::Mat& img, const cv::Size2d& viewSize, const cv::Rect2d& subRect);
};

