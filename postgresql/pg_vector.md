- https://github.com/pgvector/pgvector.git
- https://github.com/tmc/langchaingo
##
```psql
CREATE EXTENSION IF NOT EXISTS vector;
CREATE TABLE items (id bigserial PRIMARY KEY, name, features vector(3));
INSERT INTO items (features) VALUES ('[1,2,3]'), ('[4,5,6]');
INSERT INTO items (id, features) VALUES (1, '[1,2,3]'), (2, '[4,5,6]') ON CONFLICT (id) DO UPDATE SET features = EXCLUDED.features;
```
## 查询运算符

- <-> - L2 distance
- <#> - (negative) inner product
- <=> - cosine distance
- <+> - L1 distance (added in 0.7.0)

- <->：该运算符计算两个向量之间的欧几里德距离。欧几里德距离是多维空间中向量表示的点之间的直线距离。较小的欧几里德距离表示向量之间的相似性较大，因此该运算符在查找和排序相似项目时非常有用。
```psql
SELECT id, name, features, features <-> '[0.45, 0.4, 0.85]' as distance
2FROM items
3ORDER BY features <-> '[0.45, 0.4, 0.85]';
```
- <=>：该运算符计算两个向量之间的余弦相似度。余弦相似度比较两个向量的方向而不是它们的大小。余弦相似度的范围在 -1 到 1 之间，1 表示向量相同，0 表示无关，-1 表示向量指向相反方向。
```psql
SELECT id, name, features, features <=> '[0.45, 0.4, 0.85]' as similarity
2FROM items
3ORDER BY features <=> '[0.45, 0.4, 0.85]' DESC;
```
- <#>：该运算符计算两个向量之间的曼哈顿距离（也称为 L1 距离或城市街区距离）。曼哈顿距离是每个维度对应坐标差的绝对值之和。相对于欧几里德距离而言，曼哈顿距离更加强调沿着维度的较小移动。
```psql
SELECT id, name, features, features <#> '[0.45, 0.4, 0.85]' as distance
2FROM items
3ORDER BY features <#> '[0.45, 0.4, 0.85]';p
```
## 索引
为要使用的每个距离函数添加一个索引。
**L2距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_l2_ops) WITH (lists = 100);
```
**L1距离**
```psql
create index on items using hnsw (embedding vector_l1_ops);
```
**汉明距离**
```psql
create index on items using hnsw (embedding bit_hamming_ops);
```
**内积距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_ip_ops) WITH (lists = 100);
```
**余弦距离**
```psql
CREATE INDEX ON items USING ivfflat (embedding vector_cosine_ops) WITH (lists = 100);
```
**杰卡德距离**
```psql
create index on vector using hnsw (vector bit_jaccard_ops);
```

## 核心概念及原理

### 嵌入向量（Embedding Vectors）
向量 Embedding 是在自然语言处理和机器学习中广泛使用的概念。各种文本、图片或其他信号，均可通过一些算法转换为向量化的 Embedding。在向量空间中，相似的词语或信号距离更近，可以用这种性质来表示词语或信号之间的关系和相似性。例如，通过一定的向量化模型算法，将如下三句话，转换成二维向量（x，y），我们可通过坐标系来画出这些向量的位置，它们在二维坐标中的远近，就显示了其相似性，坐标位置越接近，其内容就越相似。

### Prompt Engineering 过程原理
使用者需要不断调整输入提示，从而获得相关领域的专业回答。输入模型的相关提示内容越接近问题本身，模型的输出越趋近于专业水平。通俗理解就是，模型能够利用所输入的提示信息，从中抽取出问题的答案，并总结出一份专业水准的回答。

其大致可以分为两个阶段：
1. 向量构建阶段和问答阶段。在向量构建阶段，将企业知识库的所有文档，分割成内容大小适当的片段，然后通过 Embeddings 转换算法，将转化的 Embeddings 数据，存储于云数据库 PostgreSQL 版向量数据库中
2. 在问答阶段，问答系统首先接收用户的提问，并将其转化为 Embedding 数据，然后通过与向量化的问题进行相似性检索，获取最相关的 TOP N 的知识单元。接着，通过 Prompt 模板将问题、最相关的 TOP N 知识单元、历史聊天记录整合成新的问题。大语言模型将理解并优化这个问题，然后返回相关结果。最后，系统将结果返回给提问者。

## 实现过程
接下来将介绍如何利用云数据库 PostgreSQL 版提供的 pg_vector 插件构建用于向量高效存储、检索的向量数据库。

### 训练步骤
本文将以构建企业专属“数据库顾问”问答系统为例，演示整个构建过程。使用的知识库样例为 PostgreSQL 15 官方文档

