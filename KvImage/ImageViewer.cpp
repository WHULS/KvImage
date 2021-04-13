#include "ImageViewer.h"

ImageViewer::ImageViewer(QWidget* parent)
	: QWidget(parent)
{
	// ��ʼ����Ա����
	this->meX = this->msX = this->meY = this->msY = 0;

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

void ImageViewer::showImage(const cv::Mat& img, cv::Rect imgRect)
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
	this->mRect = Transform::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));

	// ��ʾͼ��
	this->showImage(this->mImg, this->mRect);
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
	this->mRect = Transform::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));

	// ��ʾͼ��
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
		this->showImage(this->mImg, this->mRect);
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
		delta *= double(evt->angleDelta().y()) / 8 / 15;

		// (x0-x1)/(x0-x2) = w1/w2 = 1/(1+delta)
		double x0 = pt.x(),
			y0 = pt.y(),
			x1 = double(this->mRect.x),
			y1 = double(this->mRect.y);
		this->mRect.width *= (1 + delta);
		this->mRect.height *= (1 + delta);
		this->mRect.x = x0 + (x1 - x0) * (1 + delta);
		this->mRect.y = y0 + (y1 - y0) * (1 + delta);

		qDebug() << QString::fromLocal8Bit("��ǰͼ���С��width=%1, height=%2; ��ʾ��ռͼ����: w=%3, h=%4")
			.arg(this->mRect.width)
			.arg(this->mRect.height)
			.arg(double(this->mLabel->width()) / double(this->mRect.width))
			.arg(double(this->mLabel->height()) / double(this->mRect.height));

		this->showImage(this->mImg, this->mRect);
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
		this->mRect = Transform::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));
		this->showImage(this->mImg, this->mRect);
	}
}

void ImageViewer::mousePressEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->msX = pt.x();
	this->msY = pt.y();
	qDebug() << QString("ImageViewer::mousePressEvent() - (x=%1, y=%2)")
		.arg(this->msX).arg(this->msY);
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
		cv::Rect tRect = this->mRect;
		tRect.x += (this->meX - this->msX);
		tRect.y += (this->meY - this->msY);
		this->showImage(this->mImg, tRect);

		cv::Point iPt(this->meX, this->meY);
		if (tRect.contains(iPt))
		{
			px p;
			p.x = double(iPt.x - tRect.x)* (double(this->mImg.cols) / double(tRect.width));
			p.y = double(iPt.y - tRect.y) * (double(this->mImg.rows) / double(tRect.height));
			p.channels = this->mImg.type() == CV_8UC3 ? 3 : 1;
			if (p.channels == 3)
			{
				cv::Vec3b color = this->mImg.at<cv::Vec3b>(int(p.y), int(p.x));
				p.r = color[2];
				p.g = color[1];
				p.b = color[0];
			}
			else
			{
				p.r = this->mImg.at<unsigned char>(int(p.y), int(p.x));
			}

			emit imageMouseMoveEvent(p);
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
		this->mRect.x += (this->meX - this->msX);
		this->mRect.y += (this->meY - this->msY);
		this->showImage(this->mImg, this->mRect);
	}
}
