/*
 * show Chinese lunisolar calender date.
 * (c) 2011 vinoca <supoingo@163.com>
 * Licensed under the BSD
 */

#define VERSION	"1.0.5"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "chinese_calender.h"

/* 四柱、节气、节日名称 {{{ */
static const char *gan[] = {"甲", "乙", "丙", "丁", "戊",
	 			"己", "庚", "辛", "壬", "癸"};

static const char *zhi[] = {"子", "丑", "寅", "卯", "辰", "巳",
				 "午", "未", "申", "酉", "戌", "亥"};

static const char *sheng_xiao[] = {"鼠", "牛", "虎", "兔", "龍", "蛇",
					 "馬", "羊", "猴", "雞", "狗", "豬"};

static const char *jieqi_name[] = {
	 		"小寒", "大寒", "立春", "雨水", "惊蛰", "春分",
			"清明", "谷雨", "立夏", "小满", "芒种", "夏至",
			"小暑", "大暑", "立秋", "处暑", "白露", "秋分",
			"寒露", "霜降", "立冬", "小雪", "大雪", "冬至"};

static const char *yueri_name1[] = {"初", "十", "廿", "三", "大", "小",
	 					"閏",""};

static const char *yueri_name2[] = {"十", "一", "二", "三", "四", "五",
	 		"六", "七", "八", "九", "十", "十一", "腊", "正"};

static const char *weekday[] = {"一","二","三","四","五","六","日"};

/* 农历节日 */
static const int jieri_index[] = {
101, 115, 202, 505, 707, 715, 815, 909, 1208, 1223, 100};
static const char *jieri[] = {
"春節", "元宵", "龍抬頭", "端午", "七夕", "中元", "中秋",
"重陽", "臘八", "小年", "除夕"};


/* 公历节日 */
static const int festival_index[] = {
101, 214, 305, 308, 312, 315, 401, 501, 504, 601, 701, 707,
801, 910, 918, 1001, 1002, 1003, 1220, 1224, 1225};
static const char *festival[] = {
"元旦", "情人节", "学雷锋日", "妇女节", "植树节", "消费者权益日", "愚人节", "劳动节",
"青年节","儿童节", "香港回归纪念日, 中共诞辰", "抗日战争纪念日", "建军节",
"教师节", "九·一八事变纪念日", "国庆节", "国庆节假日", "国庆节假日",
"澳门回归纪念", "平安夜", "圣诞节"};
/* }}} */

static void show_help(void)
{
	printf(
	"  cdate，显示中国传统农历\n"
	"  用法：\n"
	"  cdate [-a | -g | -z | -n | -d date | date ]\n"
	"  选项：\n"
	"  -a    显示所有项\n"
	"  -g    显示公历年月日\n"
	"  -z    显示四柱\n"
	"  -n    不显示农历日历\n"
	"  -d    指定公历年月日时，格式0000年00月00日00时，\n"
	"        年份在1900与2050之间取值，如 cdate -d 2050010100 \n\n"
	"  -h    显示此帮助\n"
	"  版本："VERSION"\t反馈及疑问：www.vinoca.org\n\n"
);
	exit(1);
}

int main(int argc, char* argv[])
{
	struct c_calender d;
	struct tm *lt;
	time_t tp;

	int i,year,month,day,hour;
	int show_all = 0;
	int show_g_date = 0;
	int not_show_c_cal = 0;
	int show_sizhu = 0;
	int x = -1;

	for (i=1; i<argc && argv[i][0]=='-'; i++)
		switch(argv[i][1]) {
		case 'a': show_all = 1; break;
		case 'd':
			if (++i < argc && isdigit(*argv[i]))
				x = atoi(argv[i]);
			else show_help();
			break;
		case 'g': show_g_date = 1; break;
		case 'h': show_help(); break;
		case 'n': not_show_c_cal = 1; break;
		case 'z': show_sizhu = 1; break;
		default: show_help(); break;
		 }

	if (argc == 2 && argv[1][0] != '-') {
		if (isdigit(*argv[1]))
			x = atoi(argv[1]);
		else
			show_help();
	}

	if (x >= 0) {
		year = x / 1000000;
		month = (x % 1000000) / 10000;
		day = (x % 10000) / 100;
		hour = x % 100;
	} else {
		time(&tp);
		lt = localtime(&tp);
		year = 1900 + lt->tm_year;
		month = 1 + lt->tm_mon;
		day = lt->tm_mday;
		hour = lt->tm_hour;
	}

	if(chinese_calender(year, month, day, hour, &d)) show_help();

	 /* 显示公历 */
	 if (show_all || show_g_date) {
		x = month;
		i = year;

		if (month == 1 || month == 2) {
			x = month + 12;
			i = year - 1;
		}
		
		printf("%d年%d月%d日 星期%s\n",
			year,month,day, weekday[(day + 2 * x + 3 * 
			(x + 1) / 5 + i + i / 4 - i / 100 + i / 400) % 7]);
	 }
	
	 /* 显示农历年月日 */
	if (show_all || !not_show_c_cal) {

		printf("%s%s",
			gan[(d.sizhu >> 24) % 10],zhi[(d.sizhu >> 24) % 12]);

		printf("%s年%s",
			sheng_xiao[(d.sizhu >> 24) % 12],
			yueri_name1[(d.month_stat & 1)?6:7]);

		printf("%s月%s",
			yueri_name2[(d.month == 1)?d.month + 12:d.month],
			yueri_name1[(d.month_stat >> 1 & 1)?4:5]);

		printf("%s%s\t",
			yueri_name1[(d.day == 10)?0:d.day / 10],
			yueri_name2[d.day % 10]);

		/* 判断“除夕” */
		if (d.month == 12) {
			if ((d.month_stat >> 1 & 1) == 1 && d.day == 30)
				x = 100;
			else if ((d.month_stat & 1) == 0 && d.day == 29)
				x = 100;
			else
				x = d.month * 100 + d.day;
		} else
			x = d.month * 100 + d.day;

		/* 显示节气、节日 */
		if ((d.month_stat >> 8) < 24)
			printf("%s ",jieqi_name[d.month_stat >> 8]);

		for (i=0; i<=sizeof(jieri_index) / sizeof(int); i++)
			if (x == jieri_index[i]) printf("%s ",jieri[i]);

		for (i=0; i<=sizeof(festival_index) / sizeof(int); i++) {
			if ((month * 100 + day) == festival_index[i])
				printf("%s ",festival[i]);
		}
		printf("\n");
	}

	/* 显示四柱 */
	if (show_all || show_sizhu) {
		printf("%s%s年",
			gan[(d.sizhu >> 24) % 10],
			zhi[(d.sizhu >> 24) % 12]);

		printf("%s%s月",
			gan[((d.sizhu >> 16) & 255) % 10],
			zhi[((d.sizhu >> 16) & 255) % 12]);

		printf("%s%s日",
			gan[((d.sizhu >> 8) & 255) % 10],
			zhi[((d.sizhu >> 8) & 255) % 12]);

		printf("%s%s时\n",
			gan[(d.sizhu & 255) % 10],
			zhi[(d.sizhu & 255) % 12]);
	}

	exit(EXIT_SUCCESS);
}