以下过程包括两个主要脚本文件，构建知识库的 generate-embeddings.ts，问答脚本 queryGPT.py，建议组织项目目录如下所示：
```txt
.
├── package.json                              // ts依赖包
├── docs
│   ├── PostgreSQL15.mdx                      // 知识库文档
├── script
│   ├── generate-embeddings.ts                // 构建知识库
│   ├── queryGPT.py                           // 问答脚本
```

### 学习阶段
- https://github.com/supabase-community/nextjs-openai-doc-search
- https://github.com/lvwzhen/law-cn-ai/blob/main/lib/generate-embeddings.ts
```ts
          const configuration = new Configuration({
            apiKey: process.env.OPENAI_KEY,
          })
          const openai = new OpenAIApi(configuration)

          const embeddingResponse = await openai.createEmbedding({
            model: 'text-embedding-ada-002',
            input,
          })

          if (embeddingResponse.status !== 200) {
            throw new Error(inspect(embeddingResponse.data, false, 2))
          }

          const [responseData] = embeddingResponse.data.data

          const { error: insertPageSectionError, data: pageSection } = await supabaseClient
            .from('nods_page_section')
            .insert({
              page_id: page.id,
              slug,
              heading,
              content,
              token_count: embeddingResponse.data.usage.total_tokens,
              embedding: responseData.embedding,
            })
            .select()
            .limit(1)
            .single()
```
#### 1. 创建 PostgreSQL 实例
#### 2. 创建插件
```sql
create extension if not exists vector;
```
#### 3. 创建对应的数据库表，其中表 doc_chunks 中的字段 embedding 即为表示知识片段的向量。
```sql
-- 记录文档信息
create table docs (
  id bigserial primary key,
  -- 父文档ID
  parent_doc bigint references docs,
  -- 文档路径
  path text not null unique,
  -- 文档校验值
  checksum text
);
-- 记录chunk信息
create table doc_chunks (
  id bigserial primary key,
  doc_id bigint not null references docs on delete cascade, -- 文档ID
  content text, -- chunk内容
  token_count int, -- chunk中的token数量
  embedding vector(1536), -- chunk转化成的embedding向量
  slug text, -- 为标题生成唯一标志
  heading text -- 标题
);
```
#### 4. 构建向量知识库
在客户端机器上，将知识库文档内容，分割成内容大小适当的片段，通过 OpenAI 的 embedding 转化接口，转化成embedding 向量，并存储到数据库，参考脚本获取方式详见文末。
>> 注意 该脚本只能处理 markdown 格式的文件。
**安装 pnpm：**
```shell
curl -fsSL https://get.pnpm.io/install.sh | sh -
```
**安装nodejs**
```shell
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg
NODE_MAJOR=16
echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_$NODE_MAJOR.x nodistro main" | sudo tee /etc/apt/sources.list.d/nodesource.list
sudo apt-get update
sudo apt-get install nodejs -y
```
**安装 typescript 依赖**
```shell
pnpm run setup
pnpm install tsx
```
修改 generate-embeddings.ts，设置 OpenAI 的 key、PG 的连接串以及 markdown 文档目录：
```shell
#这里需要将user、passwd、127.0.0.1、5432 替换为实际数据库用户、密码、数据库地址、端口
const postgresql_url = 'pg://user:passwd@127.0.0.1:5432/database';
const openai_key = '-------------';
const SOURCE_DIR = path.join(__dirname, 'document path');
```
运行脚本，生成文档 embedding 向量并插入数据库：
```shell
pnpm tsx script/generate-embeddings.ts
```
### 问答阶段
#### 1. 创建相似度计算函数
为了方便应用使用，使用 PostgreSQL 的自定义函数功能，创建内置于数据库内的函数。应用只需调用 PostgreSQL，该函数便可在应用程序中获取向量匹配结果。示例中使用“内积”来计算向量的相似性。
```sql
create or replace function match_chunks(chunck_embedding vector(1536), threshold float, count int, min_length int)
returns table (id bigint, content text, similarity float)
language plpgsql
as $$
begin
  return query
  select
    doc_chunks.id,
    doc_chunks.content,
    (doc_chunks.embedding <#> chunck_embedding) * -1 as similarity
  from doc_chunks

  -- chunk内容大于设定的长度
  where length(doc_chunks.content) >= min_length

  -- The dot product is negative because of a Postgres limitation, so we negate it
  and (doc_chunks.embedding <#> chunck_embedding) * -1 > threshold
  order by doc_chunks.embedding <#> chunck_embedding
  
  limit count;
end;
$$;
```
#### 2. 提问及回答
以下 Python 程序，可以接收提问者问题，并实现上述 Prompt Engineering 的“问答阶段”的功能，最终将具备“逻辑思考”+“深度领域知识”的解答，发送给提问者。
```python
import os, psycopg2, openai

def query_handler(query = None):
    if query is None or query == "":
        print('请输入有效问题')
        return

    query = query.strip().replace('\n', ' ')
    embedding = None
    try:
      # 使用 GPT 将提问转化为 embedding 向量
        response = openai.Embedding.create(
            engine="text-embedding-ada-002", # 固定为text-embedding-ada-002
            input=[query],
        )
        embedding = response.data[0].embedding
    except Exception as ex:
        print(ex)
        return

    content = ""
    con = None
    try:
      # 处理 postgres 配置，连接数据库
      # host:127.0.0.1,port:5432,user:test,password:test,database:test
        params = postgresql_url.split(',')
        database, user, password, host, port = "test", "test", "test", "127.0.0.1", "5432"
        for param in params:
            pair = param.split(':')
            if len(pair) != 2:
                print('POSTGRESQL_URL error: ' + postgresql_url)
                return
            k, v = pair[0].strip(), pair[1].strip()
            if k == 'database':
                database = v
            elif k == 'user':
                user = v
            elif k == 'password':
                password = v
            elif k == 'host':
                host = v
            elif k == 'port':
                port = v
      # connect postgres
        con = psycopg2.connect(database=database, user=user, password=password, host=host, port=port)
        cur = con.cursor()
      # 从数据库查询若干条最接近提问的 chunk
        sql = "select match_chunks('[" + ','.join([str(x) for x in embedding]) + "]', 0.78, 5, 50)"
        cur.execute(sql)
        rows = cur.fetchall()
        for row in rows:
            row = row[0][1:-2].split(',')[-2][1:-2].strip()
            content = content + row + "\n---\n"

    except Exception as ex:
        print(ex)
        return

    finally:
        if con is not None:
            con.close()

    try:
      # 组织提问和 chunk 内容，发送给 GPT
        prompt = '''Pretend you are GPT-4 model , Act an database expert.
        I will introduce a database scenario for which you will provide advice and related sql commands.
        Please only provide advice related to this scenario. Based on the specific scenario from the documentation,
        answer the question only using that information. Please note that if there are any updates to the database
        syntax or usage rules, the latest content shall prevail. If you are uncertain or the answer is not explicitly
        written in the documentation, please respond with "I'm sorry, I cannot assist with this.\n\n''' + "Context sections:\n" + \
        content.strip().replace('\n', ' ') + "\n\nQuestion:"""" + query.replace('\n', ' ') + """"\n\nAnswer:"

        print('\n正在处理，请稍后。。。\n')
        response = openai.ChatCompletion.create(
            engine="gpt_openapi", # 固定为gpt_openapi
            messages=[
                {"role": "user", "content": prompt}
            ],
            model="gpt-35-turbo",
            temperature=0,
        )
        print('回答:')
        print(response['choices'][0]['message']['content'])

    except Exception as ex:
        print(ex)
        return

os.environ['OPENAI_KEY'] = '-----------------------'
os.environ['POSTGRESQL_URL'] = 'host:127.0.0.1,port:5432,user:test,password:test,database:test'
openai_key = os.getenv('OPENAI_KEY')
postgresql_url = os.getenv('POSTGRESQL_URL')# openai config
openai.api_type = "azure"
openai.api_base = "https://example-endpoint.openai.azure.com"
openai.api_version = "2023-XX"
openai.api_key = openai_key

def main():
    if openai_key is None or postgresql_url is None:
        print('Missing environment variable OPENAI_KEY, POSTGRESQL_URL(host:127.XX.XX.XX,port:5432,user:XX,password:XX,database:XX)')
        return
    print('我是您的PostgreSQL AI助手，请输入您想查询的问题，例如：\n1、如何创建table？\n2、给我解释一下select语句？\n3、如何创建一个存储过程？')
    while True:
        query = input("\n输入您的问题:")
        query_handler(query)
        
if __name__ == "__main__":
    main()
```
通过修改机器人自我介绍，以让提问者快速了解问答机器人的专业特长，这里的自我介绍，说明机器人是一个数据库专家的角色。
```python
prompt = '''Pretend you are GPT-4 model , Act an database expert.
        I will introduce a database scenario for which you will provide advice and related sql commands.
        Please only provide advice related to this scenario. Based on the specific scenario from the documentation,
        answer the question only using that information. Please note that if there are any updates to the database
        syntax or usage rules, the latest content shall prevail. If you are uncertain or the answer is not explicitly
        written in the documentation, please respond with "I'm sorry, I cannot assist with this.\n\n''' + "Context sections:\n" + \
        content.strip().replace('\n', ' ') + "\n\nQuestion:"""" + query.replace('\n', ' ') + """"\n\nAnswer:"
```
