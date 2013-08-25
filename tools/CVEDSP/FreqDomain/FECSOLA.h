#ifndef FECSOLA_H
#define FECSOLA_H
#include "../TimeDomain/PSOLA.h"
#include "ControlPointFilter.h"
//Formant Envelope Coefficient Shift and OverLap Add

#ifdef __cplusplus
extern "C"{
#endif

typedef struct FECSOLAFilter
{
	int Size;
	
	PSOLAFrame LLimit;
	PSOLAFrame ULimit;
	
	PSOLAFrame F0;
	PSOLAFrame F1;
	PSOLAFrame F2;
	PSOLAFrame F3;

	float SF0; //Frequency of Formant 1
	float SF1; //Frequency of Formant 2
	float SF2; //Frequency of Formant 3
	float SF3; //Frequency of Formant 4
	
} FECSOLAFilter;

typedef struct FECSOLAState
{
	float F0; //Frequency of Formant 1
	float F1; //Frequency of Formant 2
	float F2; //Frequency of Formant 3
	float F3; //Frequency of Formant 4

	float L0; //Width of Formant 1
	float L1; //Width of Formant 2
	float L2; //Width of Formant 3
	float L3; //Width of Formant 4

	float S0; //Strength of Formant 1
	float S1; //Strength of Formant 2
	float S2; //Strength of Formant 3
	float S3; //Strength of Formant 4
} FECSOLAState;

#define CreateFormant(FState, _F0, _F1, _F2, _F3, _L0, _L1, _L2, _L3, _S0, _S1, _S2, _S3) \
	(FState).F0 = (_F0); (FState).F1 = (_F1); (FState).F2 = (_F2); (FState).F3 = (_F3);\
	(FState).L0 = (_L0); (FState).L1 = (_L1); (FState).L2 = (_L2); (FState).L3 = (_L3);\
	(FState).S0 = (_S0); (FState).S1 = (_S1); (FState).S2 = (_S2); (FState).S3 = (_S3);

extern void FECSOLAFilter_Ctor(FECSOLAFilter* Dest, int Size);
extern void FECSOLAFilter_Dtor(FECSOLAFilter* Dest);

extern void FECSOLAFilter_GetFromFormantEnvelope(FECSOLAFilter* Dest, CPF* FormantEnvelope, FECSOLAState* State);
extern void FECSOLAFilter_Bake(float* Dest, FECSOLAFilter* Filter, FECSOLAState* State, int Length);

extern void FECSOLAState_Transition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_FTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_LTransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);
extern inline void FECSOLAState_STransition(FECSOLAState* Dest, FECSOLAState* From, FECSOLAState* To, float Ratio);

#ifdef __cplusplus
}
#endif

#endif