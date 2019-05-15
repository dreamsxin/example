// qsort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
T DivideAndConquer(P)
{
    if(P可以直接解决)
    {
        T <- P的结果;
        return T;
    }

    将P分解为子问题 {P1, P2,..., Pn};
    for_each(Pi : {P1, P2,..., Pn})
    {
        ti <- DivideAndConquer(Pi); //递归解决子问题Pi
    }
    T <- Merge(t1, t2,...,tn); //合并子问题的解
    
    return T;
}
*/

void exchange(int *arElem, int m, int n)
{
  int tmp = arElem[m];
  arElem[m] = arElem[n];
  arElem[n] = tmp;
}

int partion(int *arElem, int p, int r)
{
  int x = arElem[r];
  int i = p;
  for(int j = p; j < r; j++)
  {
    if(arElem[j] < x)
    {
      if(i != j)
      {
        exchange(arElem, i, j);
      }
      i++;
    }
  }
  exchange(arElem, i, r);
  return i;
}

void quick_sort(int *arElem, int p, int r)
{
  if(p < r)
  {
    int mid = partion(arElem, p, r);
    quick_sort(arElem, p, mid - 1);
    quick_sort(arElem, mid + 1, r);
  }
}

void Quick_Sort(int *arElem, int n)
{
  quick_sort(arElem, 0, n - 1);
}


int main(int argc, char* argv[])
{
	return 0;
}

