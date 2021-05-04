#include "RotRect2D.h"

// 向points数组中加入两点之间的点的坐标
// 取整，且包含起点，不包含终点
static void pushPoints(cv::Point2d start, cv::Point2d end, std::vector<cv::Point>& points)
{
	// 起止点相同
	if (abs(start.x - end.x) < 1e-5 && abs(start.y - end.y) < 1e-5) return;
	// 垂直于x轴
	else if (abs(start.x - end.x) < 1e-5)
	{
		int x = cvRound(start.x);
		int y1 = cvRound(start.y),
			y2 = cvRound(end.y);
		if (y1 > y2) std::swap(y1, y2);

		for (int y = y1; y < y2; y++) points.push_back(cv::Point(x, y));

	}
	// 垂直于y轴
	else if (abs(start.y - end.y) < 1e-5)
	{
		int y = cvRound(start.y);
		int x1 = cvRound(start.x),
			x2 = cvRound(end.x);
		if (x1 > x2) std::swap(x1, x2);

		for (int x = x1; x < x2; x++) points.push_back(cv::Point(x, y));
	}
	// x方向间距大
	else if (abs(start.x - end.x) >= abs(start.y - end.y))
	{
		if (cvRound(start.x) > cvRound(end.x)) std::swap(start, end);

		int x1 = cvRound(start.x),
			x2 = cvRound(end.x);
		double y1 = start.y,
			y2 = end.y,
			deltaY = (y2 - y1) / (x2 - x1);

		for (int x = x1; x < x2; x++)
		{
			int y = cvRound(start.y + (double(x) - start.x) * deltaY);
			points.push_back(cv::Point(x, y));
		}
	}
	// y方向间距大
	else if (abs(start.x - end.x) < abs(start.y - end.y))
	{
		if (cvRound(start.y) > cvRound(end.y)) std::swap(start, end);

		int y1 = cvRound(start.y),
			y2 = cvRound(end.y);
		double x1 = start.x,
			x2 = end.x,
			deltaX = (x2 - x1) / (y2 - y1);

		for (int y = y1; y < y2; y++)
		{
			int x = cvRound(start.x + (double(y) - start.y) * deltaX);
			points.push_back(cv::Point(x, y));
		}
	}
}

RotRect2D::RotRect2D(cv::Vec4d centerLine, double lengthExpand, double widthExpand)
{
	double x1, y1, x2, y2;
	x1 = centerLine[0];
	y1 = centerLine[1];
	x2 = centerLine[2];
	y2 = centerLine[3];

	double angle = atan2(y2 - y1, x2 - x1);

	this->mPt1 = cv::Point2d(x1 - widthExpand * sin(angle) - lengthExpand * cos(angle), y1 + widthExpand * cos(angle) - lengthExpand * sin(angle));
	this->mPt2 = cv::Point2d(x1 + widthExpand * sin(angle) - lengthExpand * cos(angle), y1 - widthExpand * cos(angle) - lengthExpand * sin(angle));
	this->mPt3 = cv::Point2d(x2 - widthExpand * sin(angle) + lengthExpand * cos(angle), y2 + widthExpand * cos(angle) + lengthExpand * sin(angle));
	this->mPt4 = cv::Point2d(x2 + widthExpand * sin(angle) + lengthExpand * cos(angle), y2 - widthExpand * cos(angle) + lengthExpand * sin(angle));

	this->mSize = cv::Size2d(double(sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2)) + 2 * lengthExpand), double(2 * widthExpand));
	this->mInitLine = centerLine;
}

RotRect2D::RotRect2D(cv::Point2d pt1, cv::Point2d pt2, cv::Point2d pt3, cv::Point2d pt4)
{
	this->mPt1 = pt1;
	this->mPt2 = pt2;
	this->mPt3 = pt3;
	this->mPt4 = pt4;
}

RotRect2D::RotRect2D()
{
	this->mPt1 = this->mPt2 = this->mPt3 = this->mPt4 = cv::Point2d(0, 0);
}

double RotRect2D::getRotateAngle() const
{
	return atan2(this->mPt3.y - this->mPt1.y, this->mPt3.x - this->mPt1.x);
}

cv::Point2d RotRect2D::getRectCenter() const
{
	return (this->mPt1 + this->mPt2 + this->mPt3 + this->mPt4) / 4;
}

cv::Size2d RotRect2D::getRectSize() const
{
	return this->mSize;
}


