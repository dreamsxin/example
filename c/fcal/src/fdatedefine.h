#ifndef __FDATE_DEFINE_H_
#define __FDATE_DEFINE_H_

#include "fdate.h"


/*这个文件中包含了一些用来操作日期的基本方法
 */

/* 太阳历中平润年的判断
 * 是润年返回1，
 * 不是返回0
 */
#define isLeapYear(year) (!((year)%400) || (!((year)%4) && ((year)%100)))

/* 获取太阳历中某年某月有多少天
 * 返回28-31
 */
#define daysOfSunMonth(sunDate) ((isLeapYear((sunDate).year) && ((sunDate).month==2)) ? 29 : sunMonthDays[(sunDate).month-1])

/* 获取太阳历中某年某月某天是今年的第几天
 * 返回1-366
 */
#define daysOfSunYear(sunDate) (sunMonthDayOffset[(sunDate).month-1]+(sunDate).day+(isLeapYear((sunDate).year) && ((sunDate).month > 2)))


/* 获取太阳历中，1900年以后的某年某月某天是周几
 * 先计算与1900-1-0(周0)的相对积日
 * 只适用于1900-2099年间
 * 0-6
 */ 

#define year4Quotient(sunDate) (((sunDate).year-1900-((sunDate).year-1900)%4)>>2)//年份除以4的商
#define year4Remainder(sunDate) ((sunDate).year%4)//年份除以4的余数
/* 计算积日
 * 这个是利用公积日算的星期，效能就没有下面的好
 * 当然，公积日有他的作用,在此不编译这个宏
 */
#if 0
#define D(sunDate) (year4Quotient(sunDate)*1461+year4Remainder(sunDate)*365-isLeapYear((sunDate).year))//公积日的计算

#define dayInWeek(sunDate) ((daysOfSunYear(sunDate)+D(sunDate))%7)
#endif

/* 这个跟上面利用公积日算的思想是一样的
 * 优点就是缩小了运算的数值
 * 减少了一次乘法运算
 * 节省了内存空间
 */
#define dayInWeek(sunDate) ((5*year4Quotient(sunDate)+daysOfSunYear(sunDate)+year4Remainder(sunDate)-isLeapYear((sunDate).year))%7)

/*获取中文的星期名
 */
#define dayNameInWeek(sunDate) sunWeekName[dayInWeek(sunDate)]

/*获取中文月份名
 */
#define monthNameInYear(month) lunarMonthName[(month)%12]

/* 获取农历天数名
 */
#define dayNameInMonth(day) lunarDayName[(day)%30]


/* 使用寿星公式来对节气进行计算
 * [Y*D+C]-L
 * Y  年份的后两位
 * D  在这里是0.2422
 * C  一个常量
 * 使用他们3个计算后取整
 * L  本世纪开始到今年的润年数
 */

/* 有些年份中的节气比较特殊，单独弄出来
 * 如果这样的月份满足incSolarDay的条件
 * 将会返回1
 * 如果满足decSolarDay的条件
 * 将会返回-1
 * 虽然这个方法看起来很笨拙，但是我不用为此写一个
 * C文件，再去使用循环遍历查找，这样，方法就统一的
 * 使用宏了
 */
#define incSolarDay(year,month,which) (\
	((year)==2016 && (month)==7 && !(which)) ||\
	((year)==2008 && (month)==5 && (which)) ||\
	((year)==2002 && (month)==8 && !(which)) ||\
	((year)==1982 && (month)==1 && !(which)) ||\
	((year)==1978 && (month)==11 && (which)) ||\
	((year)==1954 && (month)==12 && !(which)) ||\
	((year)==1942 && (month)==9 && (which)) ||\
	((year)==1927 && (month)==9 && !(which)) ||\
	((year)==1925 && (month)==7 && !(which)) ||\
	((year)==1928 && (month)==6 && (which)) ||\
	((year)==2082 && (month)==1 && (which)) ||\
	((year)==2084 && (month)==3 && (which)) ||\
	((year)==1911 && (month)==5 && !(which)) ||\
	((year)==1902 && (month)==6 && !(which)) ||\
	((year)==1922 && (month)==7 && (which)) ||\
	((year)==2089 && (month)==10 && (which)) ||\
	((year)==2089 && (month)==11 && !(which))\
)
#define	decSolarDay(year,month,which) (\
	((year)==2019 && (month)==1 && !(which)) ||\
	((year)==2021 && (month)==12 && (which)) ||\
	((year)==2026 && (month)==2 && (which)) ||\
	((year)==1918 && (month)==12 && (which))\
)
/* 获取年份的低2个权位的值
 */
#define getYearLow2(year) ((year)-((year)>=2000?2000:1900))
/* 使用寿星公式计算出大部分正确的时间
 */
#define oldStar(year,month,which) (((int)(getYearLow2(year)*0.2422+C[(year)>=2000][(month)%12][(which)]))-((int)((getYearLow2(year)-(isLeapYear(year)&&(((month)%12)<3)))/4)))

/* 用来计算节气所在的那个月中的日子
 * 结果是一个整数
 * 需要传递一个年份，一个月份，和这个月份的第几个节气
 * 第几个节气需要传第的是0,1
 * 计算节气在那个月里的正确时间
 */
#define solarTermDayInMonth(year,month,which) (oldStar(year,month,which)+((incSolarDay(year,month,which))?1:(-decSolarDay(year,month,which))))

/* 获得节气的中文名字
 */
#define dayNameInSolarTerms(month,which) solarTerm[(month)%12][(which)]

/* 解析农历数据的一些宏
 */
#define lunarData(year) lunarData[(year)-1900]//根据年份获取农历数据

#define lunarSpringInSunMonth(year) (lunarData(year)>>28)//根据年份获取农历正月初一所在的太阳历月份

#define lunarSpringInSunDay(year) (((lunarData(year)>>20)&0xf)+((lunarData(year)>>24)&0xf)*10)//获取农历正月初一所在的太阳历月份中的具体日子

/* 获取润月的索引值
 * 这个索引值为0表示没有润月
 * 不是零的时候，表示的是大小月数据的索引。
 */
#define lunarLeapMonthIndex(year) (((lunarData(year)>>16) & 0xf))

#define lunarLeapMonth(year) (lunarLeapMonthIndex(year)?(lunarLeapMonthIndex(year)-1):0)//获取润几月

#define lunarBLMonthData(year) (lunarData(year)&0x1fff)//提取出大小月数据的低13位

#define lunarMonths(year) (12+(lunarLeapMonthIndex(year)>0))//返回这一年有多少个农历月

/* 太阳历转换成农历的年份
 */

/* 判断是否减去一年
 * 好多网站上以立春作为岁首
 * 看到了关于岁首的辩论、
 * 我决定不这样做。就以春节为分割
 */
#define isMinus(sunDate) ( ((sunDate).month <= lunarSpringInSunMonth((sunDate).year)) && ((sunDate).day < lunarSpringInSunDay((sunDate).year)) )
/*获取天干
 */
#define yearToNature(sunDate)	nature[(((sunDate).year-3-isMinus(sunDate))%10)]

#define EA(sunDate) ((((sunDate).year)-3-isMinus(sunDate))%12)
/*获取地支
 */
#define yearToEarth(sunDate) earth[EA(sunDate)]
/*获取属相
 */
#define yearToAnimal(sunDate) animal[EA(sunDate)]

#endif	//__FDATE_FUNC_H_
