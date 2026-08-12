## 安装

```shell
git clone https://github.com/harrrshall/barunlm-35m.git
cd barunlm-35m
python -m venv .venv
source .venv/bin/activate
pip install -e .
python examples/generate.py \
  --prompt "The future of efficient language models is" \
  --max-new-tokens 48 \
  --temperature 0.8
```

windows:
```shell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -e .
$env:PYTHONUTF8=1
python examples\generate.py --prompt "The future of efficient language models is" --max-new-tokens 48 --temperature 0.8
python examples\generate.py `
  --prompt "The future of efficient language models is" `
  --max-new-tokens 48 `
  --temperature 0.8
```
