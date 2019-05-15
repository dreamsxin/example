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
 *   $Id: eq.c,v 1.12 2006/01/15 00:13:49 liebremx Exp $
 */
#include "stdafx.h"
#include "iir.h"
#include <stdio.h>
#include <string.h>

#include <math.h>
#include "eq.h"

static bool on = false;
void   init_equliazer(int nBand)
{
	if(nBand != 0 && nBand < EQ_MAX_BANDS)
	{
		//nBand = nBand;
	}
	else
	{
		nBand = 10;
	}
    init_iir(nBand);
}

void   uninit_equliazer()
{
	clean_history();
}


void set_eq_value(float value, int index, int chn)
{
    /* Map the gain and preamp values */
    if (index >= 0)
	{
        set_gain(index, chn, 2.5220207857061455181125E-01 * exp(8.0178361802353992349168E-02 * value) - 2.5220207852836562523180E-01 , value);
	}
    else
	{
        /* -12dB .. 12dB mapping */
        set_preamp(chn, 9.9999946497217584440165E-01 * exp(6.9314738656671842642609E-02 * value) + 3.7119444716771825623636E-07);
	}
}

/* 
 * The main stuff
 */
int do_equliazer(short * d, int length, int srate, int nch)
{
    return iir(d, length, srate, nch);
}
