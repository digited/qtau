#include "Interface.h"
#include "malloc.h"
#include "tools/CVEDSP/FreqDomain/FDAnalysis.h"
#include "tools/CVEDSP/FreqDomain/Spectrum.h"
#include "tools/CVEDSP/FreqDomain/Filter.h"
#include "tools/CVEDSP/TimeDomain/Window.h"
#include "tools/CVEDSP/IntrinUtil/FloatArray.h"

void LoadWav(float* wave, char channels, int sampleRate, unsigned long totalSamples)
{
    //
}

void UpdateSpectrum1(float* DestArray, float* Src, FECSOLAState* parameters)
{
    float* Window = (float*)malloc(4 * 1024);
    float* InterArray = (float*)malloc(4 * 1024);
    GenerateHamming(Window, 1024);
    GetMagnitudeFromWave(InterArray, Src + 44100 / 2, Window, 10);
    int i;
    for(i = 0;i < 512;i ++)
        DestArray[i] = InterArray[i / 4];
    //Boost_FloatCopy(DestArray, InterArray, 512);
    Boost_FloatMul(DestArray, DestArray, -0.5, 512);
    Boost_FloatAdd(DestArray, DestArray, 1, 512);
    free(InterArray);
    free(Window);
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
