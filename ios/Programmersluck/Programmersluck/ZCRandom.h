//
//  ZCRandom.h
//  Programmersluck
//
//  Created by cuibo on 3/21/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ZCRandom : NSObject

//初始化随机数种子
- (void)srandom:(unsigned long)Seed;

//重置重建开关并初始化随机数种子
- (void)rebuildAndSrandom:(unsigned long)Seed;

//重置重建开关
- (void)rebuild;

//获得0~max之间的(不含max)一个随机数，与之前生成的数不重复。
- (NSInteger)randomNotRepeat:(NSInteger)max;

//获得0~max之间的(不含max)一个随机数
- (NSInteger)random:(NSInteger)max;

@end

