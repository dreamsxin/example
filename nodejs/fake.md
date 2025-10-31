# Node.js 中生成随机关键词的开源库

以下是 Node.js 中用于生成随机关键词的常用开源库：

## 1. Faker.js (现在为 @faker-js/faker)

```bash
npm install @faker-js/faker
```

```javascript
const { faker } = require('@faker-js/faker');

// 生成单个随机词
const randomWord = faker.word.sample();
console.log(randomWord);

// 生成多个随机词
const randomWords = faker.word.words(5);
console.log(randomWords);

// 生成特定类型的词
const adjective = faker.word.adjective();
const noun = faker.word.noun();
const verb = faker.word.verb();

// 生成短语
const phrase = faker.word.words({ count: 3 });
```

## 2. Chance

```bash
npm install chance
```

```javascript
const Chance = require('chance');
const chance = new Chance();

// 生成随机单词
const word = chance.word();
console.log(word);

// 生成多个单词
const words = chance.words({ count: 5 });
console.log(words);

// 生成特定长度的单词
const longWord = chance.word({ length: 8 });
console.log(longWord);

// 生成句子中的单词
const sentence = chance.sentence({ words: 5 });
console.log(sentence);
```

## 3. Random-words

```bash
npm install random-words
```

```javascript
const randomWords = require('random-words');

// 生成单个随机词
const word = randomWords();
console.log(word);

// 生成多个随机词
const words = randomWords(5);
console.log(words);

// 生成特定格式的词
const exactCount = randomWords({ exactly: 3 });
const maxWords = randomWords({ max: 10 });
const minMaxWords = randomWords({ min: 2, max: 5 });

// 生成词组
const wordList = randomWords({ min: 3, max: 5, join: ' ' });
console.log(wordList);
```

## 4. Lorem-ipsum

```bash
npm install lorem-ipsum
```

```javascript
const loremIpsum = require('lorem-ipsum').loremIpsum;

// 生成随机单词
const word = loremIpsum({ units: 'words', count: 1 });
console.log(word);

// 生成多个单词
const words = loremIpsum({ units: 'words', count: 5 });
console.log(words);

// 生成句子（可提取关键词）
const sentence = loremIpsum({ units: 'words', count: 8 });
console.log(sentence);
```

## 5. Unique-random-array + 自定义词库

```bash
npm install unique-random-array
```

```javascript
const uniqueRandomArray = require('unique-random-array');

// 自定义词库
const keywordList = [
  'javascript', 'nodejs', 'programming', 'development',
  'web', 'api', 'database', 'framework', 'library',
  'frontend', 'backend', 'fullstack', 'typescript'
];

const randomKeyword = uniqueRandomArray(keywordList);

// 生成不重复的关键词
console.log(randomKeyword());
console.log(randomKeyword());
console.log(randomKeyword());

// 生成多个不重复关键词
function getMultipleKeywords(count) {
  const shuffled = [...keywordList].sort(() => 0.5 - Math.random());
  return shuffled.slice(0, count);
}

console.log(getMultipleKeywords(5));
```

## 6. Random-word-generator

```bash
npm install random-word-generator
```

```javascript
const RandomWordGenerator = require('random-word-generator');
const randomWord = new RandomWordGenerator();

// 生成随机词
const word = randomWord.generate();
console.log(word);

// 生成多个词
const words = [];
for (let i = 0; i < 5; i++) {
  words.push(randomWord.generate());
}
console.log(words);
```

7. 从文件加载词库的完整示例

```javascript
const fs = require('fs');
const readline = require('readline');

class KeywordGenerator {
  constructor(wordList = []) {
    this.wordList = wordList;
  }

  static async fromFile(filePath) {
    const wordList = [];
    const fileStream = fs.createReadStream(filePath);
    
    const rl = readline.createInterface({
      input: fileStream,
      crlfDelay: Infinity
    });

    for await (const line of rl) {
      if (line.trim()) {
        wordList.push(line.trim());
      }
    }

    return new KeywordGenerator(wordList);
  }

  getRandomWord() {
    if (this.wordList.length === 0) {
      throw new Error('Word list is empty');
    }
    return this.wordList[Math.floor(Math.random() * this.wordList.length)];
  }

  getRandomWords(count = 5) {
    const result = [];
    for (let i = 0; i < count; i++) {
      result.push(this.getRandomWord());
    }
    return result;
  }
}

// 使用示例
(async () => {
  try {
    const generator = await KeywordGenerator.fromFile('keywords.txt');
    console.log('随机关键词:', generator.getRandomWord());
    console.log('多个关键词:', generator.getRandomWords(3));
  } catch (error) {
    console.error('错误:', error.message);
  }
})();
```

推荐使用

· 通用场景: 使用 @faker-js/faker 或 chance
· 简单需求: 使用 random-words
· 专业文本: 使用 lorem-ipsum
· 自定义词库: 使用 unique-random-array 配合自己的词库