#include "KvImage.h"

KvImage::KvImage(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// ��ʼ����Ա����
	this->meX = this->msX = this->meY = this->meX = 0;

	QString mainLabelName("main_label");
	this->mLabel = ui.centralWidget->findChild<QLabel*>(mainLabelName);
	if (!this->mLabel)
	{
		qFatal(QString("QLabel [%1] not found!").arg(mainLabelName).toLocal8Bit());
	}
	this->mLabel->setAlignment(Qt::AlignCenter);
}


void KvImage::on_action_open_image_triggered()
{
	// ͨ���Ի����ȡ�ļ�·��
	QString caption = QString::fromLocal8Bit("ѡ��һ���ļ���");
	QString dir = "";  // Ϊ��Ĭ�ϼ�����һ�δ򿪵�·��
	QString filter("Image (*.png *.jpg *.jpeg *.tif *.bmp)");
	QString fileName = QFileDialog::getOpenFileName(
		this, caption, dir, filter
	);

	if (fileName.isEmpty())
	{
		qWarning() << QString::fromLocal8Bit("KvImage::on_action_open_image_triggered - δѡ��ͼƬ·��");
		return;
	}

	qInfo() << QString::fromLocal8Bit("��ͼ�� (%1)").arg(fileName);
	this->mImg = cv::imread(fileName.toLocal8Bit().toStdString());
	qInfo() << QString::fromLocal8Bit("ͼ��ߴ磺width=%1px, height=%2px")
		.arg(this->mImg.cols)
		.arg(this->mImg.rows);

	// ����ͼƬ����
	this->mRect = Transform::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));

	// ��ʾͼ��
	this->showImage(this->mImg, this->mRect);
}

void KvImage::showImage(const cv::Mat& img)
{
	if (!img.data)
	{
		qWarning() << "KvImage::showImage() - Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// ��ʾ
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height())));

	qDebug() << "KvImage::showImage() - Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void KvImage::showImage(const cv::Mat& img, cv::Rect imgRect)
{
	if (!img.data)
	{
		qWarning() << "KvImage::showImage() - Mat is Empty!";
		return;
	}
	time_t t = cv::getTickCount();

	// ��ʾ
	this->mLabel->setPixmap(Transform::MatToQPixmap(img,
		cv::Size(this->mLabel->width(), this->mLabel->height()), imgRect));

	qDebug() << "KvImage::showImage() - Cost time: "
		<< (double(cv::getTickCount() - t) / cv::getTickFrequency()) << "s";
}

void KvImage::resizeEvent(QResizeEvent* evt)
{
	// �����¼��������´���
	evt->accept();

	QSize winSize = this->size(),
		menuSize = ui.menuBar->size(),
		statusSize = ui.statusBar->size();

	int labelHeight = winSize.height() - menuSize.height() - statusSize.height(),
		labelWidth = winSize.width();

	this->mLabel->setGeometry(0, 0, labelWidth, labelHeight);
	qDebug() << "KvImage::resizeEvent() - Label size is (width=" << labelWidth
		<< "px, height=" << labelHeight << "px)";

	// ������ʾͼƬ
	if (labelWidth > 0 && labelHeight > 0 && this->mImg.data)
	{
		this->showImage(this->mImg, this->mRect);
	}
}

void KvImage::wheelEvent(QWheelEvent* evt)
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

void KvImage::mouseDoubleClickEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	qDebug() << QString("KvImage::mouseDoubleClickEvent() - (x=%1, y=%2)")
		.arg(pt.x()).arg(pt.y());

	if (this->mImg.data)
	{
		this->mRect = Transform::calcImageRect(this->mImg, cv::Size(this->mLabel->width(), this->mLabel->height()));
		this->showImage(this->mImg, this->mRect);
	}
}

void KvImage::mousePressEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->msX = pt.x();
	this->msY = pt.y();
	qDebug() << QString("KvImage::mousePressEvent() - (x=%1, y=%2)")
		.arg(this->msX).arg(this->msY);
}

void KvImage::mouseMoveEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();
	qDebug() << QString("KvImage::mouseMoveEvent() - (x=%1, y=%2)")
		.arg(this->meX).arg(this->meY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		cv::Rect tRect = this->mRect;
		tRect.x += (this->meX - this->msX);
		tRect.y += (this->meY - this->msY);
		this->showImage(this->mImg, tRect);
	}
}

void KvImage::mouseReleaseEvent(QMouseEvent* evt)
{
	// �����¼��������´���
	evt->accept();
	QPointF pt = evt->localPos();
	this->meX = pt.x();
	this->meY = pt.y();
	qDebug() << QString("KvImage::mouseReleaseEvent() - (x=%1, y=%2)")
		.arg(this->meX).arg(this->meY);

	if (this->mImg.data && this->mLabel->height() > 0 && this->mLabel->width() > 0)
	{
		this->mRect.x += (this->meX - this->msX);
		this->mRect.y += (this->meY - this->msY);
		this->showImage(this->mImg, this->mRect);
	}
}
