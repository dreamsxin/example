目前免费的AI绘画工具：Disco Diffusion、Stable Diffusion 和 Imagen。

## web ui启动参数

### 环境变量
PYTHON：设置 Python 可执行文件的自定义路径

VENV_DIR：指定虚拟环境的路径

COMMANDLINE_ARGS：主程序的附加命令行参数

IGNORE_CMD_ARGS_ERRORS：忽略命令行参数错误，不中断退出

REQS_FILE：默认为 requirements_versions.txt，设置运行时安装具有依赖项文件的名称

TORCH_COMMAND：用于安装 PyTorch 的命令

INDEX_URL：相当于 --index-url 的参数

TRANSFORMERS_CACHE：Transformers 库下载并保存与 CLIP 模型相关的文件的路径

CUDA_VISIBLE_DEVICES：如果系统上有多个 GPU，可以指定。例如，如果您想使用辅助 GPU，请设置成1，需在 webui-user.bat 中添加新行 set CUDA_VISIBLE_DEVICES=0，或者在 COMMANDLINE_ARGS 的参数中添加 --device-id 0

### 命令行参数 / 配置类
-h, --help：显示帮助消息并退出

--exit：安装后终止

--data-dir：存储所有用户数据的基本路径，默认为根目录下

--config：用于构建模型的配置文件路径，默认为 "configs/stable-diffusion/v1-inference.yaml"

--ckpt：稳定扩散模型的检查点路径；如果指定，该路径将被添加到检查点列表并加载

--ckpt-dir：稳定扩散检查点的目录路径

--no-download-sd-model：即使没有找到模型，也不下载SD1.5模型

--vae-dir：变分自编码器模型的路径

--vae-path：用作 VAE 的检查点

--gfpgan-dir：GFPGAN 目录

--gfpgan-model：GFPGAN模型文件名

--codeformer-models-path：Codeformer模型文件的目录路径

--gfpgan-models-path：GFPGAN模型文件的目录路径

--esrgan-models-path：ESRGAN模型文件的目录路径

--bsrgan-models-path：BSRGAN模型文件的目录路径

--realesrgan-models-path：RealESRGAN模型文件的目录路径

--scunet-models-path：ScuNET模型文件的目录路径

--swinir-models-path：SwinIR和SwinIR v2模型文件的目录路径

--ldsr-models-path：LDSR模型文件的目录路径

--lora-dir：Lora网络的目录路径

--clip-models-path：CLIP模型文件的目录路径

--embeddings-dir：用于文本逆向的嵌入目录，默认为 "embeddings"

--textual-inversion-templates-dir：文本逆向模板的目录

--hypernetwork-dir：超网络目录

--localizations-dir：本地化目录

--styles-file：用于样式的文件名，默认为 "styles.csv"

--ui-config-file：用于UI配置的文件名，默认为 "ui-config.json"

--no-progressbar-hiding：不隐藏Gradio UI中的进度条（默认隐藏，因为在浏览器中使用硬件加速会降低机器学习速度）

--max-batch-count：UI的最大批次计数值，默认为16

--ui-settings-file：用于UI设置的文件名，默认为 "config.json"

--allow-code：允许从Web UI执行自定义脚本

--share：使用Gradio的share=True，并使UI通过其网站访问

--listen：使用0.0.0.0作为服务器名称启动Gradio，允许响应网络请求

--port：使用给定的服务器端口启动Gradio，需要根/管理员权限才能使用1024以下的端口

--hide-ui-dir-config：从 Web UI 隐藏目录配置

--freeze-settings：禁用编辑设置

--enable-insecure-extension-access：无论其他选项如何，都启用扩展选项卡

--gradio-debug：使用--debug选项启动Gradio

--gradio-auth：设置Gradio身份验证，如 "username:password"；或逗号分隔多个，如 "u1:p1,"u2":p2,"u3":p3"

--gradio-auth-path：设置Gradio身份验证文件路径，例如 "/path/to/auth/file"，与--gradio-auth的格式相同

--disable-console-progressbars：不在控制台输出进度条。

--enable-console-prompts：在使用txt2img和img2img生成时，在控制台打印提示

--api：使用API启动Web UI

--api-auth：设置API身份验证，如 "username:password"；或逗号分隔多个，如 "u1:p1,"u2":p2,"u3":p3"

--api-log：启用所有API请求的日志记录

--nowebui：仅启动API，不启动UI

--ui-debug-mode：不加载模型以快速启动UI

--device-id：选择要使用的默认CUDA设备（可能需要在此之前设置CUDA_VISIBLE_DEVICES=0,1等环境变量）

--administrator：管理员权限

--cors-allow-origins：以逗号分隔的列表形式允许的CORS来源

--cors-allow-origins-regex：以单个正则表达式的形式允许的CORS来源

