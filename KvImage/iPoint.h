#pragma once

typedef unsigned char uchar;

/// ��: �����꼰������
class iPoint
{
public:
	iPoint();
	iPoint(int x, int y, uchar light);

	int& x();
	int& y();
	uchar& light();

private:
	int mX;
	int mY;
	uchar mLight;
};
