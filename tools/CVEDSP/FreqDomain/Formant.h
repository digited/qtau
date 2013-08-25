#ifndef FORMANT_H
#define FORMANT_H
#include "ControlPointFilter.h"

#ifdef __cplusplus
extern "C"{
#endif

extern void ExtractFormantCPF(CPF* Dest, float* Spectrum, float BaseFrequency, int Amount);

#ifdef __cplusplus
}
#endif

#endif