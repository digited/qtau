#include "DFW.h"
#include <stdlib.h>
#include <stdio.h>
#include "../IntrinUtil/FloatArray.h"

void DFW_Ctor(DFW* Dest)
{
	Dest -> DPointQ = - 1;
	Dest -> UFreq = 5500;
	Dest -> DPointList = malloc(sizeof(float) * 40);
	Boost_FloatSet((float*)Dest -> DPointList, 0, 40);
}
void DFW_Dtor(DFW* Dest)
{
	free(Dest -> DPointList);
}

void DFW_AddControlPoint(DFW* Dest, FreqPair* DPoint)
{
	int i, index;
	for(i = 0;i <= Dest -> DPointQ;i ++)
		if(DPoint -> To <= Dest -> DPointList[i].To)
			break;
	index = i;
	//Insert at index
	Dest -> DPointQ ++;
	for(i = Dest -> DPointQ;i >= index;i --)
		Dest -> DPointList[i + 1] = Dest -> DPointList[i];
	Dest -> DPointList[index] = *DPoint;
}
void DFW_AddControlFreq(DFW* Dest, float From, float To)
{
	int i, index;
	for(i = 0;i <= Dest -> DPointQ;i ++)
		if(To <= Dest -> DPointList[i].To)
			break;
	index = i;
	//Insert at index
	Dest -> DPointQ ++;
	for(i = Dest -> DPointQ;i >= index;i --)
		Dest -> DPointList[i + 1] = Dest -> DPointList[i];
	Dest -> DPointList[index].From = From;
	Dest -> DPointList[index].To   = To;
}

void DFW_Bake(float* Dest, DFW* Src, int DestLen)
{
	int i, j;
	int LIndex, UIndex;
	float Ratio;
	float MagRatio;

	MagRatio = ((float)DestLen) / (Src -> UFreq);
	Src -> DPointList[Src -> DPointQ + 1].From = Src -> UFreq;
	Src -> DPointList[Src -> DPointQ + 1].To   = Src -> UFreq;
	for(i = 0;i <= Src -> DPointQ;i ++)
	{
		LIndex = (Src -> DPointList[i + 0].To) * DestLen / (Src -> UFreq);
		UIndex = (Src -> DPointList[i + 1].To) * DestLen / (Src -> UFreq);
		for(j = LIndex;j < UIndex;j ++)
		{
			Ratio = ((float)(j - LIndex)) / (UIndex - LIndex);
			Dest[j] = (Src -> DPointList[i + 0].From * (1.0f - Ratio) +
				       Src -> DPointList[i + 1].From * Ratio) * MagRatio;
		}
	}
}

void DFW_Print(DFW* Src)
{
	int i;
	for(i = 0;i <= Src -> DPointQ;i ++)
	{
		printf("%f -> %f\n", Src -> DPointList[i].From, Src -> DPointList[i].To);
	}
}
void DFW_PrintBake(float* Dest, int Length)
{
	int i;
	for(i = 0;i < Length;i ++)
	{
		printf("%d:\t %d -> %f\n", i, i * 22050 / Length, Dest[i]);
	}
}