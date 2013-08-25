#ifndef DFW_H
#define DFW_H

#ifdef __cplusplus
extern "C"{
#endif

struct FreqPair
{
	float From;
	float To;
};
typedef struct FreqPair FreqPair;

struct DFW
{
	int DPointQ;
	int UFreq;
	FreqPair* DPointList;
};
typedef struct DFW DFW;

extern void DFW_Ctor(DFW* Dest);
extern void DFW_Dtor(DFW* Dest);

extern void DFW_AddControlPoint(DFW* Dest, FreqPair* DPoint);
extern void DFW_AddControlFreq(DFW* Dest, float From, float To);

extern void DFW_Bake(float* Dest, DFW* Src, int DestLen);

extern void DFW_Print(DFW* Src);
extern void DFW_PrintBake(float* Dest, int Length);

#ifdef __cplusplus
}
#endif

#endif