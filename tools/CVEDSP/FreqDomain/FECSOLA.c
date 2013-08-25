#include "FECSOLA.h"
#include "Filter.h"
#include "../IntrinUtil/FloatArray.h"

float AntiAliasSpec[16384];

#define SFreq(x) ((x) * (Filter -> Size) / SampleRate)
#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

void FECSOLAFilter_Ctor(FECSOLAFilter* Dest, int Size)
{
	Dest -> Size = Size;
	
	PSOLAFrame_Ctor(& Dest -> LLimit, Size);
	PSOLAFrame_Ctor(& Dest -> ULimit, Size);
	
	PSOLAFrame_Ctor(& Dest -> F0, Size);
	PSOLAFrame_Ctor(& Dest -> F1, Size);
	PSOLAFrame_Ctor(& Dest -> F2, Size);
	PSOLAFrame_Ctor(& Dest -> F3, Size);
}
void FECSOLAFilter_Dtor(FECSOLAFilter* Dest)
{
	PSOLAFrame_Dtor(& Dest -> LLimit);
	PSOLAFrame_Dtor(& Dest -> ULimit);
	
	PSOLAFrame_Dtor(& Dest -> F0);
	PSOLAFrame_Dtor(& Dest -> F1);
	PSOLAFrame_Dtor(& Dest -> F2);
	PSOLAFrame_Dtor(& Dest -> F3);
}

void FECSOLAFilter_GetFromFormantEnvelope(FECSOLAFilter* Dest, CPF* FormantEnvelope, FECSOLAState* State)
{
	CPF_Bake(AntiAliasSpec, FormantEnvelope, Dest -> Size / 2);

	PSOLAFrame_SecureGet(& Dest -> LLimit, AntiAliasSpec, Dest -> Size, 0);
	PSOLAFrame_SecureGet(& Dest -> ULimit, AntiAliasSpec, Dest -> Size, 6000 * Dest -> Size / SampleRate);
	
	PSOLAFrame_SecureGet(& Dest -> F0, AntiAliasSpec, Dest -> Size, State -> F0 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F1, AntiAliasSpec, Dest -> Size, State -> F1 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F2, AntiAliasSpec, Dest -> Size, State -> F2 * Dest -> Size / SampleRate);
	PSOLAFrame_SecureGet(& Dest -> F3, AntiAliasSpec, Dest -> Size, State -> F3 * Dest -> Size / SampleRate);

	Boost_FloatDiv(Dest -> F0.Data, Dest -> F0.Data, State -> S0, Dest -> Size);
	Boost_FloatDiv(Dest -> F1.Data, Dest -> F1.Data, State -> S1, Dest -> Size);
	Boost_FloatDiv(Dest -> F2.Data, Dest -> F2.Data, State -> S2, Dest -> Size);
	Boost_FloatDiv(Dest -> F3.Data, Dest -> F3.Data, State -> S3, Dest -> Size);

	Dest -> SF0 = State -> F0;
	Dest -> SF1 = State -> F1;
	Dest -> SF2 = State -> F2;
	Dest -> SF3 = State -> F3;
}
void FECSOLAFilter_Bake(float* Dest, FECSOLAFilter* Filter, FECSOLAState* State, int Length)
{
	int i, j;
	int LIndex, UIndex;
	Boost_FloatSet(AntiAliasSpec, 0, Filter -> Size);
	/*
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> LLimit,
	                            0,
	                            0,
	                            SFreq(State -> F0),
	                            1,
	                            MixFront);*/
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> ULimit,
	                            SFreq(State -> F3),
	                            SFreq(6000),
	                            SFreq(6000),
	                            1,
	                            MixBack);
	/*
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F0,
	                            0,
	                            SFreq(State -> F0),
	                            SFreq(State -> F0 + State -> L0),
	                            State -> S0);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F1,
	                            SFreq(max(State -> F0 - 100, min(State -> F1 - State -> L1, State -> F0))),
	                            SFreq(State -> F1),
	                            SFreq(State -> F1 + State -> L1),
	                            State -> S1);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F2,
	                            SFreq(max(State -> F0, State -> F2 - State -> L2)),
	                            SFreq(State -> F2),
	                            SFreq(State -> F2 + State -> L2),
	                            State -> S2);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F3,
	                            SFreq(max(State -> F0, State -> F3 - State -> L3)),
	                            SFreq(State -> F3),
	                            278,
	                            State -> S3);*/

	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F0,
	                            0,
	                            SFreq(State -> F0),
	                            SFreq(State -> F0 + Filter -> SF1 - Filter -> SF0),
	                            State -> S0,
	                            MixFront);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F1,
	                            SFreq(max(State -> F0 - 100, State -> F1 + Filter -> SF0 - Filter -> SF1)),
	                            SFreq(State -> F1),
	                            SFreq(State -> F1 + Filter -> SF2 - Filter -> SF1),
	                            State -> S1,
	                            MixNormal);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F2,
	                            SFreq(max(State -> F0 - 100, State -> F2 + Filter -> SF1 - Filter -> SF2)),
	                            SFreq(State -> F2),
	                            SFreq(State -> F2 + Filter -> SF3 - Filter -> SF2),
	                            State -> S2,
	                            MixNormal);
	PSOLAFrame_MixByRatio_Float(AntiAliasSpec, & Filter -> F3,
	                            SFreq(max(State -> F0 - 100, State -> F3 + Filter -> SF2 - Filter -> SF3)),
	                            SFreq(State -> F3),
	                            SFreq(6000),
	                            State -> S3,
	                            MixNormal);
	Boost_FloatSet(Dest, 0, Length);
	for(i = 0;i < Length;i ++)
	{
		LIndex = (i + 0) * Filter -> Size / Length;
		UIndex = (i + 1) * Filter -> Size / Length;
		for(j = LIndex;j < UIndex;j ++)
			Dest[i] += AntiAliasSpec[j];
		Dest[i] /= (float)(UIndex - LIndex);
	}
}

void FECSOLAState_Transition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
	FECSOLAState_FTransition(Dest, From, To, Ratio);
	FECSOLAState_LTransition(Dest, From, To, Ratio);
	FECSOLAState_STransition(Dest, From, To, Ratio);
}
inline void FECSOLAState_FTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
	Dest -> F0 = (1.0f - Ratio) * From -> F0 + Ratio * To -> F0;
	Dest -> F1 = (1.0f - Ratio) * From -> F1 + Ratio * To -> F1;
	Dest -> F2 = (1.0f - Ratio) * From -> F2 + Ratio * To -> F2;
	Dest -> F3 = (1.0f - Ratio) * From -> F3 + Ratio * To -> F3;
}
inline void FECSOLAState_LTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
	Dest -> L0 = (1.0f - Ratio) * From -> L0 + Ratio * To -> L0;
	Dest -> L1 = (1.0f - Ratio) * From -> L1 + Ratio * To -> L1;
	Dest -> L2 = (1.0f - Ratio) * From -> L2 + Ratio * To -> L2;
	Dest -> L3 = (1.0f - Ratio) * From -> L3 + Ratio * To -> L3;
}
inline void FECSOLAState_STransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio)
{
	Dest -> S0 = (1.0f - Ratio) * From -> S0 + Ratio * To -> S0;
	Dest -> S1 = (1.0f - Ratio) * From -> S1 + Ratio * To -> S1;
	Dest -> S2 = (1.0f - Ratio) * From -> S2 + Ratio * To -> S2;
	Dest -> S3 = (1.0f - Ratio) * From -> S3 + Ratio * To -> S3;
}