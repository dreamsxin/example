
```shell
# 查看索引
curl -X GET "localhost:9200/_cat/indices?v"
curl --insecure http://localhost:9200/news/_settings
curl --insecure http://localhost:9200/_ilm/policy
curl -X GET "localhost:9200/_template/template_name"
```

```shell
PUT /your_index_name/_settings
{
  "index.lifecycle.name": null,
  "index.lifecycle.rollover_alias": null
}
```
