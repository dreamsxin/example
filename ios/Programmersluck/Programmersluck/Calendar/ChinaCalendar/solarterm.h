//
//  solarterm.h
//  ChineseCalendar
//
//  Created by Tony Li on 6/4/12.
//  Copyright (c) 2012 Tony Li. All rights reserved.
//

/**
 @see https://github.com/autopear/LunarCalendar
 */

#ifndef ChineseCalendar_solarterm_h
#define ChineseCalendar_solarterm_h

int solarterm_index(int year, int month, int day);

const char *solarterm_name(const int index);

#endif
