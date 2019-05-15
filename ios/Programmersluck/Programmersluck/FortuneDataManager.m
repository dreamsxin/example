//
//  DataManager.m
//  Programmersluck
//
//  Created by cuibo on 3/17/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "FortuneDataManager.h"
#import "ExpandNSDate.h"
#import "LoadData.h"
#import "ZCRandom.h"
#import "OCLunarDate.h"

typedef enum
{
    kAppropriateSrandomTag = 0,
    kDirectionSrandomTag,
    kDrinkSrandomTag,
    kLuckyApeSrandomTag,
    kGoddessSrandomTag
}SrandomTag;

@interface FortuneDataManager()

@property(strong, nonatomic)LoadData *loadData;
@property(strong, nonatomic)NSDate *selectDate;
@property(strong, nonatomic)ZCRandom *zcRandom;

@end

@implementation FortuneDataManager

- (id)init
{
    self = [super init];
    if (self)
    {
        self.loadData = [[LoadData alloc] init];
        self.zcRandom = [[ZCRandom alloc] init];
    }
    return self;
}

//初始化随机数种子
- (void)srandomWithTag:(SrandomTag)srandomTag
{
    //取整秒数到天
    unsigned long timeSince = [self.selectDate timeIntervalSince1970];
    timeSince = timeSince - (timeSince%(3600*24));
    //初始化随机种子，使用天数加上tag(tag不能大于3600,否则随机值和明天一样了)
    [self.zcRandom rebuildAndSrandom:timeSince+srandomTag];
}

//使用日期更新数据
- (void)updateWithDate:(NSDate *)date
{
    //保存当前时间
    self.selectDate = date;
    
    OCLunarDate *lunarDate = [OCLunarDate lunarDateWithNSDate:self.selectDate];
    if(lunarDate.imonth == 3 && lunarDate.iday == 13)
    {
        //获取适宜列表
        self.appropriateList = [[NSArray alloc] initWithObjects:[self creatAppropriate:@"想她,爱她" detail:@""], nil];;
        self.tabooList = [[NSArray alloc] initWithObjects:[self creatAppropriate:@"心里有别人" detail:@""], nil];
        self.direction = @"面对着她";
        self.drinks = [[NSArray alloc] initWithObjects:@"茶", nil];
        self.goddess = @"她的生日,拒绝一切女神";
        self.luckyApe = @"所有人";
    }
    else
    {
        //更新模块
        [self updateActivitiesList];
        [self updateDirection];
        [self updateDrink];
        [self updateLuckyApe];
        [self updateGoddess];
    }
}

//更新 适宜活动列表 忌讳活动列表
- (void)updateActivitiesList
{
    [self srandomWithTag:kAppropriateSrandomTag];

    NSMutableArray *list;
    int changes;

    for (int tag = 0; tag<2; tag++)
    {
        NSString *detailKey;

        if(tag == 0)
        {
            detailKey = kLoadDataActivitiesGood;
        }
        else
        {
            detailKey = kLoadDataActivitiesBad;
        }
        
        //优先获得精选列表
        list = [self specialsActivitiesWithKey:detailKey];
        NSArray *activitiesList = [self.loadData activitiesList];
        changes = [self.zcRandom random:2-[list count]];
        for (int i=0; i < 2+changes; i++)
        {
            NSInteger activitiesItem = [self.zcRandom randomNotRepeat:[activitiesList count]];
            NSDictionary *activitiesDict = [[self.loadData activitiesList] objectAtIndex:activitiesItem];
            NSString *title = [activitiesDict objectForKey:kLoadDataActivitiesName];
            NSString *detail = [activitiesDict objectForKey:detailKey];
            [list addObject:[self creatAppropriate:title detail:detail]];
        }
        
        if(tag == 0)
        {
            self.appropriateList = list;
        }
        else
        {
            self.tabooList = list;
        }
    }
}

//精选的活动事件,type-0 获得精选适宜活动，1 获得精选忌讳活动
- (NSMutableArray *)specialsActivitiesWithKey:(NSString *)typeKey
{
    NSMutableArray *list = [[NSMutableArray alloc] init];
    NSArray *specialsList = [self.loadData specialsEventList];
    //变例列表
    for (NSDictionary *dict in specialsList)
    {
        //比较日期
        NSString *specialsDate = [dict objectForKey:kLoadDataSpecialsEventDate];
        if([specialsDate compare:[self.selectDate getDateFormatterForStr:@"yyyyMMdd"]] == NSOrderedSame)
        {
            //比较类型是否相等
            NSString *specialsType = [dict objectForKey:kLoadDataSpecialsEventType];
            if([specialsType compare:typeKey] == NSOrderedSame)
            {
                NSString *title = [dict objectForKey:kLoadDataSpecialsEventName];
                NSString *detail = [dict objectForKey:kLoadDataSpecialsEventDescription];
                [list addObject:[self creatAppropriate:title detail:detail]];
            }
        }
    }

    return list;
}

//构造一条活动
- (NSDictionary *)creatAppropriate:(NSString *)title detail:(NSString *)detail
{
    NSDictionary *appropriate = [[NSDictionary alloc] initWithObjectsAndKeys:
                                 title, kActivitiesTitle,
                                 detail, kActivitiesDetail, nil];
    return appropriate;
}

//更新 座位朝向
- (void)updateDirection
{
    [self srandomWithTag:kDirectionSrandomTag];
    
    NSArray *directionsEventList = [self.loadData directionsEventList];
    NSArray *directionsList = [self.loadData directionsList];
    
    NSInteger directionsEventItem = [self.zcRandom random:[directionsEventList count]];
    NSDictionary *directionDict = [[self.loadData directionsEventList] objectAtIndex:directionsEventItem];
    NSString *direction;
    direction = [directionDict objectForKey:kLoadDataDirectionsEventPrefixes];
    direction = [direction stringByAppendingString:@"["];
    NSInteger directionsItem = [self.zcRandom random:[directionsList count]];
    direction = [direction stringByAppendingString:
                      [[self.loadData directionsList] objectAtIndex:directionsItem]];
    direction = [direction stringByAppendingString:@"]"];
    direction = [direction stringByAppendingString:[directionDict objectForKey:kLoadDataDirectionsEventSuffix]];
    self.direction = direction;
}

//更新 最佳饮品
- (void)updateDrink
{
    [self srandomWithTag:kDrinkSrandomTag];

    NSMutableArray *list = [[NSMutableArray alloc] init];
    NSArray *drinksList = [self.loadData drinksList];
    int changes = [self.zcRandom random:2];
    for (int i=0; i<1+changes; i++)
    {
        NSInteger drinksItem = [self.zcRandom randomNotRepeat:[drinksList count]];
        [list addObject:[drinksList objectAtIndex:drinksItem]];
    }
    
    self.drinks = list;
}

//更新 最好运程序行业
- (void)updateLuckyApe
{
    [self srandomWithTag:kLuckyApeSrandomTag];
    
    NSArray *luckyWorkList = [self.loadData luckyWorkList];
    NSInteger luckyWorkItem = [self.zcRandom randomNotRepeat:[luckyWorkList count]];
    
    self.luckyApe = [luckyWorkList objectAtIndex:luckyWorkItem];
}

//更新 女神接近指数
- (void)updateGoddess
{
    [self srandomWithTag:kGoddessSrandomTag];
    
    self.goddess = [NSString stringWithFormat:@"%d%%", [self.zcRandom random:100]];
}

@end