std::vector<cv::Vec4d> RotRect2D::getEdgeLines() const
{
	std::vector<cv::Vec4d> lines;

	// 13
	lines.push_back(cv::Vec4d(this->mPt1.x, this->mPt1.y, this->mPt3.x, this->mPt3.y));

	// 24
	lines.push_back(cv::Vec4d(this->mPt2.x, this->mPt2.y, this->mPt4.x, this->mPt4.y));

	// 12
	lines.push_back(cv::Vec4d(this->mPt1.x, this->mPt1.y, this->mPt2.x, this->mPt2.y));

	// 34
	lines.push_back(cv::Vec4d(this->mPt3.x, this->mPt3.y, this->mPt4.x, this->mPt4.y));

	return lines;
}

void RotRect2D::getCenterPoints(std::vector<cv::Point>& points) const
{
	cv::Point2d s, e;
	s.x = (this->mPt1.x + this->mPt2.x) / 2;
	s.y = (this->mPt1.y + this->mPt2.y) / 2;
	e.x = (this->mPt3.x + this->mPt4.x) / 2;
	e.y = (this->mPt3.y + this->mPt4.y) / 2;
	pushPoints(s, e, points);
}

void RotRect2D::drawCornerPoints(cv::Mat& showImage, cv::Scalar color) const
{
	if (color == cv::Scalar::all(-1))
	{
		cv::RNG rng(cvGetTickCount());
		color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	}
	circle(showImage, this->mPt1, 2, color);
	putText(showImage, "pt1", this->mPt1, 0, 0.5, color);
	circle(showImage, this->mPt2, 2, color);
	putText(showImage, "pt2", this->mPt2, 0, 0.5, color);
	circle(showImage, this->mPt3, 2, color);
	putText(showImage, "pt3", this->mPt3, 0, 0.5, color);
	circle(showImage, this->mPt4, 2, color);
	putText(showImage, "pt4", this->mPt4, 0, 0.5, color);
}


void RotRect2D::drawEdgeLines(cv::Mat& showImage, cv::Scalar color, double lineWidth) const
{
	std::vector<cv::Vec4d> lines = this->getEdgeLines();
	if (color == cv::Scalar::all(-1))
	{
		cv::RNG rng(cvGetTickCount());
		color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	}
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(showImage, cv::Point2d(lines[i][0], lines[i][1]), cv::Point2d(lines[i][2], lines[i][3]), color, lineWidth);
	}
}
void RotRect2D::drawCenterLines(cv::Mat& showImage, cv::Scalar color, double lineWidth) const
{
	if (color == cv::Scalar::all(-1))
	{
		cv::RNG rng(cvGetTickCount());
		color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	}
	cv::Vec4d cl = this->getCenterLineAbs();
	line(showImage, cv::Point2d(cl[0], cl[1]), cv::Point2d(cl[2], cl[3]), color, lineWidth);
}

void RotRect2D::deleteOuterPoints(cv::Mat& src, cv::Rect2d rect, unsigned char light_thr) const
{
	if (src.channels() == 3) src.convertTo(src, cv::COLOR_BGR2GRAY);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i, j) > light_thr && !this->contain(j + rect.x, i + rect.y))
			{
				src.at<uchar>(i, j) = 0;
			}
		}
	}
}

void RotRect2D::getInnerPoints(cv::Mat& src, std::vector<iPoint>& ipts, cv::Rect2d rect, unsigned char light_thr) const
{
	if (src.channels() == 3) src.convertTo(src, cv::COLOR_BGR2GRAY);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i, j) > light_thr && this->contain(j + rect.x, i + rect.y))
			{
				iPoint pt(cvRound(j + rect.x), cvRound(i + rect.y), src.at<uchar>(i, j));
				ipts.push_back(pt);
			}
		}
	}
}


