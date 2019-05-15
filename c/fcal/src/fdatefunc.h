#ifndef __FDATE_FUNC_H_
#define __FDATE_FUNC_H_

#include <stdlib.h>
#include "fdatedefine.h"

/* BL结构体
 */
typedef struct BL{
	U8* array;//指向存放了一年大小月数据的数组指针
	U8 size;//数据数组的大小
}BL;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/* 传入年份，将返回这个年份相关的BL结构体指针
 */
BL* getLunarBLArray(U16 year);

/* 使用完成BL后，需要用此方法释放结构体
 */
void freeBLArray(BL* bl);

/* 经过测试
 * 这两百年中不会出现前一年的润月是今年的某一个太阳历中的一天
 * 就是说，今年的1月1号顶多是前一年农历十一月中的某一天
 * 这就是下面的计算方法的参考思想
 */

/* 对数据进行转化
 * 太阳历向农历的转化，这个时候，月份就是月份值
 * SunDate结构体指针中保存了转化源，LunarDate是目标
 * 转化后的结果将保存在LunarDate结构体指针指向的结构体中
 * 转化成功返回0，失败返回-1
 */
int sunToLunar(LunarDate* pLD,SunDate* pSD,BL* pBL,U8 leapMonth);

/* 对数据进行转化
 * 农历向太阳历转化，这个时候，农历中存的是BL结构体中的月份索引
 * LunarDate结构体指针中保存了转化源，SunDate是目标
 * 转化后的结果将保存在SunDate结构体指针指向的结构体中
 * 转化成功返回0，失败返回-1
 */
int lunarToSun(LunarDate* pLD,SunDate* pSD,BL* pBL,U8 leapMonth);
/*上面的操作都将跟新offset，和isSetted的值
 */

/**
 * 获取星座 
 * 看上去夏末秋初人比较多，所以用7月份的星座做计算的分割
 * 返回-1表示失败
 */
int starIdxInSun(SunDate* pDate);
#define starNameInSun(date) (\
{\
	int idx = starIdxInSun(date);\
	(idx != (-1)) ? starName[idx] : "X X 座";\
})

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif//__cplusplus

#endif	//__FDATE_FUNC_H_
