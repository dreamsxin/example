# stable-diffusion.cpp

Inference of Stable Diffusion in pure C/C++

Plain C/C++ implementation based on ggml, working in the same way as llama.cpp

## 下载安装

```shell
git clone --recursive https://github.com/leejet/stable-diffusion.cpp
cd stable-diffusion.cpp
git pull origin master
git submodule init
git submodule update
```

### Build
Build from scratch
```shell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
Using OpenBLAS
```shell
cmake .. -DGGML_OPENBLAS=ON
cmake --build . --config Release
```

## 下载模型

onvert weights
download original weights(.ckpt or .safetensors). For example

- Stable Diffusion v1.4 from https://huggingface.co/CompVis/stable-diffusion-v-1-4-original
- Stable Diffusion v1.5 from https://huggingface.co/runwayml/stable-diffusion-v1-5
- Stable Diffuison v2.1 from https://huggingface.co/stabilityai/stable-diffusion-2-1

```shell
curl -L -O https://huggingface.co/CompVis/stable-diffusion-v-1-4-original/resolve/main/sd-v1-4.ckpt
# curl -L -O https://huggingface.co/runwayml/stable-diffusion-v1-5/resolve/main/v1-5-pruned-emaonly.safetensors
# curl -L -o https://huggingface.co/stabilityai/stable-diffusion-2-1/blob/main/v2-1_768-nonema-pruned.safetensors
```
### 模型格式转换
convert weights to ggml model format
```shell
cd models
pip install -r requirements.txt
python convert.py [path to weights] --out_type [output precision]
# For example
python convert.py sd-v1-4.ckpt --out_type f16
```
## Run

```cmd
usage: ./bin/sd [arguments]

arguments:
  -h, --help                         show this help message and exit
  -M, --mode [txt2img or img2img]    generation mode (default: txt2img)
  -t, --threads N                    number of threads to use during computation (default: -1).
                                     If threads <= 0, then threads will be set to the number of CPU physical cores
  -m, --model [MODEL]                path to model
  -i, --init-img [IMAGE]             path to the input image, required by img2img
  -o, --output OUTPUT                path to write result image to (default: .\output.png)
  -p, --prompt [PROMPT]              the prompt to render
  -n, --negative-prompt PROMPT       the negative prompt (default: "")
  --cfg-scale SCALE                  unconditional guidance scale: (default: 7.0)
  --strength STRENGTH                strength for noising/unnoising (default: 0.75)
                                     1.0 corresponds to full destruction of information in init image
  -H, --height H                     image height, in pixel space (default: 512)
  -W, --width W                      image width, in pixel space (default: 512)
  --sampling-method {euler, euler_a, heun, dpm++2m, dpm++2mv2}
                                     sampling method (default: "euler_a")
  --steps  STEPS                     number of sample steps (default: 20)
  --rng {std_default, cuda}          RNG (default: cuda)
  -s SEED, --seed SEED               RNG seed (default: 42, use random seed for < 0)
  -v, --verbose                      print extra info
```
txt2img example

```shell
./bin/sd -m ../models/sd-v1-4-ggml-model-f16.bin -p "a lovely cat"
```
						
img2img example
```shell
./output.png is the image generated from the above txt2img pipeline
./bin/sd --mode img2img -m ../models/sd-v1-4-ggml-model-f16.bin -p "cat with blue eyes" -i ./output.png -o ./img2img_output.png --strength 0.4
```
