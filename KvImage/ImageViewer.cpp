#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QWidget(parent)
{
	// 初始化成员变量
	this->mImgStartX = this->mImgStartY = this->mImgEndX = this->mImgEndY = 0;
	this->meX = this->msX = this->meY = this->msY = 0;
	this->isDrawLine = this->isDrawPoint = this->isDrawRectangle = this->isDrawRotateRectangle = false;

	this->mLabel = new QLabel(this);
	this->mLabel->setAlignment(Qt::AlignCenter);
}

void ImageViewer::showImage(const cv::Mat& img)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// 显示
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height())));

	qDebug() << "Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void ImageViewer::showImage(const cv::Mat& img, const cv::Rect& imgRect)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// 显示
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height()), imgRect));

	qDebug() << "Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void ImageViewer::openImage(const cv::Mat& img)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->showImage(this->mImg, this->mImgRect);

	// 清空绘图
	this->clearDrawing();
}

void ImageViewer::openImageWithPoints(const cv::Mat& img, const QList<cv::Point2d>& pts)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->mPts = pts.toVector().toStdVector();
	this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
}

void ImageViewer::openImageWithLines(const cv::Mat& img, const QList<cv::Vec4d>& lines)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->mLines = lines.toVector().toStdVector();
	this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
}

void ImageViewer::openImageWithRectangles(const cv::Mat& img, const QList<cv::Rect2d>& rects)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->mRects = rects.toVector().toStdVector();
	this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
}

void ImageViewer::openImageWithRotateRectangles(const cv::Mat& img, const QList<RotRect2D>& rotRects)
{
	if (!img.data)
	{
		qWarning() << "Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->mRotRects = rotRects.toVector().toStdVector();
	this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
}

void ImageViewer::openImage(QString imagePath)
{
	qDebug() << QString::fromLocal8Bit("打开图像 (%1)").arg(imagePath);
	this->mImg = cv::imread(imagePath.toLocal8Bit().toStdString());
	if (!this->mImg.data)
	{
		qWarning() << QString("Read image failed! [%1]")
			.arg(imagePath);
		return;
	}
	qDebug() << QString::fromLocal8Bit("图像尺寸：width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// 计算图片矩形
	this->mImgRect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));

	// 显示图像
	this->showImage(this->mImg, this->mImgRect);

	// 清空绘图
	this->clearDrawing();
}

bool ImageViewer::hasImage()
{
	return this->mImg.data != nullptr;
}

void ImageViewer::drawPoint(cv::Mat& img, const cv::Point2d& pt, const cv::Scalar& color, const double crossLength, const double lineWidth)
{
	if (!img.data || !cv::Rect(0, 0, img.cols, img.rows).contains(pt)) return;

	cv::line(img, cv::Point(pt.x - crossLength, pt.y), cv::Point(pt.x + crossLength, pt.y), color, lineWidth);
	cv::line(img, cv::Point(pt.x, pt.y - crossLength), cv::Point(pt.x, pt.y + crossLength), color, lineWidth);
}

void ImageViewer::drawLine(cv::Mat& img, const cv::Vec4d& line, const cv::Scalar& color, const double lineWidth)
{
	if (!img.data) return;

	// x1,y1,x2,y2
	cv::line(img, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, lineWidth);
}

void ImageViewer::drawRectangle(cv::Mat& img, const cv::Rect2d& rect, const cv::Scalar& color, const double lineWidth)
{
	if (!img.data) return;

	cv::rectangle(img, rect, color, lineWidth);
}

void ImageViewer::drawRotateRectangle(cv::Mat& img, const RotRect2D& rotRect, const cv::Scalar& color, const double lineWidth)
{
	if (!img.data) return;

	rotRect.drawEdgeLines(img, color, lineWidth);
}


void ImageViewer::showImageWithPoints(const cv::Mat& img, const cv::Rect& imageRect, const std::vector<cv::Point2d>& pts, const cv::Scalar& color)
{
	if (!img.data) return;

	if (pts.size() == 0)
	{
		this->showImage(img, imageRect);
		return;
	}

	cv::Mat frame = img.clone();
	cv::Rect rect(0, 0, frame.cols, frame.rows);
	for (std::vector<cv::Point2d>::const_iterator pt = pts.begin(); pt < pts.end(); pt++)
	{
		if (!rect.contains(*pt)) continue;
		this->drawPoint(frame, *pt, color);
	}

	this->showImage(frame, imageRect);
}

