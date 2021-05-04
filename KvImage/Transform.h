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
	/*计算imgRect与viewSize的交集，进而计算出img的范围，并进行缩放，得到QImage*/
	static QImage MatToQImage(const cv::Mat& img, cv::Size2d viewSize, cv::Rect2d imgRect);
	static QPixmap MatToQPixmap(const cv::Mat& img);
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size2d imgSize);
	/*计算imgRect与viewSize的交集，进而计算出img的范围，并进行缩放，得到QPixmap*/
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size2d viewSize, cv::Rect2d imgRect);
	static cv::Mat QImageToMat(const QImage& qimg, bool cloneImageData = true);
	static cv::Mat QPixmapToMat(const QPixmap& pxm, bool cloneImageData = true);
};

