#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include "iPoint.h"


class RotRect2D
{
public:
    /**
    * @param lengthExpand: 与直线平行方向的区域扩展（默认为0）
    * @param widthExpand: 与直线垂直方向的区域扩展（默认为10）
    */
    RotRect2D(cv::Vec4d centerLine, double lengthExpand = 0.0, double widthExpand = 10.0);
    RotRect2D(cv::Point2d pt1, cv::Point2d pt2, cv::Point2d pt3, cv::Point2d pt4);
    RotRect2D();
    ~RotRect2D() {}

	/// 获取参数相关
    double getRotateAngle() const;  // 获取矩形旋转角度（图像坐标系）
	cv::Point2d getRectCenter() const;  // 获取矩形中心
	cv::Size2d getRectSize() const;  // 获取矩形大小
    std::vector<cv::Vec4d> getEdgeLines() const;  // 获取边缘直线
	void getCenterPoints(std::vector<cv::Point>& points) const;  // 获取中心线的点
	void getEdgePoints(std::vector<cv::Point>& points) const;  // 获取边缘点（所有）
    cv::Rect2d getOuterRect() const;  // 获取外接矩形（与坐标轴平行）
	cv::Mat getMask(const cv::Rect& refMatRect) const;  // 获取旋转矩形蒙版
	cv::Vec4d getCenterLineAbs() const;  // 获取绝对坐标系下的中心线
	cv::Vec4d getCenterLineRel() const;  // 获取相对外框坐标系下的中心线
	cv::Vec4d getInitLineRel() const;  // 获取相对坐标的初始直线
	cv::Vec4d getInitLineAbs() const;  // 获取绝对坐标的初始直线


	/// 绘图相关
    void drawCornerPoints(cv::Mat &showImage, cv::Scalar color = cv::Scalar::all(-1)) const;  // 绘制点
	void drawEdgeLines(cv::Mat& showImage, cv::Scalar color = cv::Scalar::all(-1), double lineWidth = 1) const;  // 绘制旋转矩形边缘直线
	void drawCenterLines(cv::Mat& showImage, cv::Scalar color = cv::Scalar::all(-1), double lineWidth = 1) const;


	/// 矩阵操作相关
	/*
		删除不位于旋转矩形内的像素点，rect是截取src的矩形
		RotRect2D r(line);
		Rect2d rect = r.getOuterRect();
		Mat src = img(rect).clone();
	*/
	void deleteOuterPoints(cv::Mat& src, cv::Rect2d rect, unsigned char light_thr = 50) const;
	void getInnerPoints(cv::Mat& src, std::vector<iPoint>& ipts, cv::Rect2d rect, unsigned char light_thr = 50) const;  // 将src中位于该旋转矩形内的点提出来，输出到数组中


	/// 二维变换相关
    void rotate(double angle);  // 绕缓冲区中心旋转
    void translation(cv::Point2d offset);  // 平移
    void zoom(double scale);  // 缩放
    bool contain(cv::Point2d pt) const;  // 判断点是否位于矩形内
	bool contain(double x, double y) const;

public:
	inline bool operator == (const RotRect2D& other)
	{
		return (
			abs(this->pt1().x - other.pt1().x) < 1e-5 &&
			abs(this->pt1().y - other.pt1().y) < 1e-5 &&
			abs(this->pt2().x - other.pt2().x) < 1e-5 &&
			abs(this->pt2().y - other.pt2().y) < 1e-5 &&
			abs(this->pt3().x - other.pt3().x) < 1e-5 &&
			abs(this->pt3().y - other.pt3().y) < 1e-5 &&
			abs(this->pt4().x - other.pt4().x) < 1e-5 &&
			abs(this->pt4().y - other.pt4().y) < 1e-5
		);
	}
public:
	cv::Point2d pt1() const { return this->mPt1; }
	cv::Point2d pt2() const { return this->mPt2; }
	cv::Point2d pt3() const { return this->mPt3; }
	cv::Point2d pt4() const { return this->mPt4; }
	cv::Size2d size() const { return this->mSize; }
	cv::Vec4d initLine() const { return this->mInitLine; }

private:
    cv::Point2d mPt1, mPt2, mPt3, mPt4;  // 四个角点
    cv::Size2d mSize;  // 矩形尺寸
	cv::Vec4d mInitLine;  // 备份创建旋转矩形的初始中心线
};
