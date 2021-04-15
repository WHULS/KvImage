#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QWidget(parent)
{
	// ��ʼ����Ա����
	this->meX = this->msX = this->meY = this->msY = 0;
	this->isDrawLine = this->isDrawPoint = this->isDrawRectangle = this->isDrawRotateRectangle = false;

	this->mLabel = new QLabel(this);
	this->mLabel->setAlignment(Qt::AlignCenter);
}

void ImageViewer::showImage(const cv::Mat& img)
{
	if (!img.data)
	{
		qWarning() << "ImageViewer::showImage() - Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// ��ʾ
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height())));

	qDebug() << "ImageViewer::showImage() - Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void ImageViewer::showImage(const cv::Mat& img, const cv::Rect& imgRect)
{
	if (!img.data)
	{
		qWarning() << "ImageViewer::showImage() - Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// ��ʾ
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height()), imgRect));

	qDebug() << "ImageViewer::showImage() - Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void ImageViewer::openImage(const cv::Mat& img)
{
	if (!img.data)
	{
		qWarning() << "ImageViewer::openImage - Mat is Empty";
		return;
	}
	this->mImg = img.clone();
	qDebug() << QString::fromLocal8Bit("ImageViewer::openImage - ͼ��ߴ磺width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// ����ͼƬ����
	this->mRect = Calc2D::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));

	// ��ʾͼ��
	this->showImage(this->mImg, this->mRect);

	// ��ջ�ͼ
	this->clearDrawing();
}

void ImageViewer::openImage(QString imagePath)
{
	qDebug() << QString::fromLocal8Bit("ImageViewer::openImage - ��ͼ�� (%1)").arg(imagePath);
	this->mImg = cv::imread(imagePath.toLocal8Bit().toStdString());
	if (!this->mImg.data)
	{
		qWarning() << QString("ImageViewer::openImage - Read image failed! [%1]")
			.arg(imagePath);
		return;
	}
	qDebug() << QString::fromLocal8Bit("ImageViewer::openImage - ͼ��ߴ磺width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// ����ͼƬ����
	this->mRect = Calc2D::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));

	// ��ʾͼ��
	this->showImage(this->mImg, this->mRect);

	// ��ջ�ͼ
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

void ImageViewer::stopDrawPoint()
{
	this->isDrawPoint = false;
	this->showImage(this->mImg, this->mRect);
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
	this->showImage(this->mImg, this->mRect);
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
	this->showImage(this->mImg, this->mRect);
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
	this->showImage(this->mImg, this->mRect);
}