void ImageViewer::showImageWithLines(const cv::Mat& img, const cv::Rect& imageRect, const std::vector<cv::Vec4d>& lines, const cv::Scalar& lineColor)
{
	if (!img.data) return;

	if (lines.size() == 0)
	{
		this->showImage(img, imageRect);
		return;
	}

	cv::Mat frame = img.clone();
	cv::Rect rect(0, 0, frame.cols, frame.rows);
	for (std::vector<cv::Vec4d>::const_iterator l = lines.begin(); l < lines.end(); l++)
	{
		this->drawLine(frame, *l);
	}

	this->showImage(frame, imageRect);
}

void ImageViewer::showImageWithRectangles(const cv::Mat& img, const cv::Rect& imageRect, const std::vector<cv::Rect2d>& rects, const cv::Scalar& lineColor)
{
	if (!img.data) return;

	if (rects.size() == 0)
	{
		this->showImage(img, imageRect);
		return;
	}

	cv::Mat frame = img.clone();
	cv::Rect rect(0, 0, frame.cols, frame.rows);
	for (std::vector<cv::Rect2d>::const_iterator r = rects.begin(); r < rects.end(); r++)
	{
		this->drawRectangle(frame, *r);
	}

	this->showImage(frame, imageRect);
}

void ImageViewer::showImageWithRotateRectangles(const cv::Mat& img, const cv::Rect& imageRect, const std::vector<RotRect2D>& rotRects, const cv::Scalar& lineColor)
{
	if (!img.data) return;

	if (rotRects.size() == 0)
	{
		this->showImage(img, imageRect);
		return;
	}

	cv::Mat frame = img.clone();
	cv::Rect rect(0, 0, frame.cols, frame.rows);
	for (std::vector<RotRect2D>::const_iterator rotRect = rotRects.begin(); rotRect < rotRects.end(); rotRect++)
	{
		this->drawRotateRectangle(frame, *rotRect);
	}

	this->showImage(frame, imageRect);
}

void ImageViewer::clearDrawing()
{
	this->mPts.clear();
	this->mRects.clear();
	this->mLines.clear();
	this->mRotRects.clear();
}

bool ImageViewer::isDrawing()
{
	return this->isDrawLine || this->isDrawPoint || this->isDrawRectangle || this->isDrawRotateRectangle;
}
bool ImageViewer::isDrawingPoint()
{
	return this->isDrawPoint;
}
bool ImageViewer::isDrawingLine()
{
	return this->isDrawLine;
}
bool ImageViewer::isDrawingRectangle()
{
	return this->isDrawRectangle;
}
bool ImageViewer::isDrawingRotateRectangle()
{
	return this->isDrawRotateRectangle;
}

void ImageViewer::startDrawPoint()
{
	if (!this->isDrawing())
	{
		this->isDrawPoint = true;
		this->mPts.clear();
	}
}

void ImageViewer::stopDraw()
{
	this->isDrawPoint = false;
	this->isDrawLine = false;
	this->isDrawRectangle = false;
	this->isDrawRotateRectangle = false;

	this->mPts.clear();
	this->mLines.clear();
	this->mRects.clear();
	this->mRotRects.clear();
}

void ImageViewer::stopDrawPoint()
{
	this->isDrawPoint = false;
	this->showImage(this->mImg, this->mImgRect);
}

void ImageViewer::startDrawLine()
{
	if (!this->isDrawing())
	{
		this->isDrawLine = true;
		this->mLines.clear();
	}
}

void ImageViewer::stopDrawLine()
{
	this->isDrawLine = false;
	this->showImage(this->mImg, this->mImgRect);
}

void ImageViewer::startDrawRectangle()
{
	if (!this->isDrawing())
	{
		this->isDrawRectangle = true;
		this->mRects.clear();
	}
}

