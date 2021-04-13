#pragma once
#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include "Transform.h"
#include "px.h"

class ImageViewer :
    public QWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);
	void showImage(const cv::Mat& img);
	void showImage(const cv::Mat& img, cv::Rect imgRect);
	void openImage(const cv::Mat& img);
	void openImage(QString imagePath);

signals:
	void imageMouseMoveEvent(px p);

protected:
	void resizeEvent(QResizeEvent* evt);
	void wheelEvent(QWheelEvent* evt);
	void mouseDoubleClickEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);

private:
	QLabel* mLabel;
	cv::Mat mImg;
	cv::Rect mRect;

	// mouse start x,y; mouse end x,y
	double msX, msY, meX, meY;
};

