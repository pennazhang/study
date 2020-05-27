#pragma once

//#include "OutputChannel.h"

const double PI = 3.14159265358979323846264338327950288419716939937510582;
const double FS = 48000.f;
const double M_SQRT2 = 1.41421356237309504880; //sqrt(2)

class plotBiquad
{
public:
	plotBiquad();
	~plotBiquad();
	void setParameter(bool enable, int ftype, double f0, double dBgain, double q_or_slope);
	void plotCurve(double *x, double *y, long pointslength);
private:
	double A1, A2, B0, B1, B2;
	double slopeToQ(double slope);
};

extern plotBiquad g_plotBiquad;