/*
 *   PCM time-domain equalizer
 *
 *   Copyright (C) 2002  Felipe Rivera <liebremx at users.sourceforge.net>
 *
 *   19.08.2002 Initial release for XMMS
 *
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
 *   $Id: eq.h,v 1.6 2006/01/15 00:16:08 liebremx Exp $
 */
#ifndef EQ_H
#define EQ_H
void   init_equliazer(int nBand = 0 );
void   uninit_equliazer() ; 
void   set_eq_value(float value, int index, int chn);
int    do_equliazer(short * d, int length, int srate, int nch);
int    get_eq_band_count();
float  get_eq_value(int index , int chn);

#endif
