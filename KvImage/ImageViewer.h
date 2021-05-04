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
#include <QList>

class ImageViewer :
    public QWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget* parent = Q_NULLPTR);
	void showImage(const cv::Mat& img);
	void showImage(const cv::Mat& img, const cv::Rect& imgRect);
	/*显示带有矢量图形的图片，适量坐标均为图像坐标，绘制后调用showImage()*/
	void showImageWithPoints(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Point2d>& pts, const cv::Scalar& pointColor = cv::Scalar(255, 0, 0));
	void showImageWithLines(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Vec4d>& lines, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void showImageWithRectangles(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<cv::Rect2d>& rects, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void showImageWithRotateRectangles(const cv::Mat& img, const cv::Rect& imgRect, const std::vector<RotRect2D>& rotRects, const cv::Scalar& lineColor = cv::Scalar(255, 0, 0));
	void openImage(const cv::Mat& img);
	void openImageWithPoints(const cv::Mat& img, const QList<cv::Point2d>& pts);
	void openImageWithLines(const cv::Mat& img, const QList<cv::Vec4d>& lines);
	void openImageWithRectangles(const cv::Mat& img, const QList<cv::Rect2d>& rects);
	void openImageWithRotateRectangles(const cv::Mat& img, const QList<RotRect2D>& rotRects);
	void openImage(QString imagePath);
	bool hasImage();
	/*在img上绘制各种图形*/
	static void drawPoint(cv::Mat& img, const cv::Point2d& pt, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double crossLength = 10.0, const double lineWidth = 1.0);
	static void drawLine(cv::Mat& img, const cv::Vec4d& line, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);
	static void drawRectangle(cv::Mat& img, const cv::Rect2d& rect, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);
	static void drawRotateRectangle(cv::Mat& img, const RotRect2D& rotRect, const cv::Scalar& color = cv::Scalar(255, 0, 0), const double lineWidth = 1.0);

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
	void stopDraw();

	// 删除矢量
	bool deletePoint(const int idx);
	bool deletePoint(const cv::Point2d& pt);
	bool deleteLine(const int idx);
	bool deleteLine(const cv::Vec4d& line);
	bool deleteRectangle(const int idx);
	bool deleteRectangle(const cv::Rect2d& rect);
	bool deleteRotateRectangle(const int idx);
	bool deleteRotateRectangle(const RotRect2D& rotRect);

	// 放大到图形
	void zoomToPoint(const int idx, const double viewWidth = 200, const double viewHeight = 200);
	void zoomToLine(const int idx, const double viewWidthExpand = 200, const double viewHeightExpand = 200);
	void zoomToRectangle(const int idx, const double viewWidthExpand = 200, const double viewHeightExpand = 200);
	void zoomToRotateRectangle(const int idx, const double viewWidthExpand = 200, const double viewHeightExpand = 200);

signals:
	void imageMouseMoveEvent(px p);
	void drawNewPoint(const cv::Point2d& pt);
	void drawNewLine(const cv::Vec4d& line);
	void drawNewRectangle(const cv::Rect2d& rect);
	void drawNewRotateRectangle(const RotRect2D& rotRect);

protected:
	void resizeEvent(QResizeEvent* evt);
	void wheelEvent(QWheelEvent* evt);
	void mouseDoubleClickEvent(QMouseEvent* evt);
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void mouseReleaseEvent(QMouseEvent* evt);

private:
	QLabel* mLabel = Q_NULLPTR;
	cv::Mat mImg;
	cv::Rect2d mImgRect;

	// mouse start x,y; mouse end x,y
	double msX, msY, meX, meY;
	double mImgStartX, mImgStartY, mImgEndX, mImgEndY;  // 用于记录起止点在图像上的坐标

	bool isDrawPoint,
		isDrawLine,
		isDrawRectangle,
		isDrawRotateRectangle;

	std::vector<cv::Point2d> mPts;
	std::vector<cv::Vec4d> mLines;
	std::vector<cv::Rect2d> mRects;
	std::vector<RotRect2D> mRotRects;
};

