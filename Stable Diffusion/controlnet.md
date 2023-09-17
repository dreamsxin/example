# 各模型作用总结

## depth模型

作用 ：该模型可以识别图片主体，依据图片主体生成不同背景的图片变种，类似于抠图 (prompt: handsom man )

## normal模型

该模型作用和depth 模型类似

示例图片 ：(prompt: man in flower/house)

## canny模型

作用 ：通过canny 模型识别图片内容为线稿，来控制 sd 图片生成内容

示例图片 ：(prompt: dog in a room)

## openpose模型

作用 ： 通过 openpose 模型识别人物图片的头，手，身体的姿势来控制 sd 图片的生成

示例图片 ：（prompt: man in suit ）

## Segmentation模型

作用 ： 通过 segmentation 模型精准分割图片上的不同的物体和内容，并以此来控制 sd 图片的生成

示例图片 ：（prompt ： house ）

## Anime Lineart模型

作用 ： 通过该模型识别动漫线稿，并以此控制 sd 生成类似动漫图片

示例图片 ：（prompt ： 1girl, in classroom, skirt, uniform, red hair, bag, green eyes ）

## Shuffle模型

作用：通过该模型重新对图片进行打乱重组，来生成新的图片

示例图片：（prompt ： hong kong ）

## Tile模型

作用 ：通过该模型可以提高原图的分辨率的同时给原图填充更多的细节，适合搭配 sd 的高清修复选项，非常适合生成4k 高质量图片

示例效果 (ps:图片只截取了部分 ) ：（ prompt : old man ）


