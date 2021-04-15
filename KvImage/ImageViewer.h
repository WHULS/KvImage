#pragma once
#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QLabel>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include "Transform.h"
#include "px.h"
#include "Calc2D.h"
#include "RotRect2D.h"

class ImageViewer :
    public QWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);
	void showImage(const cv::Mat& img);
	void showImage(const cv::Mat& img, const cv::Rect& imgRect);
	/*绘制点，pts坐标是图像坐标，绘制后调用this->showImage()*/
	void showImageWithPoints(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Point2d>& pts, const cv::Scalar& pointColor = cv::Scalar(255, 0, 0));
	void showImageWithLines(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Vec4d>& lines, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void showImageWithRectangles(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Rect2d>& rects, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void showImageWithRotateRectangles(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<RotRect2D>& rotRects, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void openImage(const cv::Mat& img);
	void openImage(QString imagePath);
	bool hasImage();
	/*在img上绘制十字交叉点*/
	void drawPoint(cv::Mat& img, const cv::Point2d& pt, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double crossLength = 10.0, const double lineWidth = 1.0);
	void drawLine(cv::Mat& img, const cv::Vec4d& line, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);
	void drawRectangle(cv::Mat& img, const cv::Rect2d& rect, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);
	void drawRotateRectangle(cv::Mat& img, const RotRect2D& rotRect, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);

	// 绘图相关
	void clearDrawing();
	bool isDrawing();
	bool isDrawingPoint();
	bool isDrawingLine();
	bool isDrawingRectangle();
	bool isDrawingRotateRectangle();

	void startDrawPoint();
	void stopDrawPoint();
	void startDrawLine();
	void stopDrawLine();
	void startDrawRectangle();
	void stopDrawRectangle();
	void startDrawRotateRectangle();
	void stopDrawRotateRectangle();

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

	bool isDrawPoint,
		isDrawLine,
		isDrawRectangle,
		isDrawRotateRectangle;

	std::vector<cv::Point2d> mPts;
	std::vector<cv::Vec4d> mLines;
	std::vector<cv::Rect2d> mRects;
	std::vector<RotRect2D> mRotRects;
};