void ImageViewer::stopDrawRectangle()
{
	this->isDrawRectangle = false;
	this->showImage(this->mImg, this->mImgRect);
}

void ImageViewer::startDrawRotateRectangle()
{
	if (!this->isDrawing())
	{
		this->isDrawRotateRectangle = true;
		this->mRotRects.clear();
	}
}

void ImageViewer::stopDrawRotateRectangle()
{
	this->isDrawRotateRectangle = false;
	this->showImage(this->mImg, this->mImgRect);
}

bool ImageViewer::deletePoint(const int idx)
{
	if (idx >= this->mPts.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mPts.erase(this->mPts.begin() + idx);
	this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
	return true;
}

bool ImageViewer::deletePoint(const cv::Point2d& pt)
{
	std::vector<cv::Point2d>::iterator pos = std::find(this->mPts.begin(), this->mPts.end(), pt);
	if (pos == this->mPts.end())
	{
		return false;
	}
	this->mPts.erase(pos);
	this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
	return true;
}

bool ImageViewer::deleteLine(const int idx)
{
	if (idx >= this->mLines.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mLines.erase(this->mLines.begin() + idx);
	this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
	return true;
}

bool ImageViewer::deleteLine(const cv::Vec4d& line)
{
	std::vector<cv::Vec4d>::iterator pos = std::find(this->mLines.begin(), this->mLines.end(), line);
	if (pos == this->mLines.end())
	{
		return false;
	}
	this->mLines.erase(pos);
	this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
	return true;
}

bool ImageViewer::deleteRectangle(const int idx)
{
	if (idx >= this->mRects.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mRects.erase(this->mRects.begin() + idx);
	this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
	return true;
}

bool ImageViewer::deleteRectangle(const cv::Rect2d& rect)
{
	std::vector<cv::Rect2d>::iterator pos = std::find(this->mRects.begin(), this->mRects.end(), rect);
	if (pos == this->mRects.end())
	{
		return false;
	}
	this->mRects.erase(pos);
	this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
	return true;
}

bool ImageViewer::deleteRotateRectangle(const int idx)
{
	if (idx >= this->mRotRects.size())
	{
		qWarning() << "Out of range";
		return false;
	}
	this->mRotRects.erase(this->mRotRects.begin() + idx);
	this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
	return true;
}

bool ImageViewer::deleteRotateRectangle(const RotRect2D& rotRect)
{
	std::vector<RotRect2D>::iterator pos = std::find(this->mRotRects.begin(), this->mRotRects.end(), rotRect);
	if (pos == this->mRotRects.end())
	{
		return false;
	}
	this->mRotRects.erase(pos);
	this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
	return true;
}

void ImageViewer::zoomToPoint(const int idx, const double viewWidth, const double viewHeight)
{
	if (!this->mImg.data)
	{
		qWarning() << "Image is empty";
		return;
	}
	if (idx >= this->mPts.size())
	{
		qWarning() << "Out of range";
		return;
	}

	cv::Point2d pt = this->mPts[idx];
	cv::Rect2d viewRect(pt.x - viewWidth / 2, pt.y - viewHeight / 2, viewWidth, viewHeight);
	this->mImgRect = Calc2D::calcImageRectWithViewRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()), viewRect);
	this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
}

void ImageViewer::zoomToLine(const int idx, const double viewWidthExpand, const double viewHeightExpand)
{
	if (!this->mImg.data)
	{
		qWarning() << "Image is empty";
		return;
	}
	if (idx >= this->mLines.size())
	{
		qWarning() << "Out of range";
		return;
	}
	
	cv::Vec4d line = this->mLines[idx];
	double x1, y1, x2, y2;
	if (line[0] <= line[2])
	{
		x1 = line[0];
		x2 = line[2];
	}
	else
	{
		x1 = line[2];
		x2 = line[0];
	}
	if (line[1] <= line[3])
	{
		y1 = line[1];
		y2 = line[3];
	}
	else
	{
		y1 = line[3];
		y2 = line[1];
	}
	cv::Rect2d viewRect(x1 - viewWidthExpand / 2, y1 - viewHeightExpand / 2, 0, 0);
	viewRect.width = x2 + viewWidthExpand / 2 - viewRect.x;
	viewRect.height = y2 + viewHeightExpand / 2 - viewRect.y;
	this->mImgRect = Calc2D::calcImageRectWithViewRect(this->mImg,
		cv::Size2d(this->mLabel->width(), this->mLabel->height()), viewRect);
	this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
}

void ImageViewer::zoomToRectangle(const int idx, const double viewWidthExpand, const double viewHeightExpand)
{
	if (!this->mImg.data)
	{
		qWarning() << "Image is empty";
		return;
	}
	if (idx >= this->mRects.size())
	{
		qWarning() << "Out of range";
		return;
	}

	cv::Rect2d rect = this->mRects[idx];
	cv::Rect2d viewRect(rect.x - viewWidthExpand / 2, rect.y - viewHeightExpand / 2,
		rect.width + viewWidthExpand, rect.height + viewHeightExpand);
	this->mImgRect = Calc2D::calcImageRectWithViewRect(this->mImg,
		cv::Size2d(this->mLabel->width(), this->mLabel->height()), viewRect);
	this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
}

void ImageViewer::zoomToRotateRectangle(const int idx, const double viewWidthExpand, const double viewHeightExpand)
{
	if (!this->mImg.data)
	{
		qWarning() << "Image is empty";
		return;
	}
	if (idx >= this->mRotRects.size())
	{
		qWarning() << "Out of range";
		return;
	}

	cv::Rect2d outRect = this->mRotRects[idx].getOuterRect();
	cv::Rect2d viewRect(outRect.x - viewWidthExpand / 2, outRect.y - viewHeightExpand / 2,
		outRect.width + viewWidthExpand, outRect.height + viewHeightExpand);
	this->mImgRect = Calc2D::calcImageRectWithViewRect(this->mImg,
		cv::Size2d(this->mLabel->width(), this->mLabel->height()), viewRect);
	this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
}

void ImageViewer::resizeEvent(QResizeEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();

	QSize winSize = this->size();

	int labelHeight = winSize.height(),
		labelWidth = winSize.width();

	this->mLabel->setGeometry(0, 0, labelWidth, labelHeight);
	qDebug() << "Label size is (width=" << labelWidth
		<< "px, height=" << labelHeight << "px)";

	// 重新显示图片
	if (labelWidth > 0 && labelHeight > 0 && this->mImg.data)
	{
		if (this->isDrawPoint)
		{
			this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
		}
		else if (this->isDrawRectangle)
		{
			this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
		}
		else if (this->isDrawRotateRectangle)
		{
			this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
		}
		else
		{
			this->showImage(this->mImg, this->mImgRect);
		}
	}
}

void ImageViewer::wheelEvent(QWheelEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();
	if (this->mImg.data)
	{
		qDebug() << "angleDelta: " << evt->angleDelta().y();  // angleDelta:  QPoint(0,120)

		QPointF pt = evt->posF();    // 鼠标中心点
		double delta = 0.1;          // 缩放系数
		delta *= double(evt->angleDelta().y()) / 8 / 15;  // 向上滚为正，放大；向下滚为负，缩小

		int winHeight, winWidth;
		cv::Rect2d nRect;
		double imgMinZoomRatio = 0.5;         // 最小缩小到原来的几倍
		double imgMaxZoomPx = 100;            // 最大放大到多少像素
		double x0 = pt.x(),   // 鼠标窗口坐标
			y0 = pt.y(),
			x1 = double(this->mImgRect.x),  // 矩形左上角
			y1 = double(this->mImgRect.y);
		winHeight = this->mLabel->height();
		winWidth = this->mLabel->width();
		// 先计算目标区域图像矩形
		nRect.width = cvRound(double(this->mImgRect.width) * (1 + delta));
		nRect.height = cvRound(double(this->mImgRect.height) * (1 + delta));
		nRect.x = cvRound(x0 + (x1 - x0) * (1 + delta));
		nRect.y = cvRound(y0 + (y1 - y0) * (1 + delta));
		// 根据计算结果优化矩形
		// 1. 缩小过小时，长宽均小于比例时，按矩形与窗口对应边的比例较大的那个来。
		// 例如矩形长比窗口长为0.3，宽之比为0.5，那么以宽为主；否则反之。
		double imgRectWidthZoomRatio = double(nRect.width) / double(winWidth);
		double imgRectHeightZoomRatio = double(nRect.height) / double(winHeight);
		if (imgRectWidthZoomRatio < imgMinZoomRatio && imgRectHeightZoomRatio < imgMinZoomRatio)
		{
			if (imgRectWidthZoomRatio >= imgRectHeightZoomRatio)
			{
				delta = double(winWidth) * imgMinZoomRatio / double(this->mImgRect.width) - 1;
			}
			else delta = double(winHeight) * imgMinZoomRatio / double(this->mImgRect.height) - 1;

			nRect.width = cvRound(double(this->mImgRect.width) * (1 + delta));
			nRect.height = cvRound(double(this->mImgRect.height) * (1 + delta));
			nRect.x = winWidth / 2 - nRect.width / 2;
			nRect.y = winHeight / 2 - nRect.height / 2;
		}
		// 2. 放大过大时，显示区映射到图像上矩形长宽任意一个小于限定值时，需要进行拉伸，保证大于限定值
		// 例如，放大后显示区矩形映射到图像上宽度为90，高度为80，限定值为100，那么要求两个都大于100
		// 调整后显示区高度应该为100，宽度为112.5，保证都大于限定值
		double imgRectRawImageWidth = double(winWidth) / double(nRect.width) * double(this->mImg.cols);
		double imgRectRawImageHeight = double(winHeight) / double(nRect.height) * double(this->mImg.rows);
		if (imgRectRawImageWidth < imgMaxZoomPx || imgRectRawImageHeight < imgMaxZoomPx)
		{
			double zoomRatio;
			if (imgRectRawImageWidth <= imgRectRawImageHeight)
			{
				zoomRatio = imgRectRawImageWidth / imgMaxZoomPx;
			} else zoomRatio = imgRectRawImageHeight / imgMaxZoomPx;

			nRect.width = double(nRect.width) * zoomRatio;
			nRect.height = double(nRect.height) * zoomRatio;
			nRect.x = cvRound(x0 + (nRect.x - x0) * (zoomRatio));
			nRect.y = cvRound(y0 + (nRect.y - y0) * (zoomRatio));
		}

		if (this->mImgRect == nRect) return;
		this->mImgRect = nRect;

		{
			//// 不允许缩放过度
			//if (this->mRect.width <= this->mLabel->width() && delta < 0) return;  // 当显示图像要小于窗口时，不允许缩小
			//if (this->mRect.height <= this->mLabel->height() && delta < 0) return;
			//if (double(this->mLabel->width()) / double(this->mRect.width) * this->mImg.cols <= 100 && delta > 0) return;  // 当显示范围在图像上小于某一像素时，不允许放大
			//if (double(this->mLabel->height()) / double(this->mRect.height) * this->mImg.rows <= 100 && delta > 0) return;

			//// (x0-x1)/(x0-x2) = w1/w2 = 1/(1+delta)
			//double x0 = pt.x(),
			//	y0 = pt.y(),
			//	x1 = double(this->mRect.x),
			//	y1 = double(this->mRect.y);
			//this->mRect.width *= (1 + delta);
			//this->mRect.height *= (1 + delta);
			//this->mRect.x = x0 + (x1 - x0) * (1 + delta);
			//this->mRect.y = y0 + (y1 - y0) * (1 + delta);

			//// 起始点也要发生改变
			//this->msX = x0 + (this->msX - x0) * (1 + delta);
			//this->msY = y0 + (this->msY - y0) * (1 + delta);
		}

		qDebug() << QString::fromLocal8Bit("当前图像大小：width=%1, height=%2; 显示区占图比例: w=%3, h=%4")
			.arg(this->mImgRect.width)
			.arg(this->mImgRect.height)
			.arg(double(this->mLabel->width()) / double(this->mImgRect.width))
			.arg(double(this->mLabel->height()) / double(this->mImgRect.height));

		if (this->isDrawPoint)
		{
			// 如果鼠标左键按下，说明是在绘图时放大图像，则绘制当前点
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				this->drawPoint(frame, cv::Point2d(this->mImgEndX, this->mImgEndY));
				this->showImageWithPoints(frame, this->mImgRect, this->mPts);
			}
			else this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
				{
					this->drawLine(frame, cv::Vec4d(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY));
				}
				this->showImageWithLines(frame, this->mImgRect, this->mLines);
			}
			else this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
			
		}
		else if (this->isDrawRectangle)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
				{
					double x1, y1, x2, y2;
					if (this->mImgStartX <= this->mImgEndX)
					{
						x1 = this->mImgStartX;
						x2 = this->mImgEndX;
					}
					else
					{
						x1 = this->mImgEndX;
						x2 = this->mImgStartX;
					}
					if (this->mImgStartY <= this->mImgEndY)
					{
						y1 = this->mImgStartY;
						y2 = this->mImgEndY;
					}
					else
					{
						y1 = this->mImgEndY;
						y2 = this->mImgStartY;
					}
					this->drawRectangle(frame, cv::Rect2d(x1, y1, x2 - x1, y2 - y1));
				}
				this->showImageWithRectangles(frame, this->mImgRect, this->mRects);
			}
			else this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
		}
		else if (this->isDrawRotateRectangle)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();

				if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
				{
					RotRect2D rotRect(cv::Vec4d(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY));
					this->drawRotateRectangle(frame, rotRect);
				}
				this->showImageWithRotateRectangles(frame, this->mImgRect, this->mRotRects);
			}
			else this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
		}
		else this->showImage(this->mImg, this->mImgRect);
	}
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();

	QPointF pt = evt->localPos();
	qDebug() << QString("(x=%1, y=%2)")
		.arg(pt.x()).arg(pt.y());

	if (this->mImg.data)
	{
		cv::Rect2d rect = Calc2D::calcImageRect(this->mImg, cv::Size2d(this->mLabel->width(), this->mLabel->height()));
		if (this->mImgRect != rect)
		{
			this->mImgRect = rect;
			if (this->isDrawPoint)
			{
				this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
			}
			else if (this->isDrawLine)
			{
				this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
			}
			else if (this->isDrawRectangle)
			{
				this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
			}
			else if (this->isDrawRotateRectangle)
			{
				this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
			}
			else
			{
				this->showImage(this->mImg, this->mImgRect);
			}
		}
	}
}

