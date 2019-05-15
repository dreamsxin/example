#ifndef __FFF_H__
#define __FFF_H__

#define PI (double)3.14159265359
#define M_PI (float)3.14159265359

/*complex number*/
struct COMPLEX
{
    COMPLEX()
    {
        re = 0.0;
        im = 0.0;
    }
    COMPLEX(float _re, float _im)
    {
        re = _re;
        im = _im;
    }
    friend COMPLEX operator+ (COMPLEX& c1, COMPLEX& c2 )
	{
        COMPLEX c;
        c.re = c1.re + c2.re;
        c.im = c1.im + c2.im;
        return c;
	}
    friend COMPLEX operator- (COMPLEX& c1, COMPLEX& c2 )
	{
        COMPLEX c;
        c.re = c1.re - c2.re;
        c.im = c1.im - c2.im;
        return c;
	}
    friend COMPLEX operator* (COMPLEX& c1, COMPLEX& c2 )
	{
        COMPLEX c;
        c.re = c1.re * c2.re - c1.im * c2.im;
        c.im = c1.re * c2.im + c2.re * c1.im;
        return c;
	}
    friend COMPLEX operator/ (COMPLEX& c1, COMPLEX& c2 )
	{
        float fm = c2.re * c2.re + c2.im * c2.im;
        COMPLEX c;
        c.re = (c1.re * c2.re + c1.im * c2.im) / fm; 
        c.im = (c1.im * c2.re - c1.re * c2.im) / fm; 
        return c;
	}
    float re;
    float im;
};

typedef struct tagFFT_HANDLE
{
    int count;
    float *win;
    COMPLEX *wt;
}FFT_HANDLE;

bool InitFft(FFT_HANDLE *hfft, int count, int window);
void ReleaseFft(FFT_HANDLE *hfft);
void FftWindowFunction(FFT_HANDLE *hfft, COMPLEX *TF);

void FFT(FFT_HANDLE *hfft, COMPLEX *TD2FD);
void FFT2(FFT_HANDLE *hfft, COMPLEX *TD2FD);
void IFFT(FFT_HANDLE *hfft, COMPLEX *FD2TD);
void IFFT2(FFT_HANDLE *hfft, COMPLEX *FD2TD);


#endif //__FFF_H__
