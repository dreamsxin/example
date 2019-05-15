/*
 * - chinese_calender.h
 * get Chinese lunisolar calender.
 *
 * (c) 2011  vinoca  <supoingo@163.com>
 * Licensed under the BSD
 *
 * use the chinese_calender function to get chinese calender data,
 * if year < 1901 or year > 2050, chinese_calender return 0.
 */

/* c_calender : 包含农历年、月、日、节气、四柱 {{{ */
struct c_calender { int year, month, day, month_stat, sizhu;};
/* 
 * c_calender 结构说明： 
 *
 * year, month, day 农历年月日
 *
 * +-------------------------------------------+
 * |month_stat|   1   |   2    | 4-7 |   8-16  |
 * |----------+-------+--------+-----+---------|
 * |  说  明  |  闰月 | 大小月 |  空 | 节气序号|
 * +-------------------------------------------+
 * 注： 节气序号为0-23，若当日无节气，则序号为24。
 *
 * +---------------------------------------+
 * | sizhu |  1-7  |  8-15 | 16-23 | 24-31 |
 * |-------+-------+-------+-------+-------|
 * | 说 明 | 时 柱 | 日 柱 | 月 柱 | 年 柱 |
 * +---------------------------------------+
 * 注： 各柱为小于60的整数，除以10得到天干，除以12得到地支。
 * }}} */

/* 日期数据 {{{ */

/* 1900-2051年农历数据 */
static const unsigned int calendar_data[] = {

/* calendar_data format:
 * +------------------------------------------------------------------+
 * |单位(位)|   1-4   |     5-16     |     17     |  18-20  |  21-31  |
 * |--------+---------+--------------+------------+---------+---------|
 * | 说  明 |闰月月份 | 每月的大小月 |闰月的大小月|    空   |    空   |
 * +------------------------------------------------------------------+
 */

0x04bd8, 0x04ae0, 0x0a570, 0x054d5, 0x0d260, 0x0d950, 0x16554, 0x056a0, 0x09ad0, 0x055d2, 
0x04ae0, 0x0a5b6, 0x0a4d0, 0x0d250, 0x1d255, 0x0b540, 0x0d6a0, 0x0ada2, 0x095b0, 0x14977, 
0x04970, 0x0a4b0, 0x0b4b5, 0x06a50, 0x06d40, 0x1ab54, 0x02b60, 0x09570, 0x052f2, 0x04970, 
0x06566, 0x0d4a0, 0x0ea50, 0x06e95, 0x05ad0, 0x02b60, 0x186e3, 0x092e0, 0x1c8d7, 0x0c950, 
0x0d4a0, 0x1d8a6, 0x0b550, 0x056a0, 0x1a5b4, 0x025d0, 0x092d0, 0x0d2b2, 0x0a950, 0x0b557, 
0x06ca0, 0x0b550, 0x15355, 0x04da0, 0x0a5b0, 0x14573, 0x052b0, 0x0a9a8, 0x0e950, 0x06aa0, 
0x0aea6, 0x0ab50, 0x04b60, 0x0aae4, 0x0a570, 0x05260, 0x0f263, 0x0d950, 0x05b57, 0x056a0, 
0x096d0, 0x04dd5, 0x04ad0, 0x0a4d0, 0x0d4d4, 0x0d250, 0x0d558, 0x0b540, 0x0b6a0, 0x195a6, 
0x095b0, 0x049b0, 0x0a974, 0x0a4b0, 0x0b27a, 0x06a50, 0x06d40, 0x0af46, 0x0ab60, 0x09570, 
0x04af5, 0x04970, 0x064b0, 0x074a3, 0x0ea50, 0x06b58, 0x055c0, 0x0ab60, 0x096d5, 0x092e0, 
0x0c960, 0x0d954, 0x0d4a0, 0x0da50, 0x07552, 0x056a0, 0x0abb7, 0x025d0, 0x092d0, 0x0cab5, 
0x0a950, 0x0b4a0, 0x0baa4, 0x0ad50, 0x055d9, 0x04ba0, 0x0a5b0, 0x15176, 0x052b0, 0x0a930, 
0x07954, 0x06aa0, 0x0ad50, 0x05b52, 0x04b60, 0x0a6e6, 0x0a4e0, 0x0d260, 0x0ea65, 0x0d530, 
0x05aa0, 0x076a3, 0x096d0, 0x04bd7, 0x04ad0, 0x0a4d0, 0x1d0b6, 0x0d250, 0x0d520, 0x0dd45, 
0x0b5a0, 0x056d0, 0x055b2, 0x049b0, 0x0a577, 0x0a4b0, 0x0aa50, 0x1b255, 0x06d20, 0x0ada0, 
0x14b63};

