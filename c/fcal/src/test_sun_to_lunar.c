#include "test_help.h"

int main(int argc,const char* argv[])
{
	SunDate sunDate= check_arg_get_date(argc, argv);
	int tmp;
	LunarDate* pLD=(LunarDate*)malloc(sizeof(LunarDate));
	BL* bl;
	printf("今天是%d-%d-%d\t",sunDate.year,sunDate.month,sunDate.day);
	printf("%s%s年[%s]\t",yearToNature(sunDate),yearToEarth(sunDate),yearToAnimal(sunDate));

	tmp=lunarLeapMonth(sunDate.year);
	bl=getLunarBLArray(sunDate.year);
	int ret;
	ret=sunToLunar(pLD,&sunDate,bl,tmp);
	if(ret<0){
		printf("转化失败\n");
		goto do_failed;
	}
	if(tmp){
		if(pLD->month<tmp){
			printf("%s月",monthNameInYear(pLD->month+1));
		}else if(pLD->month>tmp){
			printf("%s月",monthNameInYear(pLD->month));
		}else
			printf("[润]%s月",monthNameInYear(tmp));
	}
	else{
		printf("%s月",monthNameInYear(pLD->month));
	}
	printf("%s\n",dayNameInMonth(pLD->day));

	printf("今年的第%d天,%s月有%d天,今天周%s\n",daysOfSunYear(sunDate),monthNameInYear(sunDate.month),daysOfSunMonth(sunDate),dayNameInWeek(sunDate));

	printf("今天出生的人的星座是：%s\n", starNameInSun(&sunDate) );

	printf("今年的春节是%d月%d号\n",lunarSpringInSunMonth(sunDate.year),lunarSpringInSunDay(sunDate.year));
	printf("这个月的第一个节气是：%s,日期是：%d-%d\n",dayNameInSolarTerms(sunDate.month,0),sunDate.month,solarTermDayInMonth(sunDate.year,sunDate.month,0));
	printf("这个月的第二个节气是：%s,日期是：%d-%d\n",dayNameInSolarTerms(sunDate.month,1),sunDate.month,solarTermDayInMonth(sunDate.year,sunDate.month,1));
	int i;
	printf("大小月的排布情况是:\n");
	if(tmp){
		for(i=0;i<tmp;++i){
			printf("%d\t%s月\n",bl->array[i],monthNameInYear(i+1));
		}
		printf("%d\t润%s月\n",bl->array[tmp],monthNameInYear(i));
		for(;i<bl->size;++i){
			printf("%d\t%s月\n",bl->array[i],monthNameInYear(i));
		}
	}
	else{
		for(i=0;i<bl->size;++i){
			printf("%d\t%s月\n",bl->array[i],monthNameInYear(i+1));
		}
	}
	printf("----------------------------------------------\n");
do_failed:
	free(pLD);
	pLD=NULL;
	freeBLArray(bl);
	return 0;
}
