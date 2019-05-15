#ifndef __EQUALIZER_H__
#define __EQUALIZER_H__


#include "fft.h"

typedef struct tagEQUALIZER_HANDLE
{
    FFT_HANDLE hfft;
    COMPLEX *filter;
}EQUALIZER_HANDLE;

bool InitEqualizer(EQUALIZER_HANDLE *hEQ, int count);
void ReleaseEqualizer(EQUALIZER_HANDLE *hEQ);

bool UpdateFilter(EQUALIZER_HANDLE *hEQ, float *gain, int count);

bool Equalizer(EQUALIZER_HANDLE *hEQ, short *sampleData, int totalSamples, int channels);


#endif //__EQUALIZER_H__
