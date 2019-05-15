// google.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <iostream>

typedef struct tagCharItem
{
	char c;
	int value;
    bool leading;
}CHAR_ITEM;

typedef struct tagCharValue
{
    bool used;
	int value;
}CHAR_VALUE;

const int max_number_count = 10;
const int max_char_count = 9;

typedef void (*CharListReadyFuncPtr)(CHAR_ITEM ci[max_char_count]);

CHAR_ITEM *GetCharItem(CHAR_ITEM ci[max_char_count], char c)
{
    for(int i = 0; i < max_char_count; ++i)
    {
        if(ci[i].c == c)
        {
            return &ci[i];
        }
    }

    return NULL;
}

int MakeIntegerValue(CHAR_ITEM ci[max_char_count], char *chars)
{
    assert(chars);

    int value = 0;
    char *p = chars;
    while(*p != 0)
    {
        CHAR_ITEM *char_item = GetCharItem(ci, *p);
        //assert(char_item);
        if(char_item == NULL)
        {
            return 0;
        }

        value = value * 10 + char_item->value;
        p++;
    }

    return value;
}

bool IsValueValid(CHAR_ITEM ii, CHAR_VALUE vv)
{
    if(vv.used)
    {
        return false;
    }

    if(ii.leading && (vv.value == 0))
    {
        return false;
    }

    return true;
}

void SearchingResult(CHAR_ITEM ci[max_char_count], CHAR_VALUE cv[max_number_count], 
                     int index, CharListReadyFuncPtr callback)
{
    if(index == max_char_count)
    {
        callback(ci);
        return;
    }

    for(int i = 0; i < max_number_count; ++i)
    {
        if(IsValueValid(ci[index], cv[i]))
        {
            cv[i].used = true;/*set used sign*/
            ci[index].value = cv[i].value;
            SearchingResult(ci, cv, index + 1, callback);
            cv[i].used = false;/*clear used sign*/
        }
    }
}

static int total = 0;
void OnCharListReady(CHAR_ITEM ci[max_char_count])
{
    total++;
    char *minuend    = "WWWDOT";
    char *subtrahend = "GOOGLE";
    char *diff       = "DOTCOM";
/*
    char *minuend    = "AAB";
    char *subtrahend = "BBC";
    char *diff       = "CCD";
*/

    int m = MakeIntegerValue(ci, minuend);
    int s = MakeIntegerValue(ci, subtrahend);
    int d = MakeIntegerValue(ci, diff);
    if((m - s) == d)
    {
        std::cout << m << " - " << s << " = " << d << std::endl; 
    }
}

int main(int argc, char *argv[])
{

    CHAR_ITEM char_item[max_char_count] = { { 'W', -1, true  }, { 'D', -1, true  }, { 'O', -1, false }, 
                                           { 'T', -1, false }, { 'G', -1, true  }, { 'L', -1, false }, 
                                           { 'E', -1, false }, { 'C', -1, false }, { 'M', -1, false } };
/*
    CHAR_ITEM char_item[max_char_count] = { {'A', -1, true}, {'B', -1, true}, {'C', -1, true}, 
                                           {'D', -1, false} };
*/
    CHAR_VALUE char_val[max_number_count] = { {false, 0}, {false, 1}, {false, 2}, {false, 3}, 
                                             {false, 4}, {false, 5}, {false, 6}, {false, 7}, 
                                             {false, 8}, {false, 9} };

    SearchingResult(char_item, char_val, 0, OnCharListReady);

    return 0;
}

