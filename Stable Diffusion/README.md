## 提示词（Prompt）
提示词内输入的东西就是你想要画的东西。

## 反向提示词（Negative Prompt）
反向提示词内输入的就是你不想要画的东西。

一般负面提示：
低分辨率、错误、裁剪、最差质量、低质量、jpeg伪像、帧外、水印、签名
(lowres, error, cropped, worst quality, low quality, jpeg artifacts, out of frame, watermark, signature)

人物肖像的负面提示：
变形、丑陋、残缺、毁容、文本、额外的四肢、面部切割、头部切割、额外的手指、额外的手臂、绘制不佳的脸、突变、比例不良、头部裁剪、四肢畸形、手突变、融合手指、长脖子
(Negative prompts for people portraits: deformed, ugly, mutilated, disfigured, text, extra limbs, face cut, head cut, extra fingers, extra arms, poorly drawn face, mutation, bad proportions, cropped head, malformed limbs, mutated hands, fused fingers, long neck)

提示词助手：
https://github.com/wfjsw/danbooru-diffusion-prompt-builder

## 采样迭代步数（Steps）
一般情况下，我们使用的默认值是20个步骤，这其实已经足以生成任何类型的图像。

以下是有关在不同情况下使用steps的一般指南：

a. 如果你正在测试新提示并希望获得快速结果来调整输入，请使用10-15个steps。
b. 找到所需的提示后，将步骤增加到20-30,很多人的习惯是28。
c. 如果你正在创建带有毛皮或任何具有详细纹理的主题的面部或动物，并且觉得生成的图像缺少其中一些细节，请尝试将其提高到40或者更高。

## 采样方法（Sampler）
`Euler a` 具有更平滑的颜色和较少定义的边缘，使其更具“梦幻”外观。

`DPM2` 和 `DPM++` 系列更加写实。

`LMS`、`DPM fast` 虽然出图快，但有可能人是不完整的。

## 输出分辨率（宽度和高度）

宽高比例会直接决定画面内容，同样是1girl的例子：
1. 方图512*512，会倾向于出脸和半身像
2. 高图512*768，会倾向于出站着和坐着的全身像
3. 宽图768*512，会倾向于出斜构图的半躺像

## 提示词相关性（CFG Scale）

1. CFG 2–6：创意，但可能过于失真，没有按照提示进行操作。对于简短的提示可能很有趣且有用；
2. CFG 7–10：建议用于大多数提示。创造力和引导式生成之间的良好平衡；
3. CFG 10–15：当你确定提示很详细并且非常清楚你希望图像的外观时（有些纯风景和建筑类的图片可能需要的CFG会比较高，大家可以参考相应的模型说明）；
4. CFG 16–20：除非提示非常详细，否则通常不建议使用。可能会影响一致性和质量；
5. CFG >20：几乎从不使用。

## 随机种子（seed）

随机种子是决定初始随机噪声的数字，由于随机噪声决定了最终图像，如果多次使用相同的提示运行相同的 seed 的时候，会得到相同的生成图像。

## IMG2IMG
Img2img 功能的工作方式与 txt2img 完全相同，唯一的区别是你提供了一个用作起点的图像，而不是种子编号产生的噪声。

添加的噪声量取决于“重绘幅度（Denoising）”这个参数，该参数的范围从 `0` 到 `1`，如果设置为 `1` 完全用噪声替换图像，几乎就像你使用普通的 `txt2img` 而不是 `img2img` 一样。

a. 要创建图像的变体，建议使用的强度为 0.5-0.75，并且具有相同的提示。
b. 要更改图像样式，同时使其与原始图像相似，可以多次使用较低强度的 img2img，可以获得更好的图像保真度。
