#include "stdafx.h"
#include <math.h>
#include "equalizer.h"

bool InitEqualizer(EQUALIZER_HANDLE *hEQ, int count)
{
    hEQ->filter = new COMPLEX[count];
    if(hEQ->filter == NULL)
        return false;

    for(int i = 0; i < count; i++)
    {
        hEQ->filter[i].re = (float)1.0;
        hEQ->filter[i].im = (float)1.0;
    }

    bool bSuccess = InitFft(&hEQ->hfft, count, 1);
    if(!bSuccess)
    {
        delete[] hEQ->filter;
        hEQ->filter = NULL;
    }

    return bSuccess;
}

void ReleaseEqualizer(EQUALIZER_HANDLE *hEQ)
{
    if(hEQ != NULL)
    {
        ReleaseFft(&hEQ->hfft);
        if(hEQ->filter)
        {
            delete[] hEQ->filter;
        }
    }
}

bool UpdateFilter(EQUALIZER_HANDLE *hEQ, float *gain, int count)
{
    if((hEQ->hfft.count / 2) < count)
        return false;

    for(int i = 0; i < hEQ->hfft.count / 2; i++)
    {
        double dbk = pow(10.0, gain[i]/20.0);
        hEQ->filter[i].re = (float)(dbk * 0.75);
        hEQ->filter[i].im = (float)(dbk * 0.25);
        //hEQ->filter[i].re = (float)(pow(10.0, gain[i]/20.0));
        //hEQ->filter[i].im = (float)0.0;
        hEQ->filter[hEQ->hfft.count - 1 - i].re = hEQ->filter[i].re;
        hEQ->filter[hEQ->hfft.count - 1 - i].im = hEQ->filter[i].im;
    }

    IFFT(&hEQ->hfft, hEQ->filter); //to time-domain
    for(int i = 0; i < hEQ->hfft.count; i++)
    {
        hEQ->filter[i].im = (float)0.0;
    }
    FFT(&hEQ->hfft, hEQ->filter); //to freq-domain

    return true;
}

static void SampleDataToComplex(short *sampleData, int channels, COMPLEX *cdl, COMPLEX *cdr)
{
    if(channels == 1)
    {
        cdl->re = float(*sampleData / 32768.0);
        cdl->im = 0.0;
    }
    else
    {
        cdl->re = float(*sampleData / 32768.0);
        cdl->im = 0.0;
        cdr->re = float(*(sampleData + 1) / 32768.0);
        cdr->im = 0.0;
    }
}

static void ComplexToSampleData(COMPLEX *cdl, COMPLEX *cdr, int channels, short *sampleData)
{
    if(cdl->re > 1.0)
        cdl->re = 1.0;
    if(cdl->re < -1.0)
        cdl->re = -1.0;

    *sampleData = short(cdl->re * 32767.0);
    if(channels != 1)
    {
        if(cdr->re > 1.0)
            cdr->re = 1.0;
        if(cdr->re < -1.0)
            cdr->re = -1.0;
        *(sampleData + 1) = short(cdr->re * 32767.0);
    }
}

static void SampleDataMpGain(COMPLEX *cdl, COMPLEX *cdr, int count, int channels, COMPLEX *filter)
{
    for(int i = 0; i < count / 2; i++)
    {
        cdl[i] = cdl[i] * filter[i];
        cdl[count - 1 - i] = cdl[count - 1 - i] * filter[count - 1 - i];
        if(channels != 1)
        {
            cdr[i] = cdr[i] * filter[i];
            cdr[count - 1 - i] = cdr[count - 1 - i] * filter[count - 1 - i];
        }
    }
}

bool Equalizer(EQUALIZER_HANDLE *hEQ, short *sampleData, int totalSamples, int channels)
{
    int j;

    COMPLEX *leftData = new COMPLEX[hEQ->hfft.count];
    if(leftData == NULL)
        return false;
    COMPLEX *rightData = new COMPLEX[hEQ->hfft.count];
    if(rightData == NULL)
    {
        delete[] leftData;
        return false;
    }
    int procSamples = 0;
    short *procData = sampleData;
    while((totalSamples - procSamples) >= hEQ->hfft.count)
    {
        procData = sampleData + procSamples * channels;
        for(j = 0; j < hEQ->hfft.count; j++)
        {
            SampleDataToComplex(procData, channels, &leftData[j], &rightData[j]);
            procData += channels;
        }
        
        FFT(&hEQ->hfft, leftData);
        if(channels > 1)
        {
            FFT(&hEQ->hfft, rightData);
        }

        SampleDataMpGain(leftData, rightData, hEQ->hfft.count, channels, hEQ->filter);
        IFFT(&hEQ->hfft, leftData);
        if(channels > 1)
        {
            IFFT(&hEQ->hfft, rightData);
        }

        procData = sampleData + procSamples * channels;
        for(j = 0; j < hEQ->hfft.count; j++)
        {
            ComplexToSampleData(&leftData[j], &rightData[j], channels, procData);
            procData += channels;
        }

        procSamples += hEQ->hfft.count; /*每次向后移动一个窗口*/
    }

    delete[] leftData;
    delete[] rightData;

    return true;
}
