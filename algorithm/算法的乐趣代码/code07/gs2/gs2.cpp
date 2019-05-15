// gs2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


const int UNIT_COUNT = 3;

typedef struct tagPartner
{
    char *name;   //名字
    int next;     //下一个邀请对象
    int current;  //当前舞伴，-1表示还没有舞伴
    int pCount; //偏爱列表中舞伴个数
    int perfect[UNIT_COUNT]; //偏爱列表
}PARTNER;


//这里也是算法一致性处理的一个技巧
int GetPerfectPosition(PARTNER *partner, int id)
{
    for(int i = 0; i < partner->pCount; i++)
    {
        if(partner->perfect[i] == id)
        {
            return i;
        }
    }

    //返回一个非常大的值，意味着根本排不上对
    return 0x7FFFFFFF;
}

void PrintResult(PARTNER *boys, PARTNER *girls, int count)
{
    for(int i = 0; i < count; i++)
    {
        std::cout << boys[i].name << "[" 
                  << GetPerfectPosition(&boys[i], boys[i].current)
                  << "] <---> " << girls[boys[i].current].name << "["
                  << GetPerfectPosition(&girls[boys[i].current], i)
                  << "]" << std::endl;
    }
    std::cout << std::endl;
}

bool IsPartnerAssigned(PARTNER *partner)
{
    return (partner->current != -1);
}

bool IsFavoritePartner(PARTNER *partner, int bid)
{
    for(int i = 0; i < partner->pCount; i++)
    {
        if(partner->perfect[i] == bid) 
        {
            return true;
        }
    }

    return false;
}

bool IsStableMatch(PARTNER *boys, PARTNER *girls)
{
    for(int i = 0; i < UNIT_COUNT; i++)
    {
        //找到男孩当前舞伴在自己的偏好列表中的位置
        int gpos = GetPerfectPosition(&boys[i], boys[i].current);
        //在position位置之前的舞伴，男孩喜欢她们胜过current
        for(int k = 0; k < gpos; k++)
        {
            int gid = boys[i].perfect[k];
            //找到男孩在这个女孩的偏好列表中的位置
            int bpos = GetPerfectPosition(&girls[gid], i);
            //找到女孩的当前舞伴在这个女孩的偏好列表中的位置
            int cpos = GetPerfectPosition(&girls[gid], girls[gid].current);
            if(bpos < cpos)
            {
                //女孩也是喜欢这个男孩胜过喜欢自己当前的舞伴，这是不稳定因素
                return false;
            }
        }
    }

    return true;
}

int totalMatch = 0;
int stableMatch = 0;
void SearchStableMatch(int index, PARTNER *boys, PARTNER *girls)
{
    if(index == UNIT_COUNT)
    {
        totalMatch++;
        if(IsStableMatch(boys, girls))
        {
            stableMatch++;
            PrintResult(boys, girls, UNIT_COUNT);
        }
        return;
    }

    for(int i = 0; i < boys[index].pCount; i++)
    {
        int gid = boys[index].perfect[i];
        
        if(!IsPartnerAssigned(&girls[gid]) && IsFavoritePartner(&girls[gid], index))
        {
            boys[index].current = gid;
            girls[gid].current = index;
            SearchStableMatch(index + 1, boys, girls);
            boys[index].current = -1;
            girls[gid].current = -1;
        }
    }
}

int main(int argc, char* argv[])
{
#if 1
    PARTNER girls[] = 
    {
        {"Laura", 0, -1, 3, {2,0,1} }, //Laura
        {"Marcy", 0, -1, 3, {0,2,1} },  //Marcy
        {"Nancy", 0, -1, 3, {1,0,2} } //Nancy
    };

    PARTNER boys[] = 
    {
        {"Albert", 0, -1, 3, {0,2,1} }, //Albert
        {"Brad",   0, -1, 3, {1,2,0} }, //Brad
        {"Chuck",  0, -1, 3, {0,1,2} } //Chuck
    };

/*
Albert Laura Nancy Marcy
Brad Marcy Nancy Laura
Chuck Laura Marcy Nancy

Laura Chuck Albert Brad
Marcy Albert Chuck Brad
Nancy Brad Albert Chuck


Albert Nancy
Brad Marcy
Chuck Laura
*/
#endif
#if 0
PARTNER girls[] = 
{
    {"A", 0, -1, 4, {2,3,1,0} },
    {"B", 0, -1, 4, {2,1,3,0} },
    {"C", 0, -1, 4, {0,2,3,1} },
    {"D", 0, -1, 4, {1,3,2,0} }
};
PARTNER boys[] = 
{
    {"1", 0, -1, 4, {0,3,2,1} },
    {"2", 0, -1, 4, {0,1,2,3} },
    {"3", 0, -1, 4, {0,2,3,1} },
    {"4", 0, -1, 4, {1,0,3,2} }
};

/*
A: 3,4,2,1
B: 3,2,4,1
C: 1,3,4,2
D: 2,4,3,1

1: A,D,C,B
2: A,B,C,D
3: A,C,D,B
4: B,A,D,C
*/
#endif

#if 0
PARTNER girls[] = 
{
    {"girl0", 0, -1, 5, {2,1,4,0,3} },
    {"girl1", 0, -1, 5, {1,3,0,4,2} },
    {"girl2", 0, -1, 5, {0,1,2,4,3} },
    {"girl3", 0, -1, 5, {2,0,3,1,4} },
    {"girl4", 0, -1, 5, {4,1,2,3,0} }
};

PARTNER boys[] = 
{
    {"boy0", 0, -1, 5, {4,1,0,3,2} },
    {"boy1", 0, -1, 5, {3,1,4,0,2} },
    {"boy2", 0, -1, 5, {2,3,4,0,1} },
    {"boy3", 0, -1, 5, {4,0,3,1,2} },
    {"boy4", 0, -1, 5, {3,4,2,0,1} },
};
#endif

    SearchStableMatch(0, boys, girls);
    std::cout << "Total Matchs : " << totalMatch << std::endl;
    std::cout << "Stable Matchs : " << stableMatch << std::endl;

    return 0;
}
