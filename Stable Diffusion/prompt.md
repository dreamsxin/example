# 提示词

## 概述

### 画质区：用来提升整个图片的画质，图片的质量与风格。
8k, best quality（8K，最佳品质）, masterpiece（杰作），ultra highres(超高分辨率)，RAW photo，（RAW照片）, HIGHLY DETAILED（高细节）等等这些提示词用到10个以内可以明显提升画出的质感。

### 人物的写实的画质描述
1 girl,（1个女孩）detailed beautiful skin,（细致美丽的肌肤）kind smile,（亲切的微笑）absurdres,（夸张）detailed-beautiful-face,（精致美丽的容颜）petite figure,（身材娇小）detailed skin texture,（细致的皮肤纹理）pale skin,（白皙肌肤）detailed hair,（细致的头发） random hair style,（随意发型，）detailed eyes,（细致的眼睛）glistening skin,（晶莹剔透肌肤）。clean face,pretty face,sexy, cool, naughty face,((twintails)),这些我就不翻译大家可以自行百度。其中twintails用了两个括弧是权重加强。正常的情况下必出两个辫子。

### 色彩部分
VIBRANT COLORS（鲜亮色调）,VIVID COLORS（鲜艳色彩）。

### 角色描述区：
衣服、发型、肤色、表情都可以放在这个区。short sleeve shirts,（短袖衬衫）uniform,（制服）pantyhose,（连裤袜）pleated skirt,（百褶裙）ear rings,（耳环）futuristic,（未来派）studio,（工作室）white background,（白色背景）这些都可以进行细划。耳环你可以更具体的描术。这样Ai出的图会更接你想要的效果。


### 用Lora的模型来控制人物的脸型
如果只想用到基础的模型的话也可以不用。要用到Lora的模型时总的权重不能超过1.超过来就会出来奇怪的画出。<lora:koreanDollLikeness_v15:0.4><lora:TaiwanDollLikiness_v10:0.1><rola:japaneseDollLikness_v10:0.1>.0.4+0.1+0.1=0.6三个Lora模型的权重相加没超过1是没有问题的。

提示词的描术越是靠前越得到Ai的重视，在整个出图的时候会得到优先的考虑。当然后面的提示词也可以用权重来加强。上面的（（twintails））是一个例子。还有用指定倍数的方法来加强权重。（8，best quality:1.5）8K,最佳品质1.5倍。

负面提示词比较简单，就是把一些不想出现的东西加进去就可以了。

EasyNegative,Paintings, sketches, (worst quality:2), (low quality:2), (normal quality:2),lowers, normal quality, ((monochrome)), ((grayscales)), skin spots, acnes, skin blemishes,age spot, glans, (6 more fingers on one hand),(deformity), multiple breasts,(mutated hands and fingers:1.5 ), (long body :1.3), (mutation, poorly drawn :1.2) ,bad anatomy, liquid body, liquid tongue, disfigured, malformed, mutated,anatomical nonsense, text font ui, error, malformed hands, long neck, blurred, lowers,lowres, bad anatomy, bad proportions, bad shadow, uncoordinated body, unnatural body,fused breasts, bad breasts, huge breasts, poorly drawn breasts, extra breasts, liquid breasts,heavy breasts, missing breasts, huge haunch, huge thighs, huge calf, bad hands, fused hand,missing hand, disappearing arms, disappearing thigh, disappearing calf, disappearing legs,fused ears, bad ears, poorly drawn ears, extra ears, liquid ears, heavy ears, missing ears,fused animal ears, bad animal ears, poorly drawn animal ears, extra animal ears, liquid animal ears,heavy animal ears, missing animal ears, text, ui, error, missing fingers, missing limb, fused fingers,one hand with more than 5 fingers, one hand with less than 5 fingers, one hand with more than 5 digit,one hand with less than 5 digit, extra digit, fewer digits, fused digit, missing digit, bad digit, liquid digit,colorful tongue, black tongue, cropped, watermark, username, blurry, JPEG artifacts,

