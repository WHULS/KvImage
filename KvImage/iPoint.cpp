#include "iPoint.h"

iPoint::iPoint()
{
	this->mX = 0;
	this->mY = 0;
	this->mLight = 0;
}

iPoint::iPoint(int x, int y, uchar light)
{
	this->x() = x;
	this->y() = y;
	this->light() = light;
}

int& iPoint::x()
{
	return this->mX;
}

int& iPoint::y()
{
	return this->mY;
}

uchar& iPoint::light()
{
	return this->mLight;
}