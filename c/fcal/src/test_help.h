#ifndef __TEST_HELP_H_
#define __TEST_HELP_H_
#include <stdio.h>
#include <time.h>
#include "fdatefunc.h"

void show_help(const char* name) 
{
	printf("\nUsage:\n\t");
	printf("%s year month day\n\t", name);
	printf("defalut show now date info.\n");
	exit(0);
}

SunDate check_arg_get_date(const int argc, const char* argv[])
{
	SunDate sunDate;
	if (argc < 2) {
		time_t now_time;
		time(&now_time);
		struct tm* now = localtime(&now_time);
		if (now) {
			sunDate.year=(U16)(now->tm_year + 1900);
			sunDate.month=(U8)(now->tm_mon + 1);
			sunDate.day=(U8)now->tm_mday;
		}
	} else if (4 == argc) {
		sunDate.year=(U16)atoi(argv[1]);
		sunDate.month=(U8)atoi(argv[2]);
		sunDate.day=(U8)atoi(argv[3]);
	} else {
		show_help(argv[0]);
	}
	return sunDate;
}


#endif /* end of include guard: __TEST_HELP_H_ */
