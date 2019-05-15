//
//  FortuneViewController.m
//  Programmersluck
//
//  Created by cuibo on 3/17/13.
//  Copyright (c) 2013 cuibo. All rights reserved.
//

#import "FortuneViewController.h"
#import "ExpandNSDate.h"
#import "OCLunarDate.h"
#import "FortuneDataManager.h"

@interface FortuneViewController ()

@property(strong, nonatomic)FortuneDataManager *fortuneDataManager;
@property(strong, nonatomic)IBOutlet UIView *activitiesView;
@property(strong, nonatomic)IBOutlet UIView *otherFortuneView;
@property(strong, nonatomic)IBOutlet UIView *bottomInfoView;

@property(strong, nonatomic)IBOutlet UILabel *directionLabel;
@property(strong, nonatomic)IBOutlet UILabel *drinksLabel;
@property(strong, nonatomic)IBOutlet UILabel *goddessLabel;
@property(strong, nonatomic)IBOutlet UILabel *luckyApeLabel;

@end

@implementation FortuneViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        self.fortuneDataManager = [[FortuneDataManager alloc] init];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)viewDidAppear:(BOOL)animated
{
    ;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

//刷新视图
- (void)updateView:(NSDate *)aDate
{
    //更新数据管理器
    [self.fortuneDataManager updateWithDate:aDate];
    //刷新活动视图
    [self updateActivitiesView];
    //更新其他信息
    [self updateOtherFortuneView];
    //更新自己的大小
    self.view.frame = CGRectMake(self.view.frame.origin.x,
                                 self.view.frame.origin.y,
                                 self.view.frame.size.width,
                                 self.otherFortuneView.frame.origin.y + self.otherFortuneView.frame.size.height + self.bottomInfoView.frame.size.height);
}

//更新其他运势视图
- (void)updateOtherFortuneView
{
    //位置
    self.otherFortuneView.frame = CGRectMake(self.otherFortuneView.frame.origin.x,
                                             self.activitiesView.frame.origin.y + self.activitiesView.frame.size.height,
                                             self.otherFortuneView.frame.size.width,
                                             self.otherFortuneView.frame.size.height);
    
    self.directionLabel.text = self.fortuneDataManager.direction;
    NSString *drinks = @"";
    for (NSInteger i=0; i<[self.fortuneDataManager.drinks count]; i++)
    {
        NSString *str = [self.fortuneDataManager.drinks objectAtIndex:i];
        drinks = [drinks stringByAppendingString:str];
        if(i != [self.fortuneDataManager.drinks count]-1)
            drinks = [drinks stringByAppendingString:@","];
    }
    self.drinksLabel.text = drinks;
    self.goddessLabel.text = self.fortuneDataManager.goddess;
    self.luckyApeLabel.text = self.fortuneDataManager.luckyApe;
}

//刷新活动视图
- (void)updateActivitiesView
{
    CGFloat viewWidth = self.activitiesView.frame.size.width;       //视图宽度
    CGFloat leftMargin = 10;            //左边距
    CGFloat rightMargin = 5;            //右边距
    CGFloat spacing = 5;                //间距
    CGFloat itemHeight = 44;            //条目总高
    CGFloat titleHeight = 18;           //标题高
    CGFloat detailMaxRow = 2;           //内容最大行数
    CGFloat columnWidth = viewWidth/2;  //列宽
    
    //活动视图处理
    //清空活动视图
    NSArray *activitiesSubviews = [self.activitiesView subviews];
    for (UIView *mylabelview in activitiesSubviews)
    {
        if ([mylabelview isKindOfClass:[UILabel class]])
        {
            [mylabelview removeFromSuperview];
        }
    }
    //设置视图大小
    NSInteger itemMaxNum;
    if([self.fortuneDataManager.appropriateList count] > [self.fortuneDataManager.tabooList count])
    {
        itemMaxNum = [self.fortuneDataManager.appropriateList count];
    }
    else
    {
        itemMaxNum = [self.fortuneDataManager.tabooList count];
    }
    CGFloat activitiesViewHeight = (itemHeight+spacing) * itemMaxNum;
    self.activitiesView.frame = CGRectMake(self.activitiesView.frame.origin.x,
                                           self.activitiesView.frame.origin.y,
                                           self.activitiesView.frame.size.width,
                                           activitiesViewHeight);
    //添加活动内容
    for (int column=0; column<2; column++)
    {
        NSArray *activitiesList;
        if(column == 0)
        {
            activitiesList = self.fortuneDataManager.appropriateList;
        }
        else
        {
            activitiesList = self.fortuneDataManager.tabooList;
        }
        
        //一行
        for (int i=0; i<[activitiesList count]; i++)
        {
            NSDictionary *dict = [activitiesList objectAtIndex:i];
            UILabel *title;
            
            title = [[UILabel alloc] init];
            title.frame = CGRectMake(column*columnWidth + leftMargin, (itemHeight+spacing)*i,
                                     columnWidth - leftMargin - rightMargin, titleHeight);
            title.font = [UIFont boldSystemFontOfSize:titleHeight-2];
            title.backgroundColor = [UIColor clearColor];
            title.text = [dict objectForKey:kActivitiesTitle];
            [self.activitiesView addSubview:title];
            
            title = [[UILabel alloc] init];
            title.frame = CGRectMake(column*columnWidth + leftMargin, (itemHeight+spacing)*i+titleHeight,
                                     columnWidth - leftMargin - rightMargin, itemHeight-titleHeight);
            title.font = [UIFont systemFontOfSize:(itemHeight-titleHeight)/2 - 2];
            title.numberOfLines = detailMaxRow;
            title.backgroundColor = [UIColor clearColor];
            title.text = [dict objectForKey:kActivitiesDetail];
            [self.activitiesView addSubview:title];
        }
    }
}

@end