## 镜头距离
我们把可以使用的视角prompt 放进X/Y/Z plot 比较一下，为了突出效果要提高权重，尽量不要加入面部描述eg beautiful face，否则多数都会变成半身照。以下prompts 会加上 1.5 权重。
- extreme close-up超近镜
- close-up近镜
- cowboy shot 七分身镜头
- medium close-up 中近镜
- medium shot 中景镜
- long shot 远镜
- establishing shot 场景镜
- point-of-view 主观视角
- cowboy shot 西部牛仔常用的镜头，见到上半身以及大腿（为了见到拔枪），可以使用half body shot替代。
- medium full shot 中风全景镜
- full shot 全景镜
- portrait 肖像
- upper body 上半身
- full body 全身
- depth of field 背景虚化

再配合其他prompt 可以更稳定地控制距离。
extreme close-up，close-up跟 medium close-up 都是放大眼睛/面部为主，但有时extreme close-up会放大更多。
medium shot，long shot，medium full shot  跟full shot 看起来差不多，medium shot有时候会比full shot 更近一点，都是显示臀部以上到头顶的位置，因应场景这几个prompt 的距离可能会有些变化。
establishing shot的背景会比较明显，如果主体是建筑时人物可能会更细。
point-of-view角度会因应人物有点转变，背景通常比较wide，角度跟主体未必是同一水平视角。
cowboy shot受cowboy 污染变了牛仔装，距离跟medium full shot 差不多但距离比较稳定，一定会显示臀部以上到头顶。可以使用half body shot替代。
upper body会显示人身上半身，范围可能由臀部以上到胸以上。
full body是唯一可以见到全身的距离。
经测试后由近至远可用的镜头- extreme close-up> close-up> medium close-up> upper body> medium shot> medium full shot> full body。 而 point-of-view 跟 establishing shot 会环境有所影响所以不适合控制距离。

## 镜头角度

掌握距离然后就是角度，由最基本的前后左右再加一些摄影角度，一样加上1.5 权重，因为角度比较多我分成两张。

- from side 从侧面
- from down 貌似是正面
- front view正面 (looking at viewer,)
- bilaterally symmetrical左右对称
- side view 侧面
- back view 后面
- from above 从上拍摄
- from below 从下拍摄
- from behind 后拍
- wide angle view 广角镜
- fisheyes view 鱼眼镜
- macro view 微距
- overhead shot 俯视
- top down 由上向下
- bird's eye view 鸟瞰
- high angle 高角度
- slightly above 微高角度
- straight on 水平拍摄
- hero view 英雄视角
- low view 低视角
- worm's eye view 仰视
- selfie 自拍

出来的结果跟字面表示的角度差不多，也有些角度其实是重复的，也有一些受字面影响而受到污染。

- front view  straight on就是正面，但不一定是绝对正面，straight on因为水平拍摄的角度所以背景也不会歪。
- bilateral symmetry正面兼左右对称，比正面更准确。
- side view向左/向右都是随机的。
- back view跟 from behind 都是背面，back view会近一点，而且通常露背。
- from above overhead shot  high angle slightly above都是由高角度影向主体，overhead shot角度较高，high angle会背景比较阔一些。
- from below由下方偷拍(?) 的视角，天空通常会筒状变形。
- wide angle背景会有一些筒状变形fisheyes view 的变形效果会更强，但fisheyes view 受到污染，总会拿着相机。
- macro view变了拍花或微细的物件。
- bird's eye view从高角度影高去同时会见到广阔的背景，但会有雀鸟出现。
- top down的角度很糟糕(?)，变成正上方被女生抱住的视角。
- hero view角度不对，人物也受污染穿上了英雄战衣。
- low view角度不算很低，有点怀疑没有效果。
- worm's eye view完全错了，有很多虫及怪眼，跟角度完全没关系。
- selfie人物会伸手自拍而且不会太远。

其中fisheyes view 虽然会污染但因为视角比较特别还是有用的，但hero view 跟worm's eye view 及  macro view受污染角度又不明显可以放弃。

## 距离＋角度＝取景
```txt
1girl, beautiful, solo, (top down:1.5), (close-up1.5), lying in car

1girl, beautiful, solo, (from below:1.5), (full body:1.5), night, street

1girl, beautiful, solo, (establishing shot:1.5), (side view:1.5), in a bus, reading

1girl, beautiful, solo, (from above:1.5), (selfie:1.5), (close-up:1.5), in swimming pool
```

## 1、提升画面质量的提示词：
HDR, HD，UHD, 64K (HDR、UHD、4K、8K和64K)

表示图片效果，带来的改变可以试试，不过也会影响渲染出图的时间，会根据你要求的画面质量延长时间。

