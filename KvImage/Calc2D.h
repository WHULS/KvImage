#pragma once
#include <opencv2/opencv.hpp>
#include "px.h"
#include <QDebug>

class Calc2D
{
public:
	/* ����viewSizeԼ���µ�ͼƬ���� */
	static cv::Rect2d calcImageRect(const cv::Mat& img, const cv::Size2d& viewSize);

	/*��ȡͼ����ĳ��������أ�ͼ����ͼ����ο��ܲ�һ�£�pt��imageRect��ͬ������ϵ�£�imageRect��img�����Ź�ϵһ��*/
	static bool getPxFromImage(px& p, const cv::Point& pt, const cv::Mat& img, const cv::Rect& imageRect);

	/*Ҫ����ʾ��viewSize��ʾͼ�ϵ�һ��������subRect�����������ľ��δ�С*/
	static cv::Rect2d calcImageRectWithViewRect(const cv::Mat& img, const cv::Size2d& viewSize, const cv::Rect2d& subRect);
};

