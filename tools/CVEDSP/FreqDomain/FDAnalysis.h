#ifndef FDANALYSIS_H
#define FDANALYSIS_H

#ifdef __cplusplus
extern "C"{
#endif
	
extern float GetBaseFrequencyFromSpectrum(float* Spectrum, float LFreq, float UFreq, int Amount);

extern float GetBaseFrequencyFromCepstrum(float* Cepstrum, float LFreq, float UFreq, int Amount);
extern int GetBasePeriodFromCepstrum(float* Cepstrum, float LPeriod, float UPeriod, int Amount);

extern float GetBaseFrequencyFromWave(float* Wave, float LFreq, float UFreq, int Power);

#ifdef __cplusplus
}
#endif

#endif