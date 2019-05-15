#ifndef __FTYPES_H_
#define __FTYPES_H_

/*这个文件中定义了一些公共的数据结构和数据类型
 */

/*基本数据类型
 */
#define U8 unsigned char
#define  U16 unsigned short
#define U32 unsigned int

/*表示日期结构体
 */
struct FDate{
	U16 year;
	U8 month;
	U8 day;
};

typedef struct FDate SunDate;
typedef struct FDate LunarDate;

#endif //__FTYPES_H_
