## 安装

```shell
sudo apt-get install tesseract-ocr
sudo apt-get install tesseract-ocr-eng
sudo apt-get install tesseract-ocr-chi-sim
```

## 使用

识别文件 `test.png` 输出结构到 `out.txt`

```shell
tesseract test.png out -l eng
tesseract test.png out -l chi_sim
```