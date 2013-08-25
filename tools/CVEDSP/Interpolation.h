#ifndef INTERPOLATION_H
#define INTERPOLATION_H
/* NOT FINISHED YET */

typedef struct Parabola
{
	float a;
	float b;
	float c;
} Parabola;

extern Parabola GenerateQuad(float x0, float y0, float x1, float y1, float x2, float y2);
extern void QuadraticSpline(Parabola* Dest, float* X, float* Y, int Amount);

#endif