void ImageViewer::mousePressEvent(QMouseEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();
	QPointF pt = evt->localPos();
	this->msX = this->meX = pt.x();
	this->msY = this->meY = pt.y();
	qDebug() << QString("(x=%1, y=%2)")
		.arg(this->msX).arg(this->msY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		px p;
		if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, this->mImgRect))
		{
			this->mImgStartX = this->mImgEndX = p.x;
			this->mImgStartY = this->mImgEndY = p.y;
			emit imageMouseMoveEvent(p);
			
			if (this->isDrawPoint)
			{
				cv::Mat frame = this->mImg.clone();
				this->drawPoint(frame, cv::Point2d(p.x, p.y));
				this->showImageWithPoints(frame, this->mImgRect, this->mPts);
			}
			else if (this->isDrawLine)
			{
				//
			}
			else if (this->isDrawRectangle)
			{
				//
			}
			else if (this->isDrawRotateRectangle)
			{
				//
			}
		}
	}
}

void ImageViewer::mouseMoveEvent(QMouseEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();

	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		// 如果在绘图时取不到像素点，则退出；一般情况下取不到像素点可能是在移动图像
		px p;
		if (!Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, this->mImgRect)
			&& this->isDrawing())
		{
			return;
		}

		this->mImgEndX = p.x;
		this->mImgEndY = p.y;
		emit imageMouseMoveEvent(p);

		// 绘图时窗口不变，因此不需要考虑图像矩形位置变化
		if (this->isDrawPoint)
		{
			cv::Mat frame = this->mImg.clone();
			this->drawPoint(frame, cv::Point2d(this->mImgEndX, this->mImgEndY));
			this->showImageWithPoints(frame, this->mImgRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			cv::Mat frame = this->mImg.clone();
			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				this->drawLine(frame, cv::Vec4d(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY));
			}
			this->showImageWithLines(frame, this->mImgRect, this->mLines);
		}
		else if (this->isDrawRectangle)
		{
			cv::Mat frame = this->mImg.clone();
			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				double x1, y1, x2, y2;
				if (this->mImgStartX <= this->mImgEndX)
				{
					x1 = this->mImgStartX;
					x2 = this->mImgEndX;
				}
				else
				{
					x1 = this->mImgEndX;
					x2 = this->mImgStartX;
				}
				if (this->mImgStartY <= this->mImgEndY)
				{
					y1 = this->mImgStartY;
					y2 = this->mImgEndY;
				}
				else
				{
					y1 = this->mImgEndY;
					y2 = this->mImgStartY;
				}
				this->drawRectangle(frame, cv::Rect2d(x1, y1, x2 - x1, y2 - y1));
			}
			this->showImageWithRectangles(frame, this->mImgRect, this->mRects);
			
		}
		else if (this->isDrawRotateRectangle)
		{
			cv::Mat frame = this->mImg.clone();

			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				RotRect2D rotRect(cv::Vec4d(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY));
				this->drawRotateRectangle(frame, rotRect);
			}
			this->showImageWithRotateRectangles(frame, this->mImgRect, this->mRotRects);
		}
		else
		{
			cv::Rect tRect = this->mImgRect;
			tRect.x += (this->meX - this->msX);
			tRect.y += (this->meY - this->msY);
			this->showImage(this->mImg, tRect);
		}
	}
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* evt)
{
	// 接收事件，不往下传递
	evt->accept();
	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();
	qDebug() << QString("(x=%1, y=%2)")
		.arg(this->meX).arg(this->meY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		px p;
		if (!Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, this->mImgRect))
		{
			return;
		}

		this->mImgEndX = p.x;
		this->mImgEndY = p.y;
		emit imageMouseMoveEvent(p);

		if (this->isDrawPoint)
		{
			cv::Point2d nPt(this->mImgEndX, this->mImgEndY);
			this->mPts.push_back(nPt);
			emit drawNewPoint(nPt);
			this->showImageWithPoints(this->mImg, this->mImgRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				cv::Vec4d line(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY);
				this->mLines.push_back(line);
				emit drawNewLine(line);
			}
			this->showImageWithLines(this->mImg, this->mImgRect, this->mLines);
		}
		else if (this->isDrawRectangle)
		{
			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				double x1, y1, x2, y2;
				if (this->mImgStartX <= this->mImgEndX)
				{
					x1 = this->mImgStartX;
					x2 = this->mImgEndX;
				}
				else
				{
					x1 = this->mImgEndX;
					x2 = this->mImgStartX;
				}
				if (this->mImgStartY <= this->mImgEndY)
				{
					y1 = this->mImgStartY;
					y2 = this->mImgEndY;
				}
				else
				{
					y1 = this->mImgEndY;
					y2 = this->mImgStartY;
				}
				cv::Rect2d nRect(x1, y1, x2 - x1, y2 - y1);
				this->mRects.push_back(nRect);
				emit drawNewRectangle(nRect);
			}
			
			this->showImageWithRectangles(this->mImg, this->mImgRect, this->mRects);
		}
		else if (this->isDrawRotateRectangle)
		{
			if (abs(this->mImgStartX - this->mImgEndX) > 1e-5 || abs(this->mImgStartY - this->mImgEndY) > 1e-5)
			{
				RotRect2D nRotRect(cv::Vec4d(this->mImgStartX, this->mImgStartY, this->mImgEndX, this->mImgEndY));
				this->mRotRects.push_back(nRotRect);
				emit drawNewRotateRectangle(nRotRect);
			}
			this->showImageWithRotateRectangles(this->mImg, this->mImgRect, this->mRotRects);
		}
		else
		{
			if (this->msX != this->meX || this->msY != this->meY)
			{
				this->mImgRect.x += (this->meX - this->msX);
				this->mImgRect.y += (this->meY - this->msY);
				this->showImage(this->mImg, this->mImgRect);
			}
		}
	}
}