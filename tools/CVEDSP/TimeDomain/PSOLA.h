#ifndef PSOLA_H
#define PSOLA_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

enum PSOLAMixType
{
	MixFront = 0,
	MixNormal = 1,
	MixBack = 2
};

struct PSOLAFrame
{
	int Length;
	float* Data;
};

struct PulseDescriptor
{
	int Amount;
	int VoiceOnsetIndex;
};

typedef struct PSOLAFrame PSOLAFrame;
typedef struct PulseDescriptor PulseDescriptor;

#define PSOLAFrame_StaticCtor(Frame, _Length) (Frame) -> Length = (_Length)
#define PSOLAFrame_StaticDtor(Frame) ;
extern void PSOLAFrame_Ctor(PSOLAFrame* Frame, int32_t Length);
extern void PSOLAFrame_Dtor(PSOLAFrame* Frame);
extern void PSOLAFrame_DtorAll(PSOLAFrame* Frame, int32_t Amount);

extern void PSOLAFrame_Get(PSOLAFrame* Frame, float* Src, int32_t Position);
extern void PSOLAFrame_SecureGet(PSOLAFrame* Frame, float* Src, int32_t UBound, int32_t Position);
extern void PSOLAFrame_StaticGet(PSOLAFrame* Frame, float* Src, int32_t Position);
extern void PSOLAFrame_Mix(float* Dest, PSOLAFrame* Frame, int32_t Front, int32_t Position, int32_t Back);
extern void PSOLAFrame_MixByRatio(float* Dest, PSOLAFrame* Frame, int32_t Front, int32_t Position, int32_t Back, float Ratio, int MType);
extern void PSOLAFrame_MixByRatio_Float(float* Dest, PSOLAFrame* Frame, float Front, float Position, float Back, float Ratio, int MType);

extern void SetExtractorVoiceThreshold(float _Threshold);
extern void SetExtractorVolumeThreshold(float _Threshold);
extern void ExtractPulsesByBaseFrequency(int32_t* DestPulses, PulseDescriptor* DestDescriptor, float* Wave, float BaseFrequency, int32_t Length);

extern void PSOLA_Regenesis(float* Dest, PSOLAFrame* Frames, int32_t* NewPulses, int32_t* OriginalPulses, int32_t PulseNum);

#ifdef __cplusplus
}
#endif

#endif