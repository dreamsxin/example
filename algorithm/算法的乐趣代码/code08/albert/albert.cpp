// albert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>

const int GROUPS_ITEMS = 5;
const int GROUPS_COUNT = 5;

const int COLOR_BLUE    = 0;
const int COLOR_RED     = 1;
const int COLOR_GREEN   = 2;
const int COLOR_YELLOW  = 3;
const int COLOR_WHITE   = 4;


const int NATION_NORWAY     = 0;
const int NATION_DANMARK    = 1;
const int NATION_SWEDEND    = 2;
const int NATION_ENGLAND    = 3;
const int NATION_GERMANY    = 4;


const int DRINK_TEA     = 0;
const int DRINK_WATER   = 1;
const int DRINK_COFFEE  = 2;
const int DRINK_BEER    = 3;
const int DRINK_MILK    = 4;

const int PET_HORSE   = 0;
const int PET_CAT     = 1;
const int PET_BIRD    = 2;
const int PET_FISH    = 3;
const int PET_DOG     = 4;

const int CIGARET_BLENDS      = 0;
const int CIGARET_DUNHILL     = 1;
const int CIGARET_PRINCE      = 2;
const int CIGARET_PALLMALL    = 3;
const int CIGARET_BLUEMASTER  = 4;

typedef enum tagItemType
{
    type_house = 0,
    type_nation = 1,
    type_drink = 2,
    type_pet = 3,
    type_cigaret = 4
}ITEM_TYPE;

const char *itemName[GROUPS_ITEMS] = { "房子", "国家", "饮料", "宠物", "烟"};
const char *valueName[GROUPS_ITEMS][GROUPS_COUNT] = 
{ 
    { "蓝色", "红色", "绿色", "黄色", "白色" },
    { "挪威", "丹麦", "瑞士", "英国", "德国" },
    { "茶", "水", "咖啡", "啤酒", "牛奶" },
    { "马", "猫", "鸟", "鱼", "狗" },
    { "Blends", "Dunhill", "Prince", "PallMall", "BlueMaster" }
};

typedef struct tagItem
{
    ITEM_TYPE type;
    int value;
}ITEM;

typedef struct tagBind
{
    ITEM_TYPE first_type;
    int first_val;
    ITEM_TYPE second_type;
    int second_val;
}BIND;

const BIND binds[] = 
{
    { type_house, COLOR_RED, type_nation, NATION_ENGLAND },
    { type_nation, NATION_SWEDEND, type_pet, PET_DOG },
    { type_nation, NATION_DANMARK, type_drink, DRINK_TEA },
    { type_house, COLOR_GREEN, type_drink, DRINK_COFFEE },
    { type_cigaret, CIGARET_PALLMALL, type_pet, PET_BIRD },
    { type_house, COLOR_YELLOW, type_cigaret, CIGARET_DUNHILL },
    { type_cigaret, CIGARET_BLUEMASTER, type_drink, DRINK_BEER },
    { type_nation, NATION_GERMANY, type_cigaret, CIGARET_PRINCE }
};

const int BINDS_COUNT = sizeof(binds) / sizeof(binds[0]);

typedef struct tagRelation
{
    ITEM_TYPE type;
    int val;
    ITEM_TYPE relation_type;
    int relation_val;
}RELATION;

const RELATION relations[] = 
{
    { type_cigaret, CIGARET_BLENDS, type_pet, PET_CAT },
    { type_pet, PET_HORSE, type_cigaret, CIGARET_DUNHILL },
    { type_nation, NATION_NORWAY, type_house, COLOR_BLUE },
    { type_cigaret, CIGARET_BLENDS, type_drink, DRINK_WATER }
};

const int RELATIONS_COUNT = sizeof(relations) / sizeof(relations[0]);
/*
typedef struct tagGroup
{
    ITEM items[GROUPS_ITEMS];
}GROUP;
*/
typedef struct tagGroup
{
    int itemValue[GROUPS_ITEMS];
}GROUP;


int GetGroupItemValue(GROUP *group, ITEM_TYPE type)
{
    return group->itemValue[type];
}

int FindGroupIdxByItem(GROUP *groups, ITEM_TYPE type, int value)
{
    for(int i = 0; i < GROUPS_COUNT; i++)
    {
        if(GetGroupItemValue(&groups[i], type) == value)
        {
            return i;
        }
    }

    return -1;
}

bool CheckGroupBind(GROUP *groups, int groupIdx, ITEM_TYPE type, int value)
{
    if(GetGroupItemValue(&groups[groupIdx], type) != value)
    {
        return false;
    }

    return true;
}

bool CheckAllGroupsBind(GROUP *groups, const BIND *binds)
{
    for(int i = 0; i < BINDS_COUNT; i++)
    {
        int grpIdx = FindGroupIdxByItem(groups, binds[i].first_type, binds[i].first_val);
        if(grpIdx != -1)
        {
            if(!CheckGroupBind(groups, grpIdx, binds[i].second_type, binds[i].second_val))
            {
                return false;
            }
        }
    }

    return true;
}

