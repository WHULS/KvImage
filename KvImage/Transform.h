#pragma once
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include "Calc2D.h"

class Transform
{
public:
	static QImage MatToQImage(const cv::Mat &img);
	static QImage MatToQImage(const cv::Mat& img, cv::Size2d imgSize);
	/*����imgRect��viewSize�Ľ��������������img�ķ�Χ�����������ţ��õ�QImage*/
	static QImage MatToQImage(const cv::Mat& img, cv::Size2d viewSize, cv::Rect2d imgRect);
	static QPixmap MatToQPixmap(const cv::Mat& img);
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size2d imgSize);
	/*����imgRect��viewSize�Ľ��������������img�ķ�Χ�����������ţ��õ�QPixmap*/
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size2d viewSize, cv::Rect2d imgRect);
	static cv::Mat QImageToMat(const QImage& qimg, bool cloneImageData = true);
	static cv::Mat QPixmapToMat(const QPixmap& pxm, bool cloneImageData = true);
};

