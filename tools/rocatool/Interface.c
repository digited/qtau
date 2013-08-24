#include "Interface.h"

void LoadWav(float* wave, char channels, int sampleRate, unsigned long totalSamples)
{
    //
}

void UpdateSpectrum1(float* DestArray, float* Src, FECSOLAState* parameters)
{
    int i;
    for(i = 0;i < 512;i ++)
        DestArray[i] = Src[i + 1024] * 10;
}

void UpdateSpectrum2(float* DestArray, FECSOLAState* parameters)
{
    int i;
    for(i = 0;i < 512;i ++)
        DestArray[i] = (float)i / 512;
}

void Synthesis(float* DestWave, int sampleRate, FECSOLAState paramsBefore, FECSOLAState paramsAfter)
{
    //
}
