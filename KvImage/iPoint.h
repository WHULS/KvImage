#pragma once

typedef unsigned char uchar;

/// 类: 点坐标及其亮度
class iPoint
{
public:
	iPoint();
	iPoint(int x, int y, uchar light);

public:
	int x() const { return this->mX; }
	int y() const { return this->mY; }
	uchar light() const { return this->mLight; }

	void setX(int x);
	void setY(int y);
	void setLight(uchar light);

private:
	int mX;
	int mY;
	uchar mLight;
};
