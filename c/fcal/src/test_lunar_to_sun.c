#include "test_help.h"

int main(int argc,char* argv[])
{
	if (argc < 4) {
		show_help(argv[0]);
	}
	LunarDate LD={
		.year=(U16)atoi(argv[1]),
		.month=(U8)atoi(argv[2]) - 1,
		.day=(U8)atoi(argv[3])
	};
	SunDate sunDate;
	BL* bl=getLunarBLArray(LD.year);
	int ret;	
	U8 tmp=lunarLeapMonth(LD.year);
	ret = lunarToSun(&LD,&sunDate,bl,tmp);
	if(ret){
		fprintf(stderr,"转换失败\n");
		return 1;
	}
	printf("对应的日期是%u-%u-%u\n",sunDate.year,sunDate.month,sunDate.day);
	freeBLArray(bl);
	return 0;
}
