//
//  ZCRandom.m
//  Programmersluck
//
//  Created by cuibo on 3/21/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "ZCRandom.h"

@interface ZCRandom()

    @property(assign, nonatomic)BOOL isRebuildRandom;
    @property(strong, nonatomic)NSMutableArray *randomList;

@end

@implementation ZCRandom

//初始化随机数种子
- (void)srandom:(unsigned long)Seed
{
    srandom(Seed);
}

//初始化随机数种子
- (void)rebuildAndSrandom:(unsigned long)Seed
{
    [self srandom:Seed];
    [self rebuild];
}

//重置重建开关
- (void)rebuild
{
    self.isRebuildRandom = YES;
    self.randomList = [[NSMutableArray alloc] init];
}

//获得0~max之前的(不含max)一个随机数，与之前生成的数不重复。
- (NSInteger)randomNotRepeat:(NSInteger)max
{
    NSInteger current;
    if(self.isRebuildRandom == YES)
    {
        self.isRebuildRandom = NO;
        //重新开始生成
        current = random()%max;
        [self.randomList addObject:[NSNumber numberWithInteger:current]];
    }
    else
    {
        //重新开始生成
        int cx = 0;
        current = random()%max;
        for (int i=0; i<[self.randomList count]; i++)
        {
            NSNumber *num = [self.randomList objectAtIndex:i];
            //碰撞到则重生成
            if(current == [num integerValue])
            {
                current = random()%max;
                //重试次数限制,超过返回0
                if(++cx >= max)
                    return 0;
                //重试
                i = 0;
                continue;
            }
        }
        [self.randomList addObject:[NSNumber numberWithInteger:current]];
    }

    return current;
}

//获得0~max之前的(不含max)一个随机数
- (NSInteger)random:(NSInteger)max
{
    return random()%max;
}

@end
