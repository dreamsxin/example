##

```python
from flask import Flask, request, jsonify
import torch
from transformers import pipeline, AutoTokenizer, StoppingCriteria

model = "databricks/dolly-v2-3b"
tokenizer = AutoTokenizer.from_pretrained(model, padding_side="left")
dolly = pipeline(model=model, tokenizer=tokenizer, trust_remote_code=True, device_map="auto")
device = dolly.device

class CheckStop(StoppingCriteria):
    def __init__(self, stop=None):
        super().__init__()
        self.stop = stop or []
        self.matched = ""
        self.stop_ids = [tokenizer.encode(s, return_tensors='pt').to(device) for s in self.stop]
    def __call__(self, input_ids: torch.LongTensor, scores: torch.FloatTensor, **kwargs):
        for i, s in enumerate(self.stop_ids):
            if torch.all((s == input_ids[0][-s.shape[1]:])).item():
                self.matched = self.stop[i]
                return True
        return False

def llm(prompt, stop=None, **kwargs):
  check_stop = CheckStop(stop)
  result = dolly(prompt, stopping_criteria=[check_stop], **kwargs)
  return result[0]["generated_text"].rstrip(check_stop.matched)

app = Flask("dolly")

@app.route('/', methods=['POST'])
def serve_llm():
  resp = llm(**request.json)
  return jsonify(resp)

app.run(host="0.0.0.0", port="7777")
```

## ChatGLM 集成进 LangChain 工具

https://python.langchain.com/docs/integrations/llms/chatglm

https://python.langchain.com/docs/integrations/llms/databricks#wrapping-a-serving-endpoint-custom-model