bool CheckGroupRelation(GROUP *groups, int groupIdx, ITEM_TYPE type, int value)
{
    if(groupIdx == 0)
    {
        if(GetGroupItemValue(&groups[groupIdx + 1], type) != value)
        {
            return false;
        }
    }
    else if(groupIdx == (GROUPS_COUNT - 1))
    {
        if(GetGroupItemValue(&groups[groupIdx - 1], type) != value)
        {
            return false;
        }
    }
    else
    {
        if( (GetGroupItemValue(&groups[groupIdx - 1], type) != value)
            && (GetGroupItemValue(&groups[groupIdx + 1], type) != value) )
        {
            return false;
        }
    }

    return true;
}

bool CheckAllGroupsRelation(GROUP *groups, const RELATION *relations)
{
    for(int i = 0; i < RELATIONS_COUNT; i++)
    {
        int grpIdx = FindGroupIdxByItem(groups, relations[i].type, relations[i].val);
        if(grpIdx != -1)
        {
            if(!CheckGroupRelation(groups, grpIdx, relations[i].relation_type, relations[i].relation_val))
            {
                return false;
            }
        }
    }

    return true;
}

bool IsGroupItemValueUsed(GROUP *groups, int groupAsd, ITEM_TYPE type, int value)
{
    for(int i = 0; i < groupAsd; i++)
    {
        if(groups[i].itemValue[type] == value)
        {
            return true;
        }
    }

    return false;
}

typedef void (*GroupTypeEnumFuncPtr)(GROUP *groups);


void PrintGroupsNameTitle()
{
    for(int i = type_house; i <= type_cigaret; i++)
    {
        printf("%-15s", itemName[i]);
    }
    printf("\n");
}

void PrintSingleLineGroup(GROUP *group)
{
    for(int i = type_house; i <= type_cigaret; i++)
    {
        printf("%-15s", valueName[i][group->itemValue[i]]);
    }
    printf("\n");
}

void PrintAllGroupsResult(GROUP *groups, int groupCount)
{
    PrintGroupsNameTitle();
    for(int i = 0; i < groupCount; i++)
    {
        PrintSingleLineGroup(&groups[i]);
    }
    printf("\n");
}

static int cnt = 0;
void DoGroupsfinalCheck(GROUP *groups)
{
    cnt++;
    if((cnt % 1000000) == 0)
        printf("%d\n", cnt);

    if(CheckAllGroupsBind(groups, binds) && CheckAllGroupsRelation(groups, relations))
    {
        PrintAllGroupsResult(groups, GROUPS_COUNT);
    }
}

void EnumPeopleCigerts(GROUP *groups, int groupIdx)
{
    if(groupIdx == GROUPS_COUNT) /*递归终止条件*/
    {
        DoGroupsfinalCheck(groups);
        return;
    }

    for(int i = CIGARET_BLENDS; i <= CIGARET_BLUEMASTER; i++)
    {
        if(!IsGroupItemValueUsed(groups, groupIdx, type_cigaret, i))
        {
            groups[groupIdx].itemValue[type_cigaret] = i;

            EnumPeopleCigerts(groups, groupIdx + 1);
        }
    }
}

void ArrangePeopleCigert(GROUP *groups)
{
    EnumPeopleCigerts(groups, 0);
}

void EnumPeoplePats(GROUP *groups, int groupIdx)
{
    if(groupIdx == GROUPS_COUNT) /*递归终止条件*/
    {
        ArrangePeopleCigert(groups);
        return;
    }

    for(int i = PET_HORSE; i <= PET_DOG; i++)
    {
        if(!IsGroupItemValueUsed(groups, groupIdx, type_pet, i))
        {
            groups[groupIdx].itemValue[type_pet] = i;

            EnumPeoplePats(groups, groupIdx + 1);
        }
    }
}

void ArrangePeoplePet(GROUP *groups)
{
    /*这里没有可用规则*/
    EnumPeoplePats(groups, 0);
}

void EnumPeopleDrinks(GROUP *groups, int groupIdx)
{
    if(groupIdx == GROUPS_COUNT) /*递归终止条件*/
    {
        /*应用规则(8)：住在中间那个房子里的人喝牛奶；*/
        if(groups[2].itemValue[type_drink] == DRINK_MILK)
        {
            ArrangePeoplePet(groups);
        }
        return;
    }

    for(int i = DRINK_TEA; i <= DRINK_MILK; i++)
    {
        if(!IsGroupItemValueUsed(groups, groupIdx, type_drink, i))
        {
            groups[groupIdx].itemValue[type_drink] = i;
            EnumPeopleDrinks(groups, groupIdx + 1);
        }
    }
}

void ArrangePeopleDrinks(GROUP *groups)
{
    /*这里没有可用规则*/
    EnumPeopleDrinks(groups, 0);
}