/* 节气预计算值 */
static const int jieqi_data[] = {
	 0, 21208, 42467, 63836, 85337, 107014, 128867, 150921, 173149,
	 195551, 218072, 240693, 263343, 285989, 308563, 331033, 353350,
	 375494, 397447, 419210, 440795, 462224, 483532, 504758};

/* }}} */

/* 农历相关函数  {{{ */

/* 测试calendar_data某年某位的值 */
#define NIAN_BIT(nian, bit)(calendar_data[nian - 1900] & bit)

/* 农历某年哪个月是闰月，没有闰则为0 */
#define WHICH_RUN_YUE(nian)(NIAN_BIT(nian, 0xF))

/* 农历某年闰月的天数 */
static int days_of_run_rue(int nian)
{
	if(WHICH_RUN_YUE(nian))
		return (NIAN_BIT(nian, 0x10000) > 0)?30:29;
	else
		return 0;
}

/* 农历某年的天数 */
static int days_of_nian(int nian)
{
	int i;
	int sum = 348;

	for (i=0x8000; i>8; i>>=1)
		if (NIAN_BIT(nian, i) > 0) sum++;
	return sum + days_of_run_rue(nian);
}

/* 是否为公历闰年 */
static int is_bissextile(int year)
{
	if ((year % 4==0 && year % 100!=0) || year % 400==0)
		 return 1;
	else
		 return 0;
}

/* 公历某年年初到某月前一月的天数,一月忽略 */
static int sum_to_premonth(int year, int month)
{
	int i, sum = (month - 1)?(month - 1) * 30:0;

	for (i=1; i<=month - 1; i++)
		switch(i) {
			case 1: case 3: case 5: case 7: 
			case 8: case 10: case 12:
				sum++;
				break;
			case 2 :
				sum -= 2;
				if (is_bissextile(year)) sum++;
				break;
		}
	return sum;
}
/* }}} */

/* 公历相关函数 {{{ */

/* 计算两个公历日期的相差天数 */
static int sub_two_date(int year1, int month1, int day1,
		  		int year2, int month2, int day2)
{
	int i;

	/* 两个年份到年初的天数相减并加上365/366 */
	int sum = sum_to_premonth(year1, month1) + 
		 	365 - sum_to_premonth(year2, month2);
	if (is_bissextile(year2)) sum--;
	sum += day1 - day2;

	/* 闰年365+1 */
	for (i=year2+1; i<year1; i++)
		if (is_bissextile(i)) sum++;
	if ((year1 - year2) > 0)
		sum += (year1 - year2 - 1) * 365;
	else if (year1 == year2)
		sum -= 365;
	else
		return -1;

	return sum;
}

/* 公历某年的第n个节气为几号(从0小寒算起) */
static int which_day_is_jieqi(int year, int n)
{
	short days_of_permonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
	int a,i = 0;
	if (is_bissextile(year)) days_of_permonth[2]++;

	/* 1900年1月6日2点05分为小寒，精确到分往后推算 */
	a = 525948 * (year - 1900) -
		 sub_two_date(year, 1, 6, 1900, 1, 6) * 24 * 60
		 + jieqi_data[n] + 125;

	/* 若为小寒且在6日00点以前，天数减1 */
	if (a < 0) i--;
	a = i + 6 + a / (24 * 60);

	for (i=0; (i<13) && (a>28); i++)
		a -= days_of_permonth[i];

	return a;
}