- Highly detailed 增加很多的细节，有时候描述没有那么多，随手丢进去，它会补细节。
- Studio lighting 添加和谐的靠谱一些的灯光效果，小概率加一些纹理
- Professional 会帮助自动调节对比度，色彩的和谐程度
- Vivid Colors 会帮忙增加一些鲜艳的颜色，比如用画中国画高级的配色，希望用到景泰蓝，经常会出现有点雾蒙蒙的，加入后会增强颜色的纯度和饱和度。
- Bokeh 画人像可以多尝试用这个词语，会比较突出人像
- high quality 高品质
- masterpiece 杰出
- best quality 最好品质
- photography 摄影作品
- ultra highres 超高分辨率
- RAW photo 原始照片
- ultra-detailed
- finely detail
- highres
- 8k wallpaper

反向提示
- worst quality
- low quality
- bad quality

### 示例

提示语范例一：
`Draw a glass-made bodysuit that perfectly contours the human form. The surface should be sleek and transparent, revealing a vibrant blue hue within. Utilize the stable diffusion algorithm to seamlessly blend the glass texture with the interior color.`
提示语范例二：
`Illustrate a semi-transparent glass armor with intricate details and a structured design. Let the armor emit a soft inner glow, with a delicate pink tone visible through its translucency. Employ the stable diffusion technique to capture the subtle interplay between the glass and the inner pink hue.`

`generate a semi-transparent armor using stable diffusion, revealing a vibrant red bodysuit within. The armor should appear ethereal and luminescent, with intricate details that subtlety highlight the inner red layer. Through the translucency of the armor, the red bodysuit should be visible, adding a touch of mystery and allure to the overall appearance. In the final rendering, the contrast between the solid red bodysuit and the semi-transparent armor should create a captivating and dynamic visual effect.`

正向提示词:

`(8k, best quality, masterpiece, ultra highres:1.2) Photo of Pretty Japanese woman in the (style of paul rubens and rebecca guay:1.1) (melancholy winter snow:1.4)`

反向提示词

`(worst quality:2.0)`

正向提示词：

`( (8k:1.27), best quality, masterpiece, ultra highres:1.2) Photo of Pretty Japanese woman (beautiful:1.1) (sci-fi warrior woman:1.1) space soldier, (beanie:1.61) (leather jacket:1.21) intricate elegant, fantasy, detailed, ometric by greg rutkowski and alphonse mucha, gradient lighting`

反向提示词：

`(worst quality:2.0)`

## 2、常用的反向提示词：
- worst quality
- bad quality
- low quality
- normal quality
- lowres
- normal quality

## 3、小括号用法：
小括号代表的是1.1倍，比如Exquisite Crown（精美的皇冠），加上（Exquisite Crown）就代表皇冠这个词语的权重变成1.1倍，(((Exquisite Crown)))，代表1.1x1.1x1.1，1.331倍。

## 4、中括号用法：

中括号代表的是降权，因为初始化的权重是1，用[Exquisite Crown]代表的是0.952倍。

## 5、大括号用法：

大括号代表的是1.05倍，展示方式{Exquisite Crown}。

注意：一般小括号用的比较多，可以用数字表示权重，这样就不需要中括号和大括号，比如Exquisite Crown：1.331）=(((Exquisite Crown)))

## 6、元素的融合：

### 方法一：中括号表示法

`[pink|blond]long hair，beautiful girl, gothic dress, clear details, Gothic architecture interior`

在这个里面的[pink|blond]long hair，用中括号将颜色隔开，渲染的时候，是一步粉红一步金色，最后出来的是调节过后的粉金色。中括号起到了混合的作用，同理，我们还可以用在服装材质、款式、背景玄幻...除了用中括号，另外还可以用and来连接，这是更细致的写法，可以用来规定某一个你想要混合的色彩的权重。

### 方法二：AND

`pink long hair AND blond long hair，beautiful girl, gothic dress, clear details, Gothic architecture interior`

### 方法三：

此方法被称作为元素渐变。可以通过混合两个关键词来实现更有趣效果，使用语法为“[keyword1 : keyword2: factor]”，其中factor值控制了把keyword1切换到keyword2的步骤值，是一个介于0到1之间的数字。

