#include "Formant.h"
#include "Filter.h"
#include "../IntrinUtil/Calculation.h"

void ExtractFormantCPF(CPF* Dest, float* Spectrum, float BaseFrequency, int Amount)
{
	int LIndex, UIndex;
	float Freq = BaseFrequency;
	float HalfFreq = BaseFrequency * 0.5;
	float Max;
	for(;Freq < SampleRate / 2 - BaseFrequency;Freq += BaseFrequency)
	{
		Max = 0;
		LIndex = (Freq - HalfFreq) / SampleRate * Amount;
		UIndex = (Freq + HalfFreq) / SampleRate * Amount;
		for(;LIndex < UIndex;LIndex ++)
			if(Spectrum[LIndex] > Max)
				Max = Spectrum[LIndex];
		CPF_AddControlFreq(Dest, Freq, Max);
	}
}