/* }}} */

/* 计算农历年月日 {{{ */
/* 唯一的导出函数 */
int chinese_calender(int year, int month, int day, int hour,
							  struct c_calender *d)
/* year month day hour 为公历年月日时，d用来传出数据，
 * 函数默认返回1，若年份小于1901大于2050,返回0。 */
{

	static int percalc_val[] = {0, 18279, 36529};

	unsigned int i, x, run_yue, is_run_yue = 0;

	int all_days = sub_two_date(year, month, day, 1900, 1, 31);

	if (year < 1901 || year > 2050)
		 return 1;

	/* 此处为优化算法，每隔50年置一预计算值 */
	for (i=0; i<5; i++)
		if ((year > (i * 50+1900)) && (year <= ((i + 1) * 50+1900)))
			 break;

	all_days -= percalc_val[i];

	for (i=i*50+1900; (all_days>0)&&(i<2050); i++) {
		x = days_of_nian(i);
		all_days -= x;
	}
	/* -------------------------------------- */

	if (all_days < 0) {
		 all_days += x;
		 i--;
	}

	d->year = i;

	run_yue = WHICH_RUN_YUE(i);

	/* TODO 此处尚待优化 */
	for (i=1; i<13 && all_days>0; i++) {
		/* 如果有农历闰月 */
		if ((run_yue > 0) && (i == run_yue + 1) && (is_run_yue == 0)) {
			--i;
			is_run_yue = 1;
			x = days_of_run_rue(d->year);
		}
		else
			x = NIAN_BIT(d->year, 0x10000 >> i)?30:29;
		all_days -= x;
		if (is_run_yue == 1 && i == (run_yue + 1)) is_run_yue = 0;
	}
	if (all_days < 0) {
		 all_days += x;
		 i--;
	}
	
	if ((all_days == 0) && (run_yue > 0) && (i == run_yue + 1)) {
		if (is_run_yue == 1)
			is_run_yue = 0;
		else {
			is_run_yue = 1;
			--i;
		}
	}

	d->month = i;
	d->day = all_days + 1;
	d->month_stat = is_run_yue;

	/* 判断大小月 */
	if (is_run_yue)
		d->month_stat += 
			 (NIAN_BIT(d->year, 0x10000)?1:0) << 1;
	else
		d->month_stat +=
			 (NIAN_BIT(d->year, 0x10000>>d->month)?1:0) << 1;


	/* 判断节气 */
	if (day == which_day_is_jieqi(year, (month - 1) * 2))
		d->month_stat += (month - 1) * 2 << 8;
	else if (day == which_day_is_jieqi(year, (month - 1) * 2 + 1))
		d->month_stat += ((month - 1) * 2 + 1) << 8;
	else
		d->month_stat += 24 << 8;

	/* 计算四柱 */
	/* 年柱，以正月初一划分，1900年正月初一为庚子年 */
	x = year - 1900 + 36;
	/* 调整正月初一前的年柱 */
	if ((month <= 2) && (d->month >= 11)) x--;
	d->sizhu = x % 60 << 24;

	/* 月柱，以节（公历每月的第一个节气为节，第二个为气）划分，
	 * 1900年1月小寒前一天为丙子月
	 */
	x = month + 12 + (year - 1900) * 12;
	/* 调整节前的月柱 */
	if (day < which_day_is_jieqi(year, (month - 1) * 2)) x--;
	d->sizhu += x % 60 << 16;

	/* 日柱，前一天的23时与当天的00时为子时，1900年1月1日为甲戌日 */
	d->sizhu += (sub_two_date(year, month, day, 1900, 1, 1) + 10) % 60 << 8;

	/* 时柱 */
	d->sizhu += ((sub_two_date(year, month, day, 1900, 1, 1) * 24
				   + hour + 1) / 2) % 60;

	return 0;

}
/* }}} */

