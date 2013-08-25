#include "Interface.h"
#include "malloc.h"
#include "tools/CVEDSP/FreqDomain/FDAnalysis.h"
#include "tools/CVEDSP/FreqDomain/Spectrum.h"
#include "tools/CVEDSP/FreqDomain/Filter.h"
#include "tools/CVEDSP/TimeDomain/Window.h"
#include "tools/CVEDSP/IntrinUtil/FloatArray.h"
#include "tools/CVEDSP/FreqDomain/Formant.h"
#include "tools/CVEDSP/FreqDomain/FECSOLA.h"
#include "tools/CVEDSP/TimeDomain/PSOLA.h"
#include "tools/CVEDSP/DFT/FFT.h"

#include "editor/Utils.h"
float BF;
int Length;
int SampleRate;

int Pulses[5000];
PSOLAFrame PFrames[5000];
PSOLAFrame ModifiedPFrames[5000];
PulseDescriptor PD;

int FirstRun = 1;

float GraphFactor;

void LoadWav(float* wave, char channels, int sampleRate, unsigned long totalSamples)
{
    int i;
    if(channels == 1)
    {
        SampleRate = sampleRate;
        Length = totalSamples - SampleRate;
        SetSampleRate(sampleRate);
        BF = GetBaseFrequencyFromWave(wave + Length / 2, 80, 1500, 13);
        qDebug() << QString("Fundamental Freq: ") << BF;

        ExtractPulsesByBaseFrequency(Pulses, &PD, wave, BF, Length);
        qDebug() << PD.Amount << QString(" pulses extracted.");
        qDebug() << QString("VOT: ") << (float)Pulses[PD.VoiceOnsetIndex] / SampleRate;
        if(FirstRun)
        {
            for(i = 0;i < 5000; i ++)
                PSOLAFrame_Ctor(&PFrames[i], 1024);
            for(i = 0;i < 5000; i ++)
                PSOLAFrame_Ctor(&ModifiedPFrames[i], 1024);
            FirstRun = 0;
        }
        for(i = 0;i < PD.Amount;i ++)
        {
            PSOLAFrame_SecureGet(&PFrames[i], wave, Pulses[i] + 1024, Pulses[i]);
        }
    }
}

void UpdateSpectrum1(float* DestArray, float* Src)
{
    int i;
    float max = 0;
    float* Window = (float*)malloc(4 * 1024);
    float* InterArray = (float*)malloc(4 * 1024);
    GenerateHamming(Window, 1024);
    GetMagnitudeFromWave(InterArray, PFrames[PD.Amount / 2].Data, Window, 10);

    for(i = 0;i < 512;i ++)
        if(InterArray[i] > max)
            max = InterArray[i];
    for(i = 0;i < 512;i ++)
        DestArray[i] = InterArray[i / 4] / max * 3;

    Boost_FloatMul(DestArray, DestArray, -0.5, 512);
    Boost_FloatAdd(DestArray, DestArray, 0.9, 512);
    free(InterArray);
    free(Window);
    GraphFactor = 1.0f / max;
}

void UpdateSpectrum2(float* DestArray, FECSOLAState paramsBefore, FECSOLAState paramsAfter)
{
    int i;
    FECSOLAFilter FormantFilter;
    FECSOLAFilter_Ctor(&FormantFilter, 2048);
    CPF FormantCPF;
    CPF_Ctor(&FormantCPF, 1000);
    FormantCPF.UFreq = SampleRate / 2;
    FormantCPF.ControlPointQ = 0;

    paramsBefore.F0 = BF;
    paramsBefore.S0 = 1;
    paramsAfter.F0 = BF;
    paramsAfter.S0 = 1;

    float* Window = (float*)malloc(4 * 1024);
    float* InterArray = (float*)malloc(4 * 1024);
    GenerateHamming(Window, 1024);
    GetMagnitudeFromWave(InterArray, PFrames[PD.Amount / 2].Data, Window, 10);

    ExtractFormantCPF(&FormantCPF, InterArray, BF, 1024);
    Boost_FloatSet(Window, 0, 512);
    CPF_Bake(Window, &FormantCPF, 512);
    Boost_FloatAdd(Window, Window, 0.001, 512);
    Boost_FloatDivArr(InterArray, InterArray, Window, 512);

    FECSOLAFilter_GetFromFormantEnvelope(&FormantFilter, &FormantCPF, &paramsBefore);
    Boost_FloatSet(Window, 0, 512);
    FECSOLAFilter_Bake(Window, &FormantFilter, &paramsAfter, 1024);

    Boost_FloatMulArr(InterArray, InterArray, Window, 512);

    for(i = 0;i < 512;i ++)
        DestArray[i] = InterArray[i / 4] * GraphFactor * 3;

    Boost_FloatMul(DestArray, DestArray, -0.5, 512);
    Boost_FloatAdd(DestArray, DestArray, 0.9, 512);

    CPF_Dtor(&FormantCPF);
    FECSOLAFilter_Dtor(&FormantFilter);
    free(InterArray);
    free(Window);
}

float FRe[1024];
float FIm[1024];
float Magnitude[1024];
float Windowed[1024];
float Envelope[1024];
float ModifiedEnvelope[1024];
void Synthesis(float* DestWave, int sampleRate, FECSOLAState paramsBefore, FECSOLAState paramsAfter)
{
    int i;

    FECSOLAFilter FormantFilter;
    FECSOLAFilter_Ctor(&FormantFilter, 2048);
    CPF FormantCPF;
    CPF_Ctor(&FormantCPF, 1000);
    FormantCPF.UFreq = SampleRate / 2;
    FormantCPF.ControlPointQ = 0;

    paramsBefore.F0 = BF;
    paramsBefore.S0 = 1;
    paramsAfter.F0 = BF;
    paramsAfter.S0 = 1;

    float* Window = (float*)malloc(4 * 1024);
    GenerateHamming(Window, 1024);

    Boost_FloatSet(DestWave, 0, Length + SampleRate);

    for(i = 0;i < PD.Amount; i++)
    {
        ApplyWindow(Windowed, PFrames[i].Data, Window, 0, 1024);
        RFFT(FRe, FIm, Windowed, 10);
        GetMagnitude(Magnitude, FRe, FIm, 1024);

        FormantCPF.ControlPointQ = 0;
        ExtractFormantCPF(&FormantCPF, Magnitude, BF, 1024);
        CPF_Bake(Envelope, &FormantCPF, 512);
        Boost_FloatAdd(Envelope, Envelope, 0.001, 512);
        Boost_FloatDivArr(FRe, FRe, Envelope, 512);
        Boost_FloatDivArr(FIm, FIm, Envelope, 512);

        FECSOLAFilter_GetFromFormantEnvelope(&FormantFilter, &FormantCPF, &paramsBefore);
        FECSOLAFilter_Bake(ModifiedEnvelope, &FormantFilter, &paramsAfter, 1024);

        Boost_FloatCopy(ModifiedEnvelope + 139, Envelope + 139, 512 - 139);
        Boost_FloatMulArr(FRe, FRe, ModifiedEnvelope, 512);
        Boost_FloatMulArr(FIm, FIm, ModifiedEnvelope, 512);
        Reflect(FRe, FIm, FRe, FIm, 10);

        RIFFT(Windowed, FRe, FIm, 10);
        Boost_FloatDivArr(ModifiedPFrames[i].Data, Windowed, Window, 1024);
    }

    PSOLA_Regenesis(DestWave, ModifiedPFrames, Pulses, Pulses, PD.Amount);

    CPF_Dtor(&FormantCPF);
    FECSOLAFilter_Dtor(&FormantFilter);
    free(Window);
}
