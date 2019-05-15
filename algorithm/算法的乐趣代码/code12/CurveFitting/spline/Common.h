#ifndef __COMMON_H__
#define __COMMON_H__

const double PRECISION = 1E-10;

#define ARR_INDEX(row, col, dim)  ((row) * (dim) + (col))


bool IsPrecisionZero(double value);

#endif //__COMMON_H__