#include "PSOLA.h"
#include "../IntrinUtil/FloatArray.h"
#include "../FrameProcessor.h"
#include "../FreqDomain/Filter.h"
#include "TDEffects.h"
#include <stdlib.h>
#include <stdio.h>

float VolumeThreshold = 0.005;
float VoiceThreshold = 0.005;

void PSOLAFrame_Ctor(PSOLAFrame* Frame, int32_t Length)
{
	Frame -> Data = malloc(sizeof(float) * Length);
	Frame -> Length = Length;
}
void PSOLAFrame_Dtor(PSOLAFrame* Frame)
{
	free(Frame -> Data);
}
void PSOLAFrame_DtorAll(PSOLAFrame* Frame, int32_t Amount)
{
	int i;
	for(i = 0;i < Amount;i ++)
		PSOLAFrame_Dtor(Frame + i);
}

void PSOLAFrame_Get(PSOLAFrame* Frame, float* Src, int32_t Position)
{
	Boost_FloatCopy(Frame -> Data, Src + Position - Frame -> Length / 2, Frame -> Length);
}
void PSOLAFrame_SecureGet(PSOLAFrame* Frame, float* Src, int32_t UBound, int32_t Position)
{
	int Length = Frame -> Length;
	int Offset = 0;
	Boost_FloatSet(Frame -> Data, 0, Length);

	if(Position < Length / 2)
	{
		Offset = Length / 2 - Position;
		Length -= Offset;
	}
	if(Position + (Frame -> Length) / 2 > UBound)
		Length -= Position + (Frame -> Length) / 2 - UBound;
	
	Boost_FloatCopy(Frame -> Data + Offset, Src + Position - (Frame -> Length / 2) + Offset, Length);
}
void PSOLAFrame_StaticGet(PSOLAFrame* Frame, float* Src, int32_t Position)
{
	Frame -> Data = Src + Position - Frame -> Length / 2;
}
void PSOLAFrame_Mix(float* Dest, PSOLAFrame* Frame, int32_t Front, int32_t Position, int32_t Back)
{
	int32_t i;
	int32_t half = Frame -> Length / 2;
	float ratio;
	for(i = 0;i < Position - Front;i ++)
	{
		ratio = (float)i / (Position - Front);
		Dest[i + Front] += ratio * Frame -> Data[half - (Position - Front) + i];
	}
	for(i = 0;i < Back - Position;i ++)
	{
		ratio = 1 - (float)i / (Back - Position);
		Dest[i + Position] += ratio * Frame -> Data[half + i];
	}
}
void PSOLAFrame_MixByRatio(float* Dest, PSOLAFrame* Frame, int32_t Front, int32_t Position, int32_t Back, float Ratio, int MType)
{
	int32_t i;
	int32_t half = Frame -> Length / 2;
	float ratio_;
	for(i = 0;i < Position - Front;i ++)
	{
		ratio_ = (float)i / (Position - Front);
		Dest[i + Front] += Ratio * ratio_ * Frame -> Data[half - (Position - Front) + i];
	}
	for(i = 0;i < Back - Position;i ++)
	{
		ratio_ = 1 - (float)i / (Back - Position);
		Dest[i + Position] += Ratio * ratio_ * Frame -> Data[half + i];
	}
}
void PSOLAFrame_MixByRatio_Float(float* Dest, PSOLAFrame* Frame, float Front, float Position, float Back, float Ratio, int MType)
{
	int32_t i;
	int32_t half = Frame -> Length / 2;
	int32_t Front_Int, Position_Int, Back_Int;
	float ratio_;
	float Total;
	float IntRatio;
	Front_Int = (int)Front;
	Position_Int = (int)Position;
	Back_Int = (int)Back;
	IntRatio = Position - (float)((int)Position);
	if(MType == MixNormal || MType == MixBack)
		for(i = 0;i < Position_Int - Front_Int;i ++)
		{
			ratio_ = (float)i / (Position - Front);
			Total = Ratio * ratio_ * Frame -> Data[half - (int)(Position - Front) + i];
			Dest[i + Front_Int + 0] += Total * (1.0f - IntRatio);
			Dest[i + Front_Int + 1] += Total * IntRatio;
		}
	else
		for(i = 0;i < Position_Int - Front_Int;i ++)
		{
			ratio_ = 1;
			Total = Ratio * ratio_ * Frame -> Data[half - (int)(Position - Front) + i];
			Dest[i + Front_Int + 0] += Total * (1.0f - IntRatio);
			Dest[i + Front_Int + 1] += Total * IntRatio;
		}
	if(MType == MixNormal || MType == MixFront)
		for(i = 0;i < Back_Int - Position_Int;i ++)
		{
			ratio_ = 1.0f - (float)i / (Back - Position);
			Total = Ratio * ratio_ * Frame -> Data[half + i];
			Dest[i + Position_Int + 0] += Total * (1.0f - IntRatio);
			Dest[i + Position_Int + 1] += Total * IntRatio;
		}
	else
		for(i = 0;i < Back_Int - Position_Int;i ++)
		{
			ratio_ = 1;
			Total = Ratio * ratio_ * Frame -> Data[half + i];
			Dest[i + Position_Int + 0] += Total * (1.0f - IntRatio);
			Dest[i + Position_Int + 1] += Total * IntRatio;
		}
}

void SetExtractorVoiceThreshold(float _Threshold)
{
	VoiceThreshold = _Threshold;
}
void SetExtractorVolumeThreshold(float _Threshold)
{
	VolumeThreshold = _Threshold;
}

void ExtractPulsesByBaseFrequency(int32_t* DestPulses, PulseDescriptor* DestDescriptor, float* Wave, float BaseFrequency, int32_t Length)
{
	float* Processed = malloc(sizeof(float) * Length);
	float* Averaged = malloc(sizeof(float) * Length);
	int32_t i, j;
	int32_t BasePeriod = SampleRate / BaseFrequency;
	int32_t VoiceOnsetTime, VoiceKeepTime;
	int32_t VoiceOnsetRecord, VoiceKeepRecord;
	int32_t SilenceTime;
	
	GenerateBandPass(ProcessorTmp_Filter, 80, BaseFrequency * 1.5, 512);
	Process(Processed, Wave, Processor_Filter, 10, Length);
	MovingAverage(Averaged, Processed, 1, BasePeriod * 0.2, Length);

	DestDescriptor -> Amount = 0;
	DestDescriptor -> VoiceOnsetIndex = - 1;
	
	VoiceKeepRecord = 0;
	VoiceKeepTime = 0;
	VoiceOnsetRecord = 0;
	VoiceOnsetTime = - 1;
	SilenceTime = 0;
	for(i = 0;i < Length - BasePeriod;i ++)
	{
		for(j = 0;j < BasePeriod;j ++)
		{
			if(Averaged[i + j] > VoiceThreshold || Averaged[i + j] < - VoiceThreshold)
			{
				i += BasePeriod;
				VoiceKeepTime ++;
				if(VoiceOnsetTime == -1)
					VoiceOnsetTime = i;
				if(VoiceKeepTime > VoiceKeepRecord)
				{
					VoiceKeepRecord = VoiceKeepTime;
					VoiceOnsetRecord = VoiceOnsetTime + BasePeriod * 2;
				}
				goto CheckNext;
			}
		}
		VoiceKeepTime = 0;
		VoiceOnsetTime = - 1;
		CheckNext:;
	}

	for(i = 0;i < Length;i ++)
	{
		if(Wave[i] > VolumeThreshold || Wave[i] < - VolumeThreshold)
		{
			SilenceTime = i;
			break;
		}		
	}
	
	for(i = SilenceTime;i < Length - BasePeriod;i ++)
	{
		if(i < VoiceOnsetRecord)
		{
			//Unvoiced
			//Voice ended.
			if(DestDescriptor -> VoiceOnsetIndex != - 1)
				break;
			i += BasePeriod;
			DestPulses[DestDescriptor -> Amount] = i;
			DestDescriptor -> Amount ++;
			continue;
		}else
		{
			//Voiced
			if(Averaged[i] < 0 && Averaged[i + 1] > 0)
			{
				if(DestDescriptor -> VoiceOnsetIndex == - 1)
					DestDescriptor -> VoiceOnsetIndex = DestDescriptor -> Amount;
				DestPulses[DestDescriptor -> Amount] = i;
				DestDescriptor -> Amount ++;
			}
		}
	}
	free(Averaged);
	free(Processed);
}

void PSOLA_Regenesis(float* Dest, PSOLAFrame* Frames, int32_t* NewPulses, int32_t* OriginalPulses, int32_t PulseNum)
{
	int i;
	for(i = 1;i < PulseNum;i ++)
	{
		PSOLAFrame_Mix(Dest, Frames + i,
		               NewPulses[i] - (OriginalPulses[i] - OriginalPulses[i - 1]), 
		               NewPulses[i], 
		               NewPulses[i] + (OriginalPulses[i + 1] - OriginalPulses[i]));
	}
}