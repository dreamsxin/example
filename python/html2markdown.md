`choco install pandoc`

```python
import pymysql
import pypandoc
import re
from pymysql.cursors import DictCursor

# 数据库配置
db_config = {
    "host": "localhost",
    "user": "test",
    "password": "test",
    "database": "test",
    "charset": "utf8mb4"
}

# 文件名清理函数
def clean_filename(text):
    return re.sub(r'[\\/:*?"<>|]', '_', text)

try:
    # 连接数据库
    connection = pymysql.connect(**db_config)

    with connection.cursor(DictCursor) as cursor:
        # 分页查询
        page_size = 100
        page = 0

        while True:
            sql = f"SELECT * FROM `xxxx` LIMIT {page_size} OFFSET {page * page_size}"
            cursor.execute(sql)
            results = cursor.fetchall()

            if not results:
                break

            for row in results:
                article_id = row["id"]
                title = row["title"]
                html_content = row["content"]

                # 转换为 Markdown
                markdown_content = pypandoc.convert_text(html_content, 'md', format='html')

                # 生成安全文件名
                safe_title = clean_filename(title)
                filename = f"article_{article_id}_{safe_title}.md"

                # 保存文件
                with open(filename, 'w', encoding='utf-8') as md_file:
                    md_file.write(markdown_content)

                print(f"已保存：{filename}")

            page += 1

    connection.commit()

except pymysql.MySQLError as e:
    print(f"数据库错误: {e}")
except Exception as e:
    print(f"错误: {e}")
finally:
    if connection:
        connection.close()
```
