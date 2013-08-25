#ifndef CONTROLPOINTFILTER_H
#define CONTROLPOINTFILTER_H

#ifdef __cplusplus
extern "C"{
#endif
	
struct FreqPoint
{
	float Freq;
	float Magnitude;
};

typedef struct FreqPoint FreqPoint;

struct CPF
{
	int ControlPointQ;
	int UFreq;
	FreqPoint* ControlPointList;
};

typedef struct CPF CPF;

extern void CPF_Ctor(CPF* Dest, int Size);
extern void CPF_Dtor(CPF* Dest);

extern void CPF_AddControlPoint(CPF* Dest, FreqPoint* FPoint);
extern void CPF_AddControlFreq(CPF* Dest, float Freq, float Magnitude);

extern void CPF_Bake(float* Dest, CPF* Src, int DestLen);

extern void CPF_Print(CPF* Src);
extern void CPF_PrintBake(float* Dest, int Length);

#ifdef __cplusplus
}
#endif

#endif