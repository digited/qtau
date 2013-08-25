#include "Interpolation.h"
#include <stdio.h>
/* NOT FINISHED YET */

inline Parabola GenerateQuad(float x0, float y0, float x1, float y1, float x2, float y2)
{
	Parabola ret;
	ret.a = ((y0 - y1) / (x0 - x1) - (y1 - y2) / (x1 - x2)) / (x0 - x2);
	ret.b = (y0 - y1) / (x0 - x1) - ret.a * (x0 + x1);
	ret.c = y0 - ret.b * x0 - ret.a * x0 * x0;
	return ret;
}
void QuadraticSpline(Parabola* Dest, float* X, float* Y, int Amount)
{
	int i;
	Dest[0] = GenerateQuad(X[0], Y[0], X[1], Y[1], X[2], Y[2]);
	for(i = 1;i < Amount - 1;i ++)
	{
		Dest[i].a = ((Y[i] - Y[i + 1]) / (X[i] - X[i + 1]) - 2 * Dest[i - 1].a * X[i] - Dest[i - 1].b) / (X[i + 1] - X[i]);
		Dest[i].b = 2 * Dest[i - 1].a * X[i] + Dest[i - 1].b - 2 * Dest[i].a * X[i];
		Dest[i].c = Y[i] - Dest[i].a * X[i] * X[i] - Dest[i].b * X[i];
	}
}
