#pragma once
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QDebug>

class Transform
{
public:
	static QImage MatToQImage(const cv::Mat &img);
	static QImage MatToQImage(const cv::Mat& img, cv::Size imgSize);
	static QImage MatToQImage(const cv::Mat& img, cv::Size viewSize, cv::Rect imgRect);
	static QPixmap MatToQPixmap(const cv::Mat& img);
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size imgSize);
	static QPixmap MatToQPixmap(const cv::Mat& img, cv::Size viewSize, cv::Rect imgRect);
	static cv::Mat QImageToMat(const QImage& qimg, bool cloneImageData = true);
	static cv::Mat QPixmapToMat(const QPixmap& pxm, bool cloneImageData = true);

	/* 计算imgSize约束下的图片矩形 */
	static cv::Rect calcImageRect(const cv::Mat& img, cv::Size imgSize);
};