void RotRect2D::rotate(double angle)
{
	cv::Point2d center = (this->mPt1 + this->mPt4 + this->mPt2 + this->mPt3) / 4;

	// 移动到坐标原点
	this->mPt1 -= center;
	this->mPt2 -= center;
	this->mPt3 -= center;
	this->mPt4 -= center;

	// 旋转
	this->mPt1 = cv::Point2d(this->mPt1.x * cos(angle) - this->mPt1.y * sin(angle), this->mPt1.x * sin(angle) + this->mPt1.y * cos(angle));
	this->mPt2 = cv::Point2d(this->mPt2.x * cos(angle) - this->mPt2.y * sin(angle), this->mPt2.x * sin(angle) + this->mPt2.y * cos(angle));
	this->mPt3 = cv::Point2d(this->mPt3.x * cos(angle) - this->mPt3.y * sin(angle), this->mPt3.x * sin(angle) + this->mPt3.y * cos(angle));
	this->mPt4 = cv::Point2d(this->mPt4.x * cos(angle) - this->mPt4.y * sin(angle), this->mPt4.x * sin(angle) + this->mPt4.y * cos(angle));

	// 移回原位
	this->mPt1 += center;
	this->mPt2 += center;
	this->mPt3 += center;
	this->mPt4 += center;

}

void RotRect2D::translation(cv::Point2d offset)
{
	this->mPt1 += offset;
	this->mPt2 += offset;
	this->mPt3 += offset;
	this->mPt4 += offset;
}

void RotRect2D::zoom(double scale)
{
	cv::Point2d center = (this->mPt1 + this->mPt4 + this->mPt2 + this->mPt3) / 4;

	// 移动到坐标原点
	this->mPt1 -= center;
	this->mPt2 -= center;
	this->mPt3 -= center;
	this->mPt4 -= center;

	// 缩放
	this->mPt1 *= scale;
	this->mPt2 *= scale;
	this->mPt3 *= scale;
	this->mPt4 *= scale;

	// 移回原位
	this->mPt1 += center;
	this->mPt2 += center;
	this->mPt3 += center;
	this->mPt4 += center;

}


cv::Rect2d RotRect2D::getOuterRect() const
{
	double bottom, left, right, top;
	bottom = left = 9999.0f;
	right = top = -9999.0f;

	std::vector<cv::Point2d> points;
	points.push_back(this->mPt1);
	points.push_back(this->mPt2);
	points.push_back(this->mPt3);
	points.push_back(this->mPt4);

	for (size_t i = 0; i < points.size(); i++)
	{
		if (points[i].x < left)
		{
			left = points[i].x;
		}
		if (points[i].x > right)
		{
			right = points[i].x;
		}
		if (points[i].y < bottom)
		{
			bottom = points[i].y;
		}
		if (points[i].y > top)
		{
			top = points[i].y;
		}
	}

	return cv::Rect2d(left, bottom, right - left, top - bottom);
}

cv::Mat RotRect2D::getMask(const cv::Rect& refMatRect) const
{
	cv::Rect2d r = this->getOuterRect();
	cv::Mat mask = cv::Mat::zeros(refMatRect.height, refMatRect.width, CV_8UC1);
	for (int i = cvRound(r.y); i < cvRound(r.y + r.height); i++)
	{
		for (int j = cvRound(r.x); j < cvRound(r.x + r.width); j++)
		{
			if (this->contain(j, i))
			{
				mask.at<uchar>(i, j) = 1;
			}
		}
	}

	return mask;
}


bool RotRect2D::contain(cv::Point2d pt) const
{
	// 使用余弦定理计算，AB/AC为夹角边，BC为角对应的边
	// BC*BC = AB*AB + AC*AC - 2*AB*AC*cos(A)
	double AB, AC, BC;
	double a13, a34, a24, a12; // 四个角

	// 13
	AB = sqrt(pow(this->mPt1.x - pt.x, 2) + pow(this->mPt1.y - pt.y, 2));
	AC = sqrt(pow(this->mPt3.x - pt.x, 2) + pow(this->mPt3.y - pt.y, 2));
	BC = sqrt(pow(this->mPt1.x - this->mPt3.x, 2) + pow(this->mPt1.y - this->mPt3.y, 2));
	a13 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 34
	AB = sqrt(pow(this->mPt3.x - pt.x, 2) + pow(this->mPt3.y - pt.y, 2));
	AC = sqrt(pow(this->mPt4.x - pt.x, 2) + pow(this->mPt4.y - pt.y, 2));
	BC = sqrt(pow(this->mPt3.x - this->mPt4.x, 2) + pow(this->mPt3.y - this->mPt4.y, 2));
	a34 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 24
	AB = sqrt(pow(this->mPt2.x - pt.x, 2) + pow(this->mPt2.y - pt.y, 2));
	AC = sqrt(pow(this->mPt4.x - pt.x, 2) + pow(this->mPt4.y - pt.y, 2));
	BC = sqrt(pow(this->mPt2.x - this->mPt4.x, 2) + pow(this->mPt2.y - this->mPt4.y, 2));
	a24 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 12
	AB = sqrt(pow(this->mPt1.x - pt.x, 2) + pow(this->mPt1.y - pt.y, 2));
	AC = sqrt(pow(this->mPt2.x - pt.x, 2) + pow(this->mPt2.y - pt.y, 2));
	BC = sqrt(pow(this->mPt1.x - this->mPt2.x, 2) + pow(this->mPt1.y - this->mPt2.y, 2));
	a12 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	return abs(a13 + a34 + a24 + a12 - 2 * CV_PI) < 1e-5;
}

