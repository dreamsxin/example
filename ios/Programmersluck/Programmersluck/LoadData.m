//
//  LoadData.m
//  Programmersluck
//
//  Created by cuibo on 3/19/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "LoadData.h"

#define kJSONActivities         @"activities"
#define kJSONDirections         @"directions"
#define kJSONDirectionsEvent    @"directionsEvent"
#define kJSONDrinks             @"drinks"
#define kJSONLuckyWork          @"luckyWork"
#define kJSONSpecialsEvent      @"specials"

@interface LoadData()

@property(strong, nonatomic)NSDictionary *myJsonDict;    //事件列表

@end


@implementation LoadData

- (id)init
{
    self = [super init];
    if (self)
    {
        self.myJsonDict = [self  getData];
    }
    return self;
}

//获取数据字典
- (NSDictionary *)getData
{
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"Data" ofType:@"json"];
    NSString *myJSONString = [[NSString alloc] initWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:NULL];
    
    NSError *error;
    NSDictionary *myJSON = [NSJSONSerialization JSONObjectWithData:[myJSONString dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
    
    return myJSON;
}

//方向列表
- (NSArray *)directionsList
{
    return [self.myJsonDict objectForKey:kJSONDirections];
}

//事件列表
- (NSArray *)activitiesList
{
    return [self.myJsonDict objectForKey:kJSONActivities];
}

//饮品列表
- (NSArray *)drinksList
{
    return [self.myJsonDict objectForKey:kJSONDrinks];
}

//幸运工作列表
- (NSArray *)luckyWorkList
{
    return [self.myJsonDict objectForKey:kJSONLuckyWork];
}

//方向事件列表
- (NSArray *)directionsEventList
{
    return [self.myJsonDict objectForKey:kJSONDirectionsEvent];
}

//特殊日期事件列表
- (NSArray *)specialsEventList
{
    return [self.myJsonDict objectForKey:kJSONSpecialsEvent];
}

@end