/*遍历国家*/
void EnumHouseNations(GROUP *groups, int groupIdx)
{
    if(groupIdx == GROUPS_COUNT) /*递归终止条件*/
    {
        ArrangePeopleDrinks(groups);
        return;
    }

    for(int i = NATION_NORWAY; i <= NATION_GERMANY; i++)
    {
        if(!IsGroupItemValueUsed(groups, groupIdx, type_nation, i))
        {
            groups[groupIdx].itemValue[type_nation] = i;

            EnumHouseNations(groups, groupIdx + 1);
        }
    }
}

void ArrangeHouseNations(GROUP *groups)
{
    /*应用规则(9)：挪威人住在第一个房子里面；*/
    groups[0].itemValue[type_nation] = NATION_NORWAY;
    EnumHouseNations(groups, 1); /*从第二个房子开始*/
}


/* 遍历房子颜色*/
void EnumHouseColors(GROUP *groups, int groupIdx)
{
    if(groupIdx == GROUPS_COUNT) /*递归终止条件*/
    {
        ArrangeHouseNations(groups);
        return;
    }

    for(int i = COLOR_BLUE; i <= COLOR_YELLOW; i++)
    {
        if(!IsGroupItemValueUsed(groups, groupIdx, type_house, i))
        {
            groups[groupIdx].itemValue[type_house] = i;
            if(i == COLOR_GREEN) //应用线索(4)：绿房子紧挨着白房子，在白房子的左边；
            {
                groups[++groupIdx].itemValue[type_house] = COLOR_WHITE;
            }

            EnumHouseColors(groups, groupIdx + 1);
            if(i == COLOR_GREEN)
            {
                groupIdx--;
            }
        }
    }
}


int main(int argc, char* argv[])
{
    GROUP groups[GROUPS_COUNT] = { { 0 } };

    EnumHouseColors(groups, 0);

	return 0;
}

void test_Checkfunctions()
{
    GROUP groups[GROUPS_COUNT] = 
    {
        {COLOR_YELLOW, NATION_NORWAY, DRINK_WATER, PET_CAT, CIGARET_DUNHILL},
        {COLOR_BLUE, NATION_DANMARK, DRINK_TEA, PET_HORSE, CIGARET_BLENDS},
        {COLOR_RED, NATION_ENGLAND, DRINK_MILK, PET_BIRD, CIGARET_PALLMALL},
        {COLOR_GREEN, NATION_GERMANY, DRINK_COFFEE, PET_FISH, CIGARET_PRINCE},
        {COLOR_WHITE, NATION_SWEDEND, DRINK_BEER, PET_DOG, CIGARET_BLUEMASTER}
    };
    assert(CheckAllGroupsBind(groups, binds));
    assert(CheckAllGroupsRelation(groups, relations));

    GROUP groups2[GROUPS_COUNT] = 
    {
        {COLOR_YELLOW, NATION_DANMARK, DRINK_WATER, PET_CAT, CIGARET_DUNHILL},
        {COLOR_BLUE, NATION_NORWAY, DRINK_TEA, PET_HORSE, CIGARET_BLENDS},
        {COLOR_RED, NATION_ENGLAND, DRINK_MILK, PET_BIRD, CIGARET_PALLMALL},
        {COLOR_GREEN, NATION_GERMANY, DRINK_COFFEE, PET_FISH, CIGARET_PRINCE},
        {COLOR_WHITE, NATION_SWEDEND, DRINK_BEER, PET_DOG, CIGARET_BLUEMASTER}
    };

    assert(!CheckAllGroupsBind(groups2, binds));
    assert(!CheckAllGroupsRelation(groups2, relations));

    GROUP groups3[GROUPS_COUNT] = 
    {
        {COLOR_YELLOW, NATION_NORWAY, DRINK_WATER, PET_CAT, CIGARET_DUNHILL},
        {COLOR_BLUE, NATION_DANMARK, DRINK_TEA, PET_BIRD, CIGARET_BLENDS},
        {COLOR_RED, NATION_ENGLAND, DRINK_MILK, PET_HORSE, CIGARET_PALLMALL},
        {COLOR_GREEN, NATION_GERMANY, DRINK_COFFEE, PET_FISH, CIGARET_PRINCE},
        {COLOR_WHITE, NATION_SWEDEND, DRINK_BEER, PET_DOG, CIGARET_BLUEMASTER}
    };

    assert(!CheckAllGroupsBind(groups3, binds));
    assert(!CheckAllGroupsRelation(groups3, relations));
}



/*

绿房子紧挨着白房子，在白房子的左边；

住在中间那个房子里的人喝牛奶；

挪威人住在第一个房子里面；

           国家           房子           宠物           饮料           香烟
           挪威           黄色             猫         矿泉水        Dunhill
           丹麦           蓝色             马             茶         Blends
           英国           红色             鸟           牛奶       PallMall
           德国           绿色             鱼           咖啡         Prince
           瑞典           白色             狗           啤酒     BlueMaster
*/ 