void ImageViewer::resizeEvent(QResizeEvent* evt)
{
	// �����¼��������´���
	evt->accept();

	QSize winSize = this->size();

	int labelHeight = winSize.height(),
		labelWidth = winSize.width();

	this->mLabel->setGeometry(0, 0, labelWidth, labelHeight);
	qDebug() << "ImageViewer::resizeEvent() - Label size is (width=" << labelWidth
		<< "px, height=" << labelHeight << "px)";

	// ������ʾͼƬ
	if (labelWidth > 0 && labelHeight > 0 && this->mImg.data)
	{
		if (this->isDrawPoint)
		{
			this->showImageWithPoints(this->mImg, this->mRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			this->showImageWithLines(this->mImg, this->mRect, this->mLines);
		}
		else if (this->isDrawRectangle)
		{
			this->showImageWithRectangles(this->mImg, this->mRect, this->mRects);
		}
		else if (this->isDrawRotateRectangle)
		{
			this->showImageWithRotateRectangles(this->mImg, this->mRect, this->mRotRects);
		}
		else
		{
			this->showImage(this->mImg, this->mRect);
		}
	}
}

void ImageViewer::wheelEvent(QWheelEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	if (this->mImg.data)
	{
		qDebug() << "angleDelta: " << evt->angleDelta().y();  // angleDelta:  QPoint(0,120)

		QPointF pt = evt->posF();    // ������ĵ�
		double delta = 0.1;          // ����ϵ��
		delta *= double(evt->angleDelta().y()) / 8 / 15;  // ���Ϲ�Ϊ�����Ŵ����¹�Ϊ������С

		// ��ͼʱ�����������Ź���
		if (this->isDrawing())
		{
			if (this->mRect.width <= this->mLabel->width() && delta < 0) return;  // ����ʾͼ��ҪС�ڴ���ʱ����������С
			if (this->mRect.height <= this->mLabel->height() && delta < 0) return;
			if (double(this->mLabel->width()) / double(this->mRect.width) * this->mImg.cols <= 100 && delta > 0) return;  // ����ʾ��Χ��ͼ����С��100ʱ��������Ŵ�
			if (double(this->mLabel->height()) / double(this->mRect.height) * this->mImg.rows <= 100 && delta > 0) return;
		}

		// (x0-x1)/(x0-x2) = w1/w2 = 1/(1+delta)
		double x0 = pt.x(),
			y0 = pt.y(),
			x1 = double(this->mRect.x),
			y1 = double(this->mRect.y);
		this->mRect.width *= (1 + delta);
		this->mRect.height *= (1 + delta);
		this->mRect.x = x0 + (x1 - x0) * (1 + delta);
		this->mRect.y = y0 + (y1 - y0) * (1 + delta);

		// ��ʼ��ҲҪ�����ı�
		this->msX = x0 + (this->msX - x0) * (1 + delta);
		this->msY = y0 + (this->msY - y0) * (1 + delta);

		qDebug() << QString::fromLocal8Bit("��ǰͼ���С��width=%1, height=%2; ��ʾ��ռͼ����: w=%3, h=%4")
			.arg(this->mRect.width)
			.arg(this->mRect.height)
			.arg(double(this->mLabel->width()) / double(this->mRect.width))
			.arg(double(this->mLabel->height()) / double(this->mRect.height));

		if (this->isDrawPoint)
		{
			// ������������£�˵�����ڻ�ͼʱ�Ŵ�ͼ������Ƶ�ǰ��
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				px p;
				if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), frame, this->mRect))
				{
					this->drawPoint(frame, cv::Point2d(p.x, p.y));
				}
				this->showImageWithPoints(frame, this->mRect, this->mPts);
			}
			else
			{
				this->showImageWithPoints(this->mImg, this->mRect, this->mPts);
			}
		}
		else if (this->isDrawLine)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				px p1, p2;
				if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
					&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
				{
					this->drawLine(frame, cv::Vec4d(p1.x, p1.y, p2.x, p2.y));
				}
				this->showImageWithLines(frame, this->mRect, this->mLines);
			}
			else
			{
				this->showImageWithLines(this->mImg, this->mRect, this->mLines);
			}
		}
		else if (this->isDrawRectangle)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				px p1, p2;
				if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
					&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
				{
					double x1, y1, x2, y2;
					if (p1.x <= p2.x)
					{
						x1 = p1.x;
						x2 = p2.x;
					}
					else
					{
						x1 = p2.x;
						x2 = p1.x;
					}
					if (p1.y <= p2.y)
					{
						y1 = p1.y;
						y2 = p2.y;
					}
					else
					{
						y1 = p2.y;
						y2 = p1.y;
					}
					this->drawRectangle(frame, cv::Rect2d(x1, y1, x2 - x1, y2 - y1));
				}
				this->showImageWithRectangles(frame, this->mRect, this->mRects);
			}
			else
			{
				this->showImageWithRectangles(this->mImg, this->mRect, this->mRects);
			}
		}
		else if (this->isDrawRotateRectangle)
		{
			if (evt->buttons() & Qt::LeftButton)
			{
				cv::Mat frame = this->mImg.clone();
				px p1, p2;
				if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
					&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
				{
					this->drawRotateRectangle(frame, RotRect2D(cv::Vec4d(p1.x, p1.y, p2.x, p2.y)));
				}
				this->showImageWithRotateRectangles(frame, this->mRect, this->mRotRects);
			}
			else
			{
				this->showImageWithRotateRectangles(this->mImg, this->mRect, this->mRotRects);
			}
		}
		else
		{
			this->showImage(this->mImg, this->mRect);
		}
	}
}

void ImageViewer::mouseDoubleClickEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();

	QPointF pt = evt->localPos();
	qDebug() << QString("ImageViewer::mouseDoubleClickEvent() - (x=%1, y=%2)")
		.arg(pt.x()).arg(pt.y());

	if (this->mImg.data)
	{
		cv::Rect rect = Calc2D::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));
		if (this->mRect != rect)
		{
			this->mRect = rect;
			if (this->isDrawPoint)
			{
				this->showImageWithPoints(this->mImg, this->mRect, this->mPts);
			}
			else if (this->isDrawLine)
			{
				this->showImageWithLines(this->mImg, this->mRect, this->mLines);
			}
			else if (this->isDrawRectangle)
			{
				this->showImageWithRectangles(this->mImg, this->mRect, this->mRects);
			}
			else if (this->isDrawRotateRectangle)
			{
				this->showImageWithRotateRectangles(this->mImg, this->mRect, this->mRotRects);
			}
			else
			{
				this->showImage(this->mImg, this->mRect);
			}
		}
	}
}

