#include<stdio.h>
#include <time.h>

int main()
{        
    /* initialize */
    int y=1980, m=2, d=5;    
    struct tm t = { .tm_year=y-1900, .tm_mon=m-1, .tm_mday=d };
    /* modify */
    t.tm_mday += 1;
    mktime(&t);
    /* show result */
    printf("%s", asctime(&t)); /* prints: Sun Mar 16 00:00:00 1980 */

    char buffer[30];
    strftime(buffer, 30, "%Y-%m-%d", &t);
    puts(buffer);
    return 0;
}

int fun2()
{
    int d, m, y, nd, nm, ny;
    printf("Enter the year: ");
    scanf("%d", &y);
    printf("\nEnter the month: ");
    scanf("%d", &m);
    printf("\nEnter the date: ");
    scanf("%d", &d);
    printf("\nSo, your desired date is: %d/%d/%d", d, m, y);
    if (m%2!=0 && m>=1 && m<=7 && d==31)
    {
        nd=1;
        nm=m+1;
        ny=y;
    }
    else if (m%2==0 && m>=8 && m<=12 && d==31)
    {
        nd=1;
        nm=m+1;
        ny=y;
    }
    else if (m%2==0 && m>=4 && m<=6 && d==30)
    {
        nd=1;
        nm=m=1;
        ny=y;
    }
    else if (m%2!=0 && m>=9 && m<=11 && d==30)
    {
        nd=1;
        nm=m+1;
        ny=y;
    }
    else if (m==2 && y%4==0 && d==29)
    {
        nd=1;
        nm=m+1;
        ny=y;
    }
    else if (m==2 && y%4!=0, d==28)
    {
        nd=1;
        nm=m+1;
        ny=y;
    }
    else if (m==12 && d==31)
    {
        nd=1;
        nm=1;
        ny=y+1;
    }
    else
    {
        nd=d+1;
        nm=m;
        ny=y;
    }
    printf("\n\nThe next date of your desired date is:\t%d/%d/%d\n", nm, nm, ny);
}