举个例子，输入提示词“Oil painting portrait of [Joe Biden: Donald Trump: 0.5]”，采样步数设置为30。这里指的是，第1~15步，提示词为“Oil painting portrait of Joe Biden”；第16~30步，提示词为“Oil painting portrait of Donald Trump”。解释一下，factor值决定了关键词的切换节点，设置为0.5时指的是在30*0.5 = 15步时切换。

### 采用注释
采用注释可以有效的仿制颜色污染的情况，示例：
`1girl\( (yellow hair), black eyes, smile,blue hat,white skin, (pink Sweaters:1.3)\),upper body,open hands`
对 `1girl` 进行注释。

## 7、元素的精细控制（分步描绘）：


有时候为了更好的控制画面，我们可能会要求在渲染步骤中，描绘的重点不同，以形成我们心目中的画面。

### 1.语法格式
- `[To:when] 意思是：在经过指定数量的步骤后，将位于 to 处的提示词添加到提示中。
- `[From::when]` 意思是：在经过指定数量的步骤后，从提示中删除位于 from 处的提示词。
- `[From:to:when]` 意思是：在经过指定数量的步骤后，将位于 from 处的提示词替换为 to 处的提示词。

**使用[keyword:number]方式表示**

`[flower:5],long blond hair, beautiful girl, gothic dress, clear details, Gothic architecture interior`

`[flower:5]`的意思是从第5步开始画花花，直到结束，以降低画的步数来达到弱化的效果。

但是这也有个局限，在我们画画步数本来就不高的情况下，很容易画不出来，它没办法只用10步或15步给画出来的时候，往往不理你。

此外，还有一些采样会不太搭理这种写法，可以探索看看。

小黑板：

`[flower:5]` 代表从第5步开始直到结束

`[flower::10]` 代表从开始就一起画，但是画到第10步就不画了

`[[flower::30]:5]` 代表从第5步开始画，到30步结束

长呼一口浊气，试想一下，如果我们在画画的时候，写tag能够这样精细控制，熟练掌握各个元素出现的轻重，出来的画面能多细致。

### 语法详解

我来举个例子，我采用的提示词是："a [handsome:strong:15] boy", 共采用25步。Stable Diffusion在一开始的时候运行的时候，他理解的提示词是 "a handsome boy"，经过 15 步之后，Stable Diffusion理解的提示词将被替换为 "a strong boy"。它将基于之前已经生成的第15步的"a handsome boy"的图像上进行继续计算。

接下来，我们来测试下更为复杂的用法，例如我采用提示词是： "a handsome boy wear[Medieval boots:Neon lights :0.25] and [Mechanical armor:cyberpunk clothes:0.75][ in foreground::0.6][ in background:0.25][shoddy:masterful:0.5]，"，进行 100 步的采样。

1. 一开始的提示词是 "a handsome boy wear Medieval boots and Mechanical armor in foreground shoddy"。可以看到生成了一个穿着中世纪长靴和铠甲的人，质量不佳。
2. 经过 25 步之后，提示词变为 "a handsome boy wear Neon lights and Mechanical armor in foreground in background shoddy "。提示词将中世纪长靴替换成了霓虹灯，质量不佳。
3. 经过 50 步之后，提示词变为" a handsome boy wear Neon lights and Mechanical armor in foreground in background Masterful "。可以看到中世纪长靴已经变了，加入了霓虹灯元素，画质改善的作用也出来了。
4. 经过 60 步之后，提示词变为 " a handsome boy wear Neon lights and Mechanical armor in background Masterful "。将霓虹灯元素转移至背景。
5. 经过 75 步之后，提示词变为 " a handsome boy wear Neon lights and cyberpunk clothes in background Masterful "。最终加入的赛博朋克风的衣物展示了完整的效果，背景也顺利了加入了霓虹灯元素。

## 8、画面的比重控制：
上面是控制某一个东西的比重，下面来扒画面的比重。

但是这是需要很长的步数来表现的，我今天用的不画那么多步，就写一下怎么表示。

比如说我们将步数设定在100，前面50步用来画人，后面50步用来画花花。

[girl：flower：0.5]，这样就表示前面的50%步数是画人的，后面的用来画花，人就会画到50步就结束了；

另外一种就是直接写步数，据说可以这样用，但是我觉得并不好用，写法：[girl:flower:50]，在总步数100的时候，前面50用来画人，后面的画花。

只是两种写法不一样，亲测下面的不如上面的写法好用。

## 9、元素随机选择：
这个在批量生成的时候会好用一些，一张两张的体现不出。

这里用到的是大括号。

之前的tag：

Long blond hair, beautiful girl, gothic dress, clear details, Gothic architecture interior

之后的tag：

CrownCorolla|Hairpin|Bowknot，long blond hair, beautiful girl, gothic dress, clear details, Gothic architecture interior

## 10、词汇顺序/数量/位置影响
早期的标记具有更一致的位置，因此神经网络更容易预测它们的相关性。而且由于attention机制的特殊性，每次训练时，开始的标记和结束的标记总会被注意到（attention）。而且由于标记越多，单个标记被被注意到的概率越低。

基于以上特性，有以下几点需要注意：

开头与结尾的词往往作用性更强。

提示词数量越多，单个提示词的作用性越低。

开头的数个提示词的作用较强，有更强的相关。

关于数量，你可能已经注意到了，当你写prompt时会有数量限制。

但是在 webui中，你是可以写 75 个词汇以上的提示的。webui会自动通过对提示词进行分组。当提示超过 75 个 token（可以理解token代表你的提示词），提交多组 75 个 token。单个token只具有同一组中其他内容的上下文。

每一组都会被补充至(1,77,768)的张量，然后进行合并，比如俩组就会合并为(1,154,768)的张量，然后被送入U-Net。

值得注意的是，

为了避免将你的短语分成俩组，webui在分组时会查看附近是否有,来尽量还原你想要的输入。

然后你还能通过输入BREAK来快速分组，BREAK必须为大写。

## 3D渲染

unreal engine（虚幻引擎）, octane render（Octane 渲染器）, bokeh（浅景深效果）, vray（VRay 渲染器）, houdini render（Houdini 渲染器）, quixel megascans（Quixel Megascans 材质库）, arnold render（Arnold 渲染器）, 8k uhd（8K 超高清分辨率）, raytracing（光线追踪技术）, cgi（计算机生成图像）, lumen reflections（Lumen 反射光技术）, cgsociety（CGSociety 数字艺术社区）, ultra realistic（超逼真效果）, 100mm（100 毫米焦距），film photography（胶片摄影），dslr（数码单反相机），cinema4d（Cinema 4D 三维建模软件），studio quality（工作室级别的质量），film grain（胶片颗粒感）。

## 特殊效果

analog photo（胶片摄影），polaroid（拍立得相机），macro photography（微距摄影），overglaze（高光效果），volumetric fog（体积雾效果），depth of field (or dof)（景深），silhouette（轮廓效果），motion lines（动感线条），motion blur（动态模糊效果），fisheye（鱼眼镜头），ultra-wide angle（超广角镜头）。

## 2d

digital art（数字艺术），digital painting（数字绘画），trending on artstation（在 Artstation 上流行的趋势），golden ratio（黄金分割比例），evocative（引人回忆的），award winning（获奖的），shiny（闪闪发光的），smooth（光滑的），surreal（超现实主义的），divine（神圣的），celestial（天体的），elegant（优雅的），oil painting (helps improve multiple styles)（油画（有助于提高多种风格）），soft（柔和的），fascinating（迷人的），fine art（艺术品），official art（官方艺术），keyvisual（关键视觉）。

## 特殊效果

color page（彩页），halftone（半色调），character design（角色设计），concept art（概念艺术），symmetry（对称性），pixiv fanbox (for anime/manga)（Pixiv Fanbox（用于动漫 / 漫画）），trending on dribbble (for vector graphics)（在 Dribbble 上流行的趋势（用于矢量图形）），precise lineart（精确线稿），tarot card（塔罗牌）。

## 环境

Environments（环境），stunning environment（令人惊叹的环境），wide-angle（广角），aerial view（鸟瞰视角），landscape painting（山水画），aerial photography（航拍摄影），massive scale（大规模的），street level view（街头视角），landscape（风景），panoramic（全景），lush vegetation（繁茂的植被），idyllic（田园诗般的），overhead shot（俯拍）。

## 细节

wallpaper（墙纸），poster（海报），sharp focus（锐利对焦），hyperrealism（超现实主义），insanely detailed（极度详细），lush detail（丰富的细节），filigree（金银丝细工），intricate（复杂的），crystalline（晶莹剔透的），perfectionism（完美主义），max detail（最大化细节），4k uhd（4K 超高清分辨率），spirals（螺旋形），tendrils（卷须形），ornate（华丽的），HQ（高清晰度），angelic（天使的），decorations（装饰），embellishments（装饰物），masterpiece（杰作），hard edge（硬边线条），breathtaking（惊人的），embroidery（刺绣）。

## 光照

bloom（光晕效果），god rays（神光效果），hard shadows（硬阴影），studio lighting（工作室灯光），soft lighting（柔和灯光），diffused lighting（漫射光），rim lighting（边缘光），volumetric lighting（体积光），specular lighting（高光），cinematic lighting（电影级灯光），luminescence（发光），translucency（半透明效果），subsurface scattering（次表面散射），global illumination（全局光照），indirect light（间接光），radiant light rays（光辐射线），bioluminescent details（生物发光效果），ektachrome（电影胶片效果），glowing（发光的），shimmering light（闪烁的光线），halo（光环效果），iridescent（虹彩色效果），backlighting（逆光），caustics（光线折射效果）。

## 色彩

vibrant（鲜艳的），muted colors（暗淡的颜色），vivid color（鲜明的颜色），post-processing（后期处理），colorgrading（调色），tone mapping（色调映射），lush（丰富的），low contrast（低对比度），vintage（复古的），aesthetic（美学的），psychedelic（迷幻的），monochrome（单色的）。

## 其他示例：

正向提示词：

((extreme detail)),(ultra-detailed), extremely detailed CG unity 8k wallpaper, <lora:MarinKitagawa:0.7>(anime screencap:1.2), marin kitagawa, school uniform, plaid skirt, black choker, smile, teeth, looking at viewer, sitting, crossed legs, night sky, stars, outdoors,
反向提示词：

sketches, (worst quality:2), (low quality:2), (normal quality:2), lowres, normal quality, ((monochrome)), ((grayscale)), skin spots, acnes, skin blemishes, bad anatomy,(fat:1.2), lowres,bad anatomy,bad hands, text, error, missing fingers,extra digit, fewer digits, cropped, worstquality, low quality, normal quality,jpegartifacts,signature, watermark, username,blurry,bad feet,cropped,poorly drawn hands,poorly drawn face,mutation,deformed,worst quality,low quality,normal quality,jpeg artifacts,signature,watermark,extra fingers,fewer digits,extra limbs,extra arms,extra legs,malformed limbs,fused fingers,too many fingers,long neck,cross-eyed,mutated hands,polar lowres,bad body,bad proportions,gross proportions,text,error,missing fingers,missing arms,missing legs,extra digit, extra arms, extra leg, extra foot,

正向提示词

`masterpiece, illustration,best_quality, rosaria_\(genshin_impact\), <lora:rosaria_anything_v4:0.95>, nun, fishnet, shoulder_gloves, bare_shoulder, bangs, (pray:1.2), hands together, sitting on floor, church, looking up, crying with eyes open, looking up,<lora:Moxin_10:0.2>, <lora:LORAChineseDoll_chinesedolllikeness1:0.2>, <lora:tifaMeenow_tifaV2:0.5>, red hair, red eyes, headdress, short hair, skinny, crown, [after]{zoom_enhance mask="fingers" replacement="closeup hand" max_denoising_strength=0.2 precision=80}[/after], white gloves,`

反向提示词

`wrinkles Deformed eyes, ((disfigured)), ((bad art)), ((deformed)), ((extra limbs)), (((duplicated))), ((morbid)), ((mutilated)), out of frame, extra fingers, mutated hands, poorly drawn eyes, ((poorly drawn hands)), ((poorly drawn face)), (((extra legs))), (fused fingers), (too many fingers), (((long neck))), tiling, poorly drawn, mutated, cross-eye, canvas frame, frame, cartoon, 3d, weird colors, blurry, ((old)), ((ugly)), ((child)),, NG_DeepNegative_V1_75T, (extra hand:1.4),`

其他关键词

中文

(杰作),(最好的质量),(超详细),(全身：1.3),1个女孩,赤壁,可爱的,动物耳朵,手套,兔子耳朵,高抬腿,长发,无袖的,裙子,manjuu(碧蓝航线),茶碗,怀表,卡片,无袖连衣裙,黄眼睛,白色的头发,杯子,黑手套,茶壶,张开嘴,独奏,天空,全身,花,微笑,保持,看着观众,黑色鞋履,手表,黑色长筒袜,常设,云,裸露的肩膀,黑裙子,纸牌,很长的头发,鞋,钟,茶,光线追踪,(美丽细致的脸),(美丽细致的眼睛)

英文

(masterpiece),(best quality),(ultra-detailed), (full body:1.3), 1girl,chibi,Cute,animal ears, gloves, rabbit ears, thighhighs, long hair, sleeveless, dress, manjuu (azur lane), teacup, pocket watch, card, sleeveless dress, yellow eyes, white hair, cup, black gloves, teapot, open mouth, solo, sky, flower, smile, holding, looking at viewer, black footwear, watch, black thighhighs, standing, cloud, bare shoulders, black dress, playing card, very long hair, shoes, clock, tea,Ray Tracing,(beautiful detailed face),(beautiful detailed eyes),lora:blindbox_V1Mix:1.2

反向关键词

中文

多胸，（变异的手和手指：1.5），（长身体：1.3），（变异，画得不好：1.2），黑白色，解剖学不好，液体身体，液体舌头，毁容，畸形，变异，解剖学废话， 文本字体用户界面，错误，手畸形，长脖子，模糊，降低，低分辨率，解剖学不良，比例不良，阴影不良，身体不协调，身体不自然，融合的乳房，坏乳房，巨大的乳房，绘制不良的乳房，额外的乳房， 流乳、巨乳、缺胸、大臀、大腿、大腿、烂手、融合手、缺手、消失的手臂、消失的东西、消失的小腿、消失的腿、融合的耳朵、坏耳朵、画得不好的耳朵、额外 耳朵，液体耳朵，沉重的耳朵，缺失的耳朵，融合的动物耳朵，坏的动物耳朵，画得不好的动物耳朵，额外的动物耳朵，液体动物耳朵，沉重的动物耳朵，缺失的动物耳朵，文本，UI，错误，缺少手指，缺失 四肢，手指融合，一只手多于 5 个手指，一只手少于 5 个手指，一只手多于 5 个手指，一只手少于 5 个手指，多余的手指，更少的手指，融合的手指，缺失的手指，坏的 数字，液体数字，彩色舌头，黑色舌头，裁剪，水印，用户名，模糊，JPEG伪影，签名，3D，3D游戏，3D游戏场景，3D角色，畸形脚，多余的脚，坏脚，画得不好的脚，融合 脚，缺脚，多余的鞋子，坏鞋子，融合的鞋子，两只以上的鞋子，糟糕的鞋子，糟糕的手套，糟糕的手套，融合的手套，糟糕的精液，糟糕的精液，融合的精液，糟糕的头发，糟糕的头发， 融合的头发，大肌肉，丑陋，丑陋的脸，融合的脸，画得不好的脸，克隆的脸，大脸，长脸，坏眼睛，融合的眼睛画得不好的眼睛，多余的眼睛，四肢畸形，超过2个乳头，缺少乳头，不同 乳头，融合乳头，坏乳头，画得不好的乳头，黑色乳头，彩色乳头，毛比例。 短臂，（（缺臂）），缺大腿，缺小腿，缺腿，突变，重复，病态，残缺不全，手画得不好，超过1只左手，超过1只右手，变形，（模糊），毁容， 缺腿，多余的手臂，多余的大腿，2条以上的大腿，多余的小腿，融合的小腿，多余的腿，膝盖不好，多余的膝盖，两条腿以上，尾巴不好，嘴巴不好，嘴巴融合，嘴巴不好画，舌头不好， 舌头长在嘴里，舌头太长，舌头黑，大嘴巴，嘴巴裂开，嘴巴不好，脸脏，牙齿脏，内裤脏，内裤不干净，内裤拉得不好，棉布，拉得不好，内裤不好，牙齿黄，厚 嘴唇，烂骆驼趾，五颜六色的骆驼趾，烂屁眼，烂屁眼，融合屁眼，缺屁眼，烂肛门，烂阴部，烂胯，烂裆缝 融合肛门，融合阴户，融合肛门，融合胯部，烂胯 , 熔接缝, 肛门画不好, 阴部画不好, 胯部画不好, 裆缝画不好, 大腿缝不好, 大腿缝没缝, 大腿缝熔断, 大腿缝漏液, 大腿缝画不好, 肛门画不好, 锁骨不好, 熔断 锁骨，缺失锁骨，液体锁骨，强壮的女孩，肥胖，最差质量，低质量，正常质量，液体触手，坏触手，糟糕的触手，分裂触手，融合的触手，缺失的阴蒂，坏阴蒂，融合的阴蒂，彩色阴蒂， 黑色阴蒂，液体阴蒂，二维码，条形码，审查，安全内裤，安全短裤，胡子，毛茸茸的，小马，阴毛，马赛克，排泄物，面孔，狗屎，Futa，睾丸

英文

multiple breasts, (mutated hands and fingers:1.5 ), (long body :1.3), (mutation, poorly drawn :1.2) , black-white, bad anatomy, liquid body, liquid tongue, disfigured, malformed, mutated, anatomical nonsense, text font UI, error, malformed hands, long neck, blurred, lowers, low res, bad anatomy, bad proportions, bad shadow, uncoordinated body, unnatural body, fused breasts, bad breasts, huge breasts, poorly drawn breasts, extra breasts, liquid breasts, heavy breasts, missing breasts, huge haunch, huge thighs, huge calf, bad hands, fused hand, missing hand, disappearing arms, disappearing thing, disappearing calf, disappearing legs, fused ears, bad ears, poorly drawn ears, extra ears, liquid ears, heavy ears, missing ears, fused animal ears, bad animal ears, poorly drawn animal ears, extra animal ears, liquid animal ears, heavy animal ears, missing animal ears, text, UI, error, missing fingers, missing limb, fused fingers, one hand with more than 5 fingers, one hand with less than 5 fingers, one hand with more than 5 digit, one hand with less than 5 digit, extra digit, fewer digits, fused digit, missing digit, bad digit, liquid digit, colorful tongue, black tongue, cropped, watermark, username, blurry, JPEG artifacts, signature, 3D, 3D game, 3D game scene, 3D character, malformed feet, extra feet, bad feet, poorly drawn feet, fused feet, missing feet, extra shoes, bad shoes, fused shoes, more than two shoes, poorly drawn shoes, bad gloves, poorly drawn gloves, fused gloves, bad cum, poorly drawn cum, fused cum, bad hairs, poorly drawn hairs, fused hairs, big muscles, ugly, bad face, fused face, poorly drawn face, cloned face, big face, long face, bad eyes, fused eyes poorly drawn eyes, extra eyes, malformed limbs, more than 2nipples, missing nipples, different nipples, fused nipples, bad nipples, poorly drawn nipples, black nipples, colorful nipples, gross proportions. short arm, ((missing arms)), missing thighs, missing calf, missing legs, mutation, duplicate, morbid, mutilated, poorly drawn hands,more than 1 left hand, more than 1 righthand, deformed, (blurry), disfigured, missing legs, extra arms, extra thighs, more than 2 thighs, extra calf, fused calf, extra legs, bad knee, extra knee, more than 2 legs, bad tails, bad mouth, fused mouth, poorly drawn mouth, bad tongue, tongue within mouth, too long tongue, black tongue, big mouth, cracked mouth, bad mouth, dirty face, dirty teeth, dirty pantie, fused pantie, poorly drawn pantie, fused cloth, poorly drawn cloth, bad pantie, yellow teeth, thick lips, bad camel toe, colorful camel toe, bad asshole, poorly drawn asshole, fused asshole, missing asshole, bad anus, bad pussy, bad crotch, bad crotch seam fused anus, fused pussy, fused anus, fused crotch, poorly drawn crotch, fused seam, poorly drawn anus, poorly drawn pussy, poorly drawn crotch, poorly drawn crotch seam, bad thigh gap, missing thigh gap, fused thigh gap, liquid thigh gap, poorly drawn thigh gap, poorly drawn anus, bad collarbone, fused collarbone, missing collarbone, liquid collarbone, strong girl, obesity, worst quality, low quality, normal quality, liquid tentacles, bad tentacles, poorly drawn tentacles, split tentacles, fused tentacles, missing clit, bad clit, fused clit, colorful clit, black clit, liquid clit, QR code, bar code, censored, safety panties, safety knickers, beard, furry ,pony, pubic hair, mosaic, excrement, faces, shit, Futa, testis