void ImageViewer::mousePressEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->msX = this->meX = pt.x();
	this->msY = this->meY = pt.y();
	qDebug() << QString("ImageViewer::mousePressEvent() - (x=%1, y=%2)")
		.arg(this->msX).arg(this->msY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		px p;
		if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, this->mRect))
		{
			emit imageMouseMoveEvent(p);
			
			if (this->isDrawPoint)
			{
				cv::Mat frame = this->mImg.clone();
				this->drawPoint(frame, cv::Point2d(p.x, p.y));
				this->showImageWithPoints(frame, this->mRect, this->mPts);
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
	// �����¼��������´���
	evt->accept();

	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		// ��ͼʱ���ڲ��䣬��˲���Ҫ����ͼ�����λ�ñ仯
		if (this->isDrawPoint)
		{
			cv::Mat frame = this->mImg.clone();
			px p;
			if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), frame, this->mRect))
			{
				this->drawPoint(frame, cv::Point2d(p.x, p.y));
				this->showImageWithPoints(frame, this->mRect, this->mPts);
				emit imageMouseMoveEvent(p);
			}
		}
		else if (this->isDrawLine)
		{
			cv::Mat frame = this->mImg.clone();
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
			{
				this->drawLine(frame, cv::Vec4d(p1.x, p1.y, p2.x, p2.y));
				this->showImageWithLines(frame, this->mRect, this->mLines);
				emit imageMouseMoveEvent(p2);
			}
		}
		else if (this->isDrawRectangle)
		{
			cv::Mat frame = this->mImg.clone();
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
			{
				double x1, y1, x2, y2;
				if (p1.x <= p2.x)
				{
					x1 = p1.x;
					x2 = p2.x;
				}
				else
				{
					x1 = p2.x;
					x2 = p1.x;
				}
				if (p1.y <= p2.y)
				{
					y1 = p1.y;
					y2 = p2.y;
				}
				else
				{
					y1 = p2.y;
					y2 = p1.y;
				}
				this->drawRectangle(frame, cv::Rect2d(x1, y1, x2 - x1, y2 - y1));
				this->showImageWithRectangles(frame, this->mRect, this->mRects);
				emit imageMouseMoveEvent(p2);
			}
		}
		else if (this->isDrawRotateRectangle)
		{
			cv::Mat frame = this->mImg.clone();
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), frame, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), frame, this->mRect))
			{
				RotRect2D rotRect(cv::Vec4d(p1.x, p1.y, p2.x, p2.y));
				this->drawRotateRectangle(frame, rotRect);
				this->showImageWithRotateRectangles(frame, this->mRect, this->mRotRects);
				emit imageMouseMoveEvent(p2);
			}
		}
		else
		{
			cv::Rect tRect = this->mRect;
			tRect.x += (this->meX - this->msX);
			tRect.y += (this->meY - this->msY);
			px p;
			if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, tRect))
			{
				emit imageMouseMoveEvent(p);
			}
			this->showImage(this->mImg, tRect);
		}
	}
}

void ImageViewer::mouseReleaseEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();
	qDebug() << QString("ImageViewer::mouseReleaseEvent() - (x=%1, y=%2)")
		.arg(this->meX).arg(this->meY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{

		if (this->isDrawPoint)
		{
			px p;
			if (Calc2D::getPxFromImage(p, cv::Point(this->meX, this->meY), this->mImg, this->mRect))
			{
				emit imageMouseMoveEvent(p);

				this->mPts.push_back(cv::Point2d(p.x, p.y));
			}
			this->showImageWithPoints(this->mImg, this->mRect, this->mPts);
		}
		else if (this->isDrawLine)
		{
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), this->mImg, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), this->mImg, this->mRect))
			{
				emit imageMouseMoveEvent(p2);

				if (abs(p1.x - p2.x) > 1e-5 || abs(p1.y - p2.y) > 1e-5)
				{
					this->mLines.push_back(cv::Vec4d(p1.x, p1.y, p2.x, p2.y));
				}
			}
			this->showImageWithLines(this->mImg, this->mRect, this->mLines);
		}
		else if (this->isDrawRectangle)
		{
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), this->mImg, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), this->mImg, this->mRect))
			{
				emit imageMouseMoveEvent(p2);

				if (abs(p1.x - p2.x) > 1e-5 || abs(p1.y - p2.y) > 1e-5)
				{
					double x1, y1, x2, y2;
					if (p1.x <= p2.x)
					{
						x1 = p1.x;
						x2 = p2.x;
					}
					else
					{
						x1 = p2.x;
						x2 = p1.x;
					}
					if (p1.y <= p2.y)
					{
						y1 = p1.y;
						y2 = p2.y;
					}
					else
					{
						y1 = p2.y;
						y2 = p1.y;
					}
					this->mRects.push_back(cv::Rect2d(x1, y1, x2 - x1, y2 - y1));
				}
			}
			this->showImageWithRectangles(this->mImg, this->mRect, this->mRects);
		}
		else if (this->isDrawRotateRectangle)
		{
			px p1, p2;
			if (Calc2D::getPxFromImage(p1, cv::Point(this->msX, this->msY), this->mImg, this->mRect)
				&& Calc2D::getPxFromImage(p2, cv::Point(this->meX, this->meY), this->mImg, this->mRect))
			{
				emit imageMouseMoveEvent(p2);

				if (abs(p1.x - p2.x) > 1e-5 || abs(p1.y - p2.y) > 1e-5)
				{
					this->mRotRects.push_back(RotRect2D(cv::Vec4d(p1.x, p1.y, p2.x, p2.y)));
				}
			}
			this->showImageWithRotateRectangles(this->mImg, this->mRect, this->mRotRects);
		}
		else
		{
			if (this->msX != this->meX || this->msY != this->meY)
			{
				this->mRect.x += (this->meX - this->msX);
				this->mRect.y += (this->meY - this->msY);
				this->showImage(this->mImg, this->mRect);
			}
		}
	}
}