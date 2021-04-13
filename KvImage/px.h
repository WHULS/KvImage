#pragma once
class px
{
public:
	px();
	px(double x, double y, int channels = 1, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
	double x, y;
	int channels;
	unsigned char r, g, b;
};

