#include "FDAnalysis.h"
#include "Filter.h"
#include "Spectrum.h"
#include "../TimeDomain/Window.h"
#include "../DFT/FFT.h"
#include "../IntrinUtil/Calculation.h"
#include "../IntrinUtil/FloatArray.h"
#include <math.h>

#define FreqToIndex(Freq) (Freq * Amount / SampleRate)
#define IndexToFreq(Index) ((float)SampleRate * Index / Amount)

float Window[16384];
float Spec[16384];
float GetBaseFrequencyFromSpectrum(float* Spectrum, float LFreq, float UFreq, int Amount)
{
	int LBoundIndex, UBoundIndex;
	int i;
	float Avg;
	LBoundIndex = FreqToIndex(LFreq);
	UBoundIndex = FreqToIndex(UFreq);
	Avg = Boost_FloatAvg(Spectrum + LBoundIndex, UBoundIndex - LBoundIndex);
	for(i = LBoundIndex > 2 ? LBoundIndex : 2;i < UBoundIndex;i ++)
	{
		if(Spectrum[i] > Avg)
		{
			if(Spectrum[i - 1] < Spectrum[i] && Spectrum[i - 2] < Spectrum[i] &&
			   Spectrum[i + 1] < Spectrum[i] && Spectrum[i + 2] < Spectrum[i])
				return IndexToFreq(i);
		}
	}
	return 0;
}

float GetBaseFrequencyFromCepstrum(float* Cepstrum, float LFreq, float UFreq, int Amount)
{
	return (float)SampleRate / GetBasePeriodFromCepstrum(Cepstrum,
	                                                     (float)SampleRate / UFreq,
	                                                     (float)SampleRate / LFreq,
	                                                     Amount);
}
int GetBasePeriodFromCepstrum(float* Cepstrum, float LPeriod, float UPeriod, int Amount)
{
	int i;
	float max = 0;
	int maxIndex = 0;
	for(i = LPeriod;i < UPeriod;i ++)
	{
		if(Cepstrum[i] > max)
		{
			max = Cepstrum[i];
			maxIndex = i;
		}
	}
	return maxIndex;
}
float GetBaseFrequencyFromWave(float* Wave, float LFreq, float UFreq, int Power)
{
	int Amount;
	if(Power < 9 || Power > 14)
		return 0;
	Amount = pow(2, Power);
	GenerateHamming(Window, Amount);
	GetMagnitudeFromWave(Spec, Wave, Window, Power);
	return GetBaseFrequencyFromSpectrum(Spec, LFreq, UFreq, Amount);
}