--tls-keyfile：部分启用TLS，需要--tls-certfile才能完全生效

--tls-certfile：部分启用TLS，需要--tls-keyfile才能完全生效

--disable-tls-verify：通过后，即可使用自签名证书

--server-name：设置服务器主机名

--no-gradio-queue：禁用渐变队列；导致网页使用http请求而不是websocket；是早期版本中的默认值

--no-hashing：禁用检查点的 SHA-256 哈希以帮助加载性能

--skip-version-check：不检查torch和xformers的版本

--skip-python-version-check：不检查 python 版本

--skip-torch-cuda-test：不检查CUDA是否能够正常工作

--skip-install：跳过软件包的安装

### 命令行参数 / 性能类
--xformers：启用xformers，加快图像的生成速度

--force-enable-xformers：强制启动xformers，无论是否可以运行都不报错

--xformers-flash-attention：启用带有 Flash Attention 的 xformers 以提高再现性（仅支持 SD2.x）

--opt-sdp-attention：启用缩放点积交叉注意层优化；需要 PyTorch 2.*

--opt-sdp-no-mem-attention：启用缩放点积交叉注意力层优化，无需内存高效注意力，使图像生成具有确定性；需要 PyTorch 2.*

--opt-split-attention：强制启用 Doggettx 的交叉注意力层优化。默认情况下，它在启用 CUDA 的系统上处于打开状态

--opt-split-attention-invokeai：强制启用 InvokeAI 的交叉注意力层优化。默认情况下，当 CUDA 不可用时它会打开

--opt-split-attention-v1：启用旧版本的分割注意力优化，不会消耗所有可用的 VRAM

--opt-sub-quad-attention：实现内存高效的子二次交叉注意力层优化

--sub-quad-q-chunk-size：用于子二次交叉注意力层优化的查询块大小

--sub-quad-kv-chunk-size：用于子二次交叉注意力层优化的 KV 块大小

--sub-quad-chunk-threshold：使用分块进行子二次交叉注意力层优化的 VRAM 阈值的百分比

--opt-channelslast：启用 4d 张量的替代布局，可能会导致仅在具有 Tensor 核心（16xx 及更高）的 Nvidia 卡上更快的推理

--disable-opt-split-attention：强制禁用跨注意层优化

--disable-nan-check：不检查生成的图像/潜在空间是否包含nan值；在持续集成中运行时无需检查点

--use-cpu：对指定模块使用CPU作为torch设备

--no-half：不将模型切换为16位浮点数

--precision：以此精度进行评估

--no-half-vae：不将VAE模型切换为16位浮点数

--upcast-sampling：向上采样。与 --no-half 无效。通常产生与 --no-half 类似的结果，但在使用较少内存的情况下性能更好

--medvram：启用稳定扩散模型优化，牺牲一点速度以减少显存使用

--lowvram：启用稳定扩散模型优化，牺牲大量速度以极低的显存使用

--lowram：将稳定扩散检查点权重加载到显存而非RAM

--always-batch-cond-uncond：禁用使用 --medvram 或 --lowvram 时为节省内存而启用的条件/无条件批处理

### 命令行参数 / 通用类

--autolaunch：在启动时使用系统的默认浏览器打开WebUI URL

--theme：在WebUI中使用指定的主题light/dark。如果未指定，则使用浏览器的默认主题

--use-textbox-seed：在UI中使用文本框输入种子（没有上/下箭头，但可以输入长种子）

--disable-safe-unpickle：禁用对PyTorch模型的恶意代码检查

--ngrok：用于ngrok的自动令牌，是gradio --share的替代方案

--ngrok-region：ngrok应该在其中启动的区域

--update-check：启动时，通知您的 Web UI 版本（提交）是否与当前主分支保持同步

--update-all-extensions：启动时，它会提取您已安装的所有扩展的最新更新

--reinstall-xformers：强制重新安装 xformers。对于升级很有用 - 但升级后将其删除，否则您将永久重新安装 xformers

--reinstall-torch：强制重新安装割炬。对于升级很有用 - 但升级后将其删除，否则您将永久重新安装 torch

--tests：运行测试以验证 Web UI 功能，请参阅 wiki 主题了解更多详细信息

--no-tests：即使指定了 --tests 选项，也不运行测试

### 命令行参数 / 已过期（不再有效果）

--show-negative-prompt：显示否定提示

--deepdanbooru：二次元TAG解析

--unload-gfpgan：卸载 gfpgan

--gradio-img2img-tool：图生图处理工具

--gradio-inpaint-tool：重绘处理工具

--gradio-queue：梯度队列

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
- https://github.com/tinygeeker/autocue
- https://github.com/wfjsw/danbooru-diffusion-prompt-builder

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
