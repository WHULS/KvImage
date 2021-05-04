#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include "iPoint.h"


class RotRect2D
{
public:
    /**
    * @param lengthExpand: ��ֱ��ƽ�з����������չ��Ĭ��Ϊ0��
    * @param widthExpand: ��ֱ�ߴ�ֱ�����������չ��Ĭ��Ϊ10��
    */
    RotRect2D(cv::Vec4d centerLine, double lengthExpand = 0.0, double widthExpand = 10.0);
    RotRect2D(cv::Point2d pt1, cv::Point2d pt2, cv::Point2d pt3, cv::Point2d pt4);
    RotRect2D();
    ~RotRect2D() {}

	/// ��ȡ�������
    double getRotateAngle() const;  // ��ȡ������ת�Ƕȣ�ͼ������ϵ��
	cv::Point2d getRectCenter() const;  // ��ȡ��������
	cv::Size2d getRectSize() const;  // ��ȡ���δ�С
    std::vector<cv::Vec4d> getEdgeLines() const;  // ��ȡ��Եֱ��
	void getCenterPoints(std::vector<cv::Point>& points) const;  // ��ȡ�����ߵĵ�
	void getEdgePoints(std::vector<cv::Point>& points) const;  // ��ȡ��Ե�㣨���У�
    cv::Rect2d getOuterRect() const;  // ��ȡ��Ӿ��Σ���������ƽ�У�
	cv::Mat getMask(const cv::Rect& refMatRect) const;  // ��ȡ��ת�����ɰ�
	cv::Vec4d getCenterLineAbs() const;  // ��ȡ��������ϵ�µ�������
	cv::Vec4d getCenterLineRel() const;  // ��ȡ����������ϵ�µ�������
	cv::Vec4d getInitLineRel() const;  // ��ȡ�������ĳ�ʼֱ��
	cv::Vec4d getInitLineAbs() const;  // ��ȡ��������ĳ�ʼֱ��


	/// ��ͼ���
    void drawCornerPoints(cv::Mat &showImage, cv::Scalar color = cv::Scalar::all(-1)) const;  // ���Ƶ�
	void drawEdgeLines(cv::Mat& showImage, cv::Scalar color = cv::Scalar::all(-1), double lineWidth = 1) const;  // ������ת���α�Եֱ��
	void drawCenterLines(cv::Mat& showImage, cv::Scalar color = cv::Scalar::all(-1), double lineWidth = 1) const;


	/// ����������
	/*
		ɾ����λ����ת�����ڵ����ص㣬rect�ǽ�ȡsrc�ľ���
		RotRect2D r(line);
		Rect2d rect = r.getOuterRect();
		Mat src = img(rect).clone();
	*/
	void deleteOuterPoints(cv::Mat& src, cv::Rect2d rect, unsigned char light_thr = 50) const;
	void getInnerPoints(cv::Mat& src, std::vector<iPoint>& ipts, cv::Rect2d rect, unsigned char light_thr = 50) const;  // ��src��λ�ڸ���ת�����ڵĵ�������������������


	/// ��ά�任���
    void rotate(double angle);  // �ƻ�����������ת
    void translation(cv::Point2d offset);  // ƽ��
    void zoom(double scale);  // ����
    bool contain(cv::Point2d pt) const;  // �жϵ��Ƿ�λ�ھ�����
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
    cv::Point2d mPt1, mPt2, mPt3, mPt4;  // �ĸ��ǵ�
    cv::Size2d mSize;  // ���γߴ�
	cv::Vec4d mInitLine;  // ���ݴ�����ת���εĳ�ʼ������
};
