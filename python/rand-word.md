```python
from faker import Faker

fake = Faker()

# 生成随机单词
random_word = fake.word()

# 生成多个随机单词
random_words = fake.words(10)

# 生成特定领域的术语
tech_terms = [fake.word(ext_word_list=None) for _ in range(5)]  # 可以传入自定义词表

print(f"单个词: {random_word}")
print(f"多个词: {random_words}")
```

```python
# 需要先安装: pip install random-word
from random_word import RandomWords

r = RandomWords()

# 获取单个随机词
random_word = r.get_random_word()

# 获取多个随机词
random_words = r.get_random_words()

print(random_word)
print(random_words)
```
