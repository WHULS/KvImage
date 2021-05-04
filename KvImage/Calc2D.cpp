#include "Calc2D.h"

cv::Rect2d Calc2D::calcImageRect(const cv::Mat& img, const cv::Size2d& viewSize)
{
    // 计算缩放比例
    double xScale, yScale;
    xScale = viewSize.width / double(img.cols);
    yScale = viewSize.height / double(img.rows);
    double scale = std::min(xScale, yScale);

    // 计算缩放后的图片矩形（居中显示)
    cv::Rect2d imgRect;
    imgRect.width = double(img.cols) * scale;
    imgRect.height = double(img.rows) * scale;
    imgRect.x = double(viewSize.width - imgRect.width) / 2;
    imgRect.y = double(viewSize.height - imgRect.height) / 2;

    return imgRect;
}

bool Calc2D::getPxFromImage(px& p, const cv::Point& pt, const cv::Mat& img, const cv::Rect& imageRect)
{
	if (imageRect.contains(pt))
	{
		p.x = double(pt.x - imageRect.x) * (double(img.cols) / double(imageRect.width));
		p.y = double(pt.y - imageRect.y) * (double(img.rows) / double(imageRect.height));
		p.channels = img.type() == CV_8UC3 ? 3 : 1;
		if (p.channels == 3)
		{
			cv::Vec3b color = img.at<cv::Vec3b>(int(p.y), int(p.x));
			p.r = color[2];
			p.g = color[1];
			p.b = color[0];
		}
		else
		{
			p.r = img.at<unsigned char>(int(p.y), int(p.x));
		}
		return true;
	}

    return false;
}

cv::Rect2d Calc2D::calcImageRectWithViewRect(const cv::Mat& img, const cv::Size2d& viewSize, const cv::Rect2d& subRect)
{
	if (viewSize.height == 0 || viewSize.width == 0 || subRect.height == 0 || subRect.width == 0)
	{
		qWarning() << "Nothing to calculate!";
		return cv::Rect2d();
	}
	double xScale, yScale;
	xScale = viewSize.width / double(subRect.width);
	yScale = viewSize.height / double(subRect.height);
	double scale = std::min(xScale, yScale);

	cv::Rect2d rect;
	rect.width = double(img.cols) * scale;
	rect.height = double(img.rows) * scale;
	rect.x = viewSize.width / 2 - (subRect.x + subRect.width / 2) * scale;
	rect.y = viewSize.height / 2 - (subRect.y + subRect.height / 2) * scale;

	return rect;
}
