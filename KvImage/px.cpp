#include "px.h"
px::px()
{
	this->x = this->y = 0;
	this->channels = 1;
	this->r = this->g = this->b = 0;
}

px::px(double x, double y, int channels, unsigned char r, unsigned char g, unsigned char b)
{
	this->x = x;
	this->y = y;
	this->channels = channels;
	this->r = r;
	this->g = g;
	this->b = b;
}
