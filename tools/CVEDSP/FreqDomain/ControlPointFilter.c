#include "ControlPointFilter.h"
#include <stdlib.h>
#include <stdio.h>
#include "../IntrinUtil/FloatArray.h"
#include "../Interpolation.h"

void CPF_Ctor(CPF* Dest, int Size)
{
	Dest -> ControlPointQ = - 1;
	Dest -> UFreq = 5500;
	Dest -> ControlPointList = malloc(sizeof(FreqPoint) * Size);
	Boost_FloatSet((float*)Dest -> ControlPointList, 0, sizeof(FreqPoint) * Size / sizeof(float));
}
void CPF_Dtor(CPF* Dest)
{
	free(Dest -> ControlPointList);
}

void CPF_AddControlPoint(CPF* Dest, FreqPoint* FPoint)
{
	int i, index;
	for(i = 0;i <= Dest -> ControlPointQ;i ++)
		if(FPoint -> Freq <= Dest -> ControlPointList[i].Freq)
			break;
	index = i;
	//Insert at index
	Dest -> ControlPointQ ++;
	for(i = Dest -> ControlPointQ;i >= index;i --)
		Dest -> ControlPointList[i + 1] = Dest -> ControlPointList[i];
	Dest -> ControlPointList[index] = *FPoint;
}
void CPF_AddControlFreq(CPF* Dest, float Freq, float Magnitude)
{
	int i, index;
	for(i = 0;i <= Dest -> ControlPointQ;i ++)
		if(Freq <= Dest -> ControlPointList[i].Freq)
			break;
	index = i;
	//Insert at index
	Dest -> ControlPointQ ++;
	for(i = Dest -> ControlPointQ;i >= index;i --)
		Dest -> ControlPointList[i + 1] = Dest -> ControlPointList[i];
	Dest -> ControlPointList[index].Freq      = Freq;
	Dest -> ControlPointList[index].Magnitude = Magnitude;
}

void CPF_Bake(float* Dest, CPF* Src, int DestLen)
{
	int i, j;
	int LIndex, UIndex;
	float Ratio;
	
	UIndex = 0; LIndex = 0;
	
	Src -> ControlPointList[Src -> ControlPointQ + 1].Freq      = Src -> UFreq;
	Src -> ControlPointList[Src -> ControlPointQ + 1].Magnitude = 0;
	for(i = 0;i <= Src -> ControlPointQ;i ++)
	{
		LIndex = (Src -> ControlPointList[i + 0].Freq) * DestLen / (Src -> UFreq);
		UIndex = (Src -> ControlPointList[i + 1].Freq) * DestLen / (Src -> UFreq);
		
		for(j = LIndex;j < UIndex;j ++)
		{
			Ratio = ((float)(j - LIndex)) / (UIndex - LIndex);
			Dest[j] =  Src -> ControlPointList[i + 0].Magnitude * (1.0f - Ratio) +
				       Src -> ControlPointList[i + 1].Magnitude * Ratio;
		}
	}
}

void CPF_Print(CPF* Src)
{
	int i;
	for(i = 0;i <= Src -> ControlPointQ;i ++)
	{
		printf("%f : %f\n", Src -> ControlPointList[i].Freq, Src -> ControlPointList[i].Magnitude);
	}
}
void CPF_PrintBake(float* Dest, int Length);