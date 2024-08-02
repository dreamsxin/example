# Superset

Apache Superset 支持用户的各种数据类型可视化和数据分析，支持简单图饼图到复杂的地理空间图表。Apache Superset 是一个轻量级、简单化、直观化、可配置的 BI 框架。

主流可视化工具：Superset、DataEase、MetaBase

## 部署

https://superset.apache.org/docs/quickstart

```shell
git clone https://github.com/apache/superset
# Enter the repository you just cloned
$ cd superset

# Fire up Superset using Docker Compose
$ docker compose -f docker-compose-image-tag.yml up
```

### Log into Superset
Now head over to http://localhost:8088 and log in with the default created account:

- username: admin
- password: admin
