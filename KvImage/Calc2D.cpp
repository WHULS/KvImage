#include "Calc2D.h"

cv::Rect Calc2D::calcImageRect(const cv::Mat& img, cv::Size imgSize)
{
    // 计算缩放比例
    double xScale, yScale;
    xScale = double(imgSize.width) / double(img.cols);
    yScale = double(imgSize.height) / double(img.rows);
    double scale = std::min(xScale, yScale);

    // 计算缩放后的图片矩形（居中显示)
    cv::Rect imgRect;
    imgRect.width = int(img.cols * scale);
    imgRect.height = int(img.rows * scale);
    imgRect.x = int((imgSize.width - imgRect.width) / 2);
    imgRect.y = int((imgSize.height - imgRect.height) / 2);

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