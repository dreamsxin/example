#include "stdafx.h"
#include <math.h>
#include "spectrum.h"

static void SampleDataToComplex(short *sampleData, int channels, COMPLEX *cd)
{
    if(channels == 1)
    {
        cd->re = float(*sampleData / 32768.0);
        cd->im = 0.0;
    }
    else
    {
        cd->re = float(*sampleData + *(sampleData + 1) / 65536.0);
        cd->im = 0.0;
    }
}

bool PowerSpectrumT(FFT_HANDLE *hfft, short *sampleData, int totalSamples, int channels, float *power)
{
    int i,j;

    for(i = 0; i < hfft->count; i++)
        power[i] = (float)0.0;

    COMPLEX *inData = new COMPLEX[hfft->count];
    if(inData == NULL)
        return false;

    int procSamples = 0;
    short *procData = sampleData;
    while((totalSamples - procSamples) >= hfft->count)
    {
        procData = sampleData + procSamples * channels;
        for(j = 0; j < hfft->count; j++)
        {
            SampleDataToComplex(procData, channels, &inData[j]);
            procData += channels;
        }
        procSamples += (hfft->count / 2); /*每次向后移动半个窗口*/
        //procSamples += hfft->count; /*每次向后移动一个窗口*/
        
        FftWindowFunction(hfft, inData);
        FFT(hfft, inData);

        for(i = 0; i < hfft->count; i++)
        {
            power[i] += float(20.0 * log10(sqrt(inData[i].re * inData[i].re + inData[i].im * inData[i].im) / (hfft->count / 2)));
        }
    }

    delete[] inData;

    return true;
}

bool PowerSpectrumS(FFT_HANDLE *hfft, short *sampleData, int totalSamples, int channels, float *power)
{
    int i,j;

    for(i = 0; i < hfft->count; i++)
        power[i] = (float)0.0;

    COMPLEX *inData = new COMPLEX[hfft->count];
    if(inData == NULL)
        return false;

    int procSamples = 0;
    short *procData = sampleData;
    while((totalSamples - procSamples) >= hfft->count)
    {
        procData = sampleData + procSamples * channels;
        for(j = 0; j < hfft->count; j++)
        {
            SampleDataToComplex(procData, channels, &inData[j]);
            procData += channels;
        }
        //procSamples += (hfft->count / 2); /*每次向后移动半个窗口*/
        procSamples += hfft->count; /*每次向后移动一个窗口*/
        
        //FftWindowFunction(hfft, inData);
        FFT(hfft, inData);

        for(i = 0; i < hfft->count; i++)
        {
            //power[i] += float(sqrt(inData[i].re * inData[i].re + inData[i].im * inData[i].im));
            power[i] += float(20.0 * log10(sqrt(inData[i].re * inData[i].re + inData[i].im * inData[i].im) / (hfft->count / 2)));
        }
    }

    delete[] inData;

    return true;
}
