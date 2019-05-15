/*
 *   PCM time-domain equalizer
 *
 *   Copyright (C) 2002-2006  Felipe Rivera <liebremx at users.sourceforge.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: iir.h,v 1.13 2006/01/15 00:26:32 liebremx Exp $
 */
#ifndef IIR_H
#define IIR_H

#include "iir_cfs.h"

/*
 * Flush-to-zero to avoid flooding the CPU with underflow exceptions 
 */
#ifdef SSE_MATH
#define FTZ 0x8000
#define FTZ_ON { \
    unsigned int mxcsr; \
  __asm__  __volatile__ ("stmxcsr %0" : "=m" (*&mxcsr)); \
  mxcsr |= FTZ; \
  __asm__  __volatile__ ("ldmxcsr %0" : : "m" (*&mxcsr)); \
}
#define FTZ_OFF { \
    unsigned int mxcsr; \
  __asm__  __volatile__ ("stmxcsr %0" : "=m" (*&mxcsr)); \
  mxcsr &= ~FTZ; \
  __asm__  __volatile__ ("ldmxcsr %0" : : "m" (*&mxcsr)); \
}
#else
#define FTZ_ON
#define FTZ_OFF
#endif



typedef double sample_t;
typedef sample_t level_t;

/* Max bands supported by the code
FIXME: 31 bands processing IS HARD ON THE PROCESSOR
On a PIII@900Mhz I got ~45% of CPU usage
Have to improve the filter implementation
Got it down to ~20% for 31 Bands
Still MMX/3DNow!/SSE whatever, can bring it down */
#define EQ_MAX_BANDS 32
/* Number of channels (Stereo) */
#define EQ_CHANNELS 2

#define  EXTRA_FILTER true

/*
 * Function prototypes
 */
void init_iir(int nBand);
void clean_history();
void set_gain(int index, int chn, double val , float rawValue);
void set_preamp(int chn, double val);
int  iir(short * d, int length, int srate, int nch);
//extern sIIRCoefficients *iir_cf;




#endif /* #define IIR_H */

