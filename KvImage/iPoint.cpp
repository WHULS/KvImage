#include "iPoint.h"

iPoint::iPoint()
{
	this->mX = 0;
	this->mY = 0;
	this->mLight = 0;
}

iPoint::iPoint(int x, int y, uchar light)
{
	this->mX = x;
	this->mY = y;
	this->mLight = light;
}

void iPoint::setX(int x)
{
	this->mX = x;
}

void iPoint::setY(int y)
{
	this->mY = y;
}

void iPoint::setLight(uchar light)
{
	this->mLight = light;
}
