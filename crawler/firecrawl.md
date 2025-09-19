
## 安装

- https://github.com/firecrawl/firecrawl/blob/main/SELF_HOST.md

```shell
git clone https://github.com/firecrawl/firecrawl.git
cd firecrawl
docker-compose build  # 首次构建（可选）
docker-compose up -d  # 启动服务
```

## 验证与使用
- 访问http://localhost:3002/test验证服务是否正常运行
- API调用示例：通过localhost:3002/v1/crawl接口抓取网页内容
- 管理界面：http://localhost:3002/admin/@/queues查看任务队列状态
