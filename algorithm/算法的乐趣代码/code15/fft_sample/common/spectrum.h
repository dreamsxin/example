#ifndef __SPECTRUM_H__
#define __SPECTRUM_H__

#include "fft.h"


bool PowerSpectrumT(FFT_HANDLE *hfft, short *sampleData, int totalSamples, int channels, float *power);

bool PowerSpectrumS(FFT_HANDLE *hfft, short *sampleData, int totalSamples, int channels, float *power);

#endif //__SPECTRUM_H__
