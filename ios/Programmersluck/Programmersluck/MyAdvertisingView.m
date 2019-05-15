//
//  MyAdvertisingView.m
//  LiftStatistics
//
//  Created by cuibo on 8/23/12.
//
//

#import "MyAdvertisingView.h"

@interface MyAdvertisingView()

@property(strong, nonatomic)UIImageView *backgroundImg;
@property(strong, nonatomic)UILabel *titleLable;
@property(strong, nonatomic)UILabel *contentLable;

//被点击
- (IBAction)touchUpInside:(id)sender;

@end

@implementation MyAdvertisingView

@synthesize backgroundImg;
@synthesize titleLable;
@synthesize contentLable;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self)
    {
        // Initialization code
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self)
    {
        // Initialization code
        //背景图片
        //self.backgroundColor = [UIColor colorWithRed:0.7 green:0.7 blue:0.7 alpha:1];
        //self.backgroundImg = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"AdmobBackGround.png"]];
        //self.backgroundImg.frame = CGRectMake(0, 0, self.frame.size.width, self.frame.size.height);
        //self.backgroundImg.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
        //self.backgroundImg.userInteractionEnabled = NO;
        //[self addSubview:self.backgroundImg];
        //标题
        self.titleLable = [[UILabel alloc] init];
        self.titleLable.text = NSLocalizedString(@"Lofocus", @"广告占位－广告标题");
        self.titleLable.font = [UIFont systemFontOfSize:17];
        self.titleLable.textColor = [UIColor blackColor];
        self.titleLable.backgroundColor = [UIColor clearColor];
        self.titleLable.frame = CGRectMake(10, 10, self.frame.size.width-20, 20);
        self.titleLable.autoresizingMask = UIViewAutoresizingFlexibleWidth;
        [self addSubview:self.titleLable];
        //内容
        self.contentLable = [[UILabel alloc] init];
        self.contentLable.text = NSLocalizedString(@"TechnicalSupport - weibo:@lofocus", @"广告占位－广告内容");
        self.contentLable.font = [UIFont systemFontOfSize:13];
        self.contentLable.textColor = [UIColor blackColor];
        self.contentLable.backgroundColor = [UIColor clearColor];
        self.contentLable.frame = CGRectMake(10, 30, self.frame.size.width-20, 15);
        self.contentLable.autoresizingMask = UIViewAutoresizingFlexibleWidth;
        [self addSubview:self.contentLable];

        [self addTarget:self
                       action:@selector(touchUpInside:)
             forControlEvents:UIControlEventTouchUpInside];

        //self.backgroundColor = [UIColor blueColor];
    }
    return self;
}

- (IBAction)touchUpInside:(id)sender
{
    //[[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithFormat:NSLocalizedString(@"https://twitter.com/lofocusInc", @"广告占位－广告链接")]]];
}

@end


