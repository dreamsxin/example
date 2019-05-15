#include "fdatefunc.h"
/* 获取BL结构体
 * 得到一年的大小月数据
 */
BL* getLunarBLArray(U16 year)
{
	U8 arraySize=lunarMonths(year);
	U8 i;
	BL* bl=(BL*)malloc(sizeof(BL));
	if(!bl)
		return NULL;
	U8 *array=(U8*)malloc(sizeof(U8)*arraySize);
	if(!array)
		return NULL;
	short BLData=lunarBLMonthData(year);
	for(i=0;i<arraySize;++i){
		array[i]=((BLData>>(arraySize-i-1))&1)+29;
	}
	bl->array=array;
	bl->size=arraySize;
	return bl;
}

/* 释放BL结构体空间
*/
void freeBLArray(BL* bl)
{
	if(bl){
		free(bl->array);
		bl->size=0;
		free(bl);
		bl=NULL;
	}
}


/* 1900-1-31到2099-12-31为有效时间
*/
int sunToLunar(LunarDate* pLD,SunDate* pSD,BL* pBL,U8 leapMonth)
{
	// 检测数据指针的有效性
	if(!pLD || !pBL || !pSD)
		return -1;
	// 检测数据的有效性,只要检测下限就可以了
	if((pSD->year==1900 && pSD->month==1 && pSD->day < 31))
		return -1;
	// 进行必要的数据转化，使得LunurDate 中保存的是春节的日期 
	pLD->year=pSD->year;
	pLD->month=lunarSpringInSunMonth(pSD->year);
	pLD->day=lunarSpringInSunDay(pSD->year);
	short offset=daysOfSunYear(*pSD)-daysOfSunYear(*pLD);

	short countDays=0;
	U8 i=0;
	if(offset<0){
		--pLD->year;	
		countDays=offset+(lunarData(pLD->year)&1?30:29);
		pLD->month=lunarMonths(pLD->year)-(countDays<0)-(lunarLeapMonthIndex(pLD->year)>0);
		pLD->day=((countDays<0)?(lunarData(pLD->year)&2?30:29):0)+countDays+1;
	}else{
		for(i=0;i<pBL->size;++i){
			countDays+=pBL->array[i];
			if(countDays>offset)
				break;
		}
		pLD->day=offset+pBL->array[i]-countDays+1;
		pLD->month=i+(leapMonth==0);
	}
	return 0;
}

/* 1900-1-31 to 2099-12-31为有农历数据的日期
 * 1900-0-1 to 2099-11-20 为阳历有效范围对应的有效农历范围
 * 农历转化成太阳历
 */
int lunarToSun(LunarDate* pLD,SunDate* pSD,BL* pBL,U8 leapMonth)
{
	// 异常处理
	if(!pLD || !pBL || !pSD)
		return -1;
	if((pLD->year==2099 && pLD->month==11 && pLD->day > 20))
		return -1;
	// 初始化pSD
	pSD->year=pLD->year;
	pSD->month=lunarSpringInSunMonth(pSD->year);
	pSD->day=lunarSpringInSunDay(pSD->year);

	short offset=0;
	short countDays=0;
	U8 i;
	// 获取明年的阳历1月1是哪天,就是用今年的新年的偏移天数和，今天的农历和新年的
	// 差值求和，算得和明年阳历第一天的插值，利用这个插值来进行计算
	for(i=0;i<pLD->month;++i){
		countDays += pBL->array[i];
	}
	countDays = countDays + pLD->day + daysOfSunYear(*pSD) - 1;

	offset=countDays - 365 - isLeapYear(pSD->year);
	// 根据相对偏移量来计算年月日
	if(offset>0){
		//说明是阳历年中的明年的1，2月份中的日子
		pSD->month=(offset>31)?2:1;
		pSD->day=offset-(offset>31 ? 31 : 0);
		++pSD->year;
	}else{
		//说明是今年中的某一天,需要考虑上闰年
		if(countDays>28 && isLeapYear(pSD->year))
			--countDays;
		for(i=0;i<12 && sunMonthDayOffset[i]<countDays;++i);
		pSD->month=i;
		pSD->day=countDays-sunMonthDayOffset[i-1];
	}
	return 0;
}

int starIdxInSun(SunDate* pDate)
{
	if (pDate) {
		int leap = isLeapYear(pDate->year);
		int offsetDay = daysOfSunYear(*pDate) % (starEndOffsetDays[leap][11] + 1);
		int idx = 6;
		if (offsetDay >= starEndOffsetDays[leap][idx]) {
			while(offsetDay > starEndOffsetDays[leap][idx])
				++idx;
		} else {
			idx = 0;
			while(offsetDay > starEndOffsetDays[leap][idx])
				++idx;
		}
		return idx;
	}
	return -1;
}
