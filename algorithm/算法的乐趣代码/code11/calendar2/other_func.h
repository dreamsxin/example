#ifndef __OTHER_FUNCTION_H__
#define __OTHER_FUNCTION_H__

/*利用已知节气推算其它节气的函数，st的值以小寒节气为0，大寒为1，其它节气类推*/
double CalculateSolarTermsByExp(int year, int st);

double CalculateSolarTermsByFm(int year, int st);
double CalculateNewMoonByFm(int m);

#endif //__OTHER_FUNCTION_H__
