#pragma once
#include <opencv2/opencv.hpp>
#include "px.h"

class Calc2D
{
public:
	/* ����imgSizeԼ���µ�ͼƬ���� */
	static cv::Rect calcImageRect(const cv::Mat& img, cv::Size imgSize);

	/*��ȡͼ����ĳ��������أ�ͼ����ͼ����ο��ܲ�һ�£�pt��imageRect��ͬ������ϵ�£�imageRect��img�����Ź�ϵһ��*/
	static bool getPxFromImage(px& p, const cv::Point& pt, const cv::Mat& img, const cv::Rect& imageRect);
};

