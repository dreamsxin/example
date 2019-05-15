#include <string.h>
#include "test_help.h"


int main(int argc,const char* argv[])
{

	SunDate sunDate = check_arg_get_date(argc, argv);
	int tmp;
	LunarDate* pLD=(LunarDate*)malloc(sizeof(LunarDate));
	BL* bl;
	printf("%d-%d-%d\t",sunDate.year,sunDate.month,sunDate.day);
	printf("%s%s年[%s] ",yearToNature(sunDate),yearToEarth(sunDate),yearToAnimal(sunDate));

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

do_failed:
	free(pLD);
	pLD=NULL;
	return 0;
}