bool RotRect2D::contain(double x, double y) const
{
	// 使用余弦定理计算，AB/AC为夹角边，BC为角对应的边
	// BC*BC = AB*AB + AC*AC - 2*AB*AC*cos(A)
	double AB, AC, BC;
	double a13, a34, a24, a12; // 四个角

	// 13
	AB = sqrt(pow(this->mPt1.x - x, 2) + pow(this->mPt1.y - y, 2));
	AC = sqrt(pow(this->mPt3.x - x, 2) + pow(this->mPt3.y - y, 2));
	BC = sqrt(pow(this->mPt1.x - this->mPt3.x, 2) + pow(this->mPt1.y - this->mPt3.y, 2));
	a13 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 34
	AB = sqrt(pow(this->mPt3.x - x, 2) + pow(this->mPt3.y - y, 2));
	AC = sqrt(pow(this->mPt4.x - x, 2) + pow(this->mPt4.y - y, 2));
	BC = sqrt(pow(this->mPt3.x - this->mPt4.x, 2) + pow(this->mPt3.y - this->mPt4.y, 2));
	a34 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 24
	AB = sqrt(pow(this->mPt2.x - x, 2) + pow(this->mPt2.y - y, 2));
	AC = sqrt(pow(this->mPt4.x - x, 2) + pow(this->mPt4.y - y, 2));
	BC = sqrt(pow(this->mPt2.x - this->mPt4.x, 2) + pow(this->mPt2.y - this->mPt4.y, 2));
	a24 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	// 12
	AB = sqrt(pow(this->mPt1.x - x, 2) + pow(this->mPt1.y - y, 2));
	AC = sqrt(pow(this->mPt2.x - x, 2) + pow(this->mPt2.y - y, 2));
	BC = sqrt(pow(this->mPt1.x - this->mPt2.x, 2) + pow(this->mPt1.y - this->mPt2.y, 2));
	a12 = acos((AB * AB + AC * AC - BC * BC) / (2 * AB * AC));

	return abs(a13 + a34 + a24 + a12 - 2 * CV_PI) < 1e-5;
}

cv::Vec4d RotRect2D::getCenterLineAbs() const
{
	cv::Point2d p1, p2;
	p1 = (this->mPt1 + this->mPt2) / 2;
	p2 = (this->mPt3 + this->mPt4) / 2;

	return cv::Vec4d((p1.x), (p1.y), (p2.x), (p2.y));
}

cv::Vec4d RotRect2D::getCenterLineRel() const
{
	cv::Rect2d out = this->getOuterRect();

	cv::Point2d p1, p2;
	p1 = (this->mPt1 + this->mPt2) / 2;
	p2 = (this->mPt3 + this->mPt4) / 2;

	p1.x -= out.x;
	p1.y -= out.y;
	p2.x -= out.x;
	p2.y -= out.y;

	return cv::Vec4d((p1.x), (p1.y), (p2.x), (p2.y));
}


void RotRect2D::getEdgePoints(std::vector<cv::Point>& points) const
{
	pushPoints(this->mPt1, this->mPt3, points);
	pushPoints(this->mPt3, this->mPt4, points);
	pushPoints(this->mPt4, this->mPt2, points);
	pushPoints(this->mPt2, this->mPt1, points);
}

cv::Vec4d RotRect2D::getInitLineAbs() const
{
	return this->mInitLine;
}

cv::Vec4d RotRect2D::getInitLineRel() const
{
	cv::Rect2d out = this->getOuterRect();

	return cv::Vec4d((this->mInitLine[0] - out.x), (this->mInitLine[1] - out.y),
		(this->mInitLine[2] - out.x), (this->mInitLine[3] - out.y));
}