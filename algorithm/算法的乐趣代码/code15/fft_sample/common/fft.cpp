#include "stdafx.h"
#include "fft.h"
#include <math.h>


/*copy from Audacity project*/
int NumberOfBits(int PowerOfTwo)
{
   for (int i = 0;; i++)
      if (PowerOfTwo & (1 << i))
         return i;
}

/*
window = 1 -> hanning window
*/
bool InitFft(FFT_HANDLE *hfft, int count, int window)
{
    int i;

    hfft->count = count;
    hfft->win = new float[count];
    if(hfft->win == NULL)
    {
        return false;
    }
    hfft->wt = new COMPLEX[count];
    if(hfft->wt == NULL)
    {
        delete[] hfft->win;
        return false;
    }
    for(i = 0; i < count; i++)
    {
        hfft->win[i] = float(0.50 - 0.50 * cos(2 * M_PI * i / (count - 1)));
    }
    for(i = 0; i < count; i++)
    {
        float angle = -i * M_PI * 2 / count;
        hfft->wt[i].re = cos(angle);
        hfft->wt[i].im = sin(angle);
    }

    return true;
}

void ReleaseFft(FFT_HANDLE *hfft)
{
    if(hfft != NULL)
    {
        hfft->count = 0;
        if(hfft->win != NULL)
        {
            delete[] hfft->win;
            hfft->win = NULL;
        }
        if(hfft->wt != NULL)
        {
            delete[] hfft->wt;
            hfft->wt = NULL;
        }
    }
}

void FftWindowFunction(FFT_HANDLE *hfft, COMPLEX *TF)
{
    for(int i = 0; i < hfft->count; i++)
    {
        TF[i].re = TF[i].re * hfft->win[i];
    }
}

int BitReverise(int s, int fu) 
{
	int s1 = s;
	int k = 0;
	for (int i = 1; i <= fu; i++) 
    {
		int s2 = s1 >> 1;
		k = ((k << 1) + s1) - (s2 << 1);
		s1 = s2;
	}

	return k;
}

//////////////////////////////////////////////////////////

void FFT(FFT_HANDLE *hfft, COMPLEX *TD2FD)
{
    int i,j,k,butterfly,p;

    int power = NumberOfBits(hfft->count);

    /*蝶形运算*/
    for(k = 0; k < power; k++)
    {
        for(j = 0; j < 1<<k; j++)
        {
            butterfly = 1 << (power-k);
            p = j * butterfly;
            int s = p + butterfly / 2;
            for(i = 0; i < butterfly/2; i++)
            {
                COMPLEX t = TD2FD[i + p] + TD2FD[i + s];
                TD2FD[i + s] = (TD2FD[i + p] - TD2FD[i + s]) * hfft->wt[i*(1<<k)];
                TD2FD[i + p] = t;
            }
        }
    }

    /*重新排序*/
	for (k = 0; k < hfft->count; k++) 
    {
		int r = BitReverise(k, power);
		if (r > k) 
        {
            COMPLEX t = TD2FD[k];
            TD2FD[k] = TD2FD[r];
            TD2FD[r] = t;
		}
	}
}

void IFFT(FFT_HANDLE *hfft, COMPLEX * FD2TD)
{
    int i,j,k,butterfly,p;

    int power = NumberOfBits(hfft->count);

    for(k = 0; k < hfft->count; k++)
        FD2TD[k] = FD2TD[k] / COMPLEX(hfft->count, 0.0);
    
    /*蝶形运算*/
    for(k = 0; k < power; k++)
    {
        for(j = 0; j < 1<<k; j++)
        {
            butterfly = 1 << (power-k);
            p = j * butterfly;
            int s = p + butterfly / 2;
            for(i = 0; i < butterfly/2; i++)
            {
                COMPLEX t = FD2TD[i + p] + FD2TD[i + s];
                FD2TD[i + s] = (FD2TD[i + p] - FD2TD[i + s]) * COMPLEX(hfft->wt[i*(1<<k)].re, -hfft->wt[i*(1<<k)].im);
                FD2TD[i + p] = t;
            }
        }
    }
	/*----按照倒位序重新排列变换后信号----*/
	for (k = 0; k < hfft->count; k++) 
    {
		int r = BitReverise(k, power);
		if (r > k) 
        {
            COMPLEX t = FD2TD[k];
            FD2TD[k] = FD2TD[r];
            FD2TD[r] = t;
		}
	}
}
