# sybil

https://github.com/logv/sybil

## 数据格式
Sybil 不需要预先定义表结构，数据使用 JSON 格式表示。示例：
```json
{
  // time is in seconds since the epoch and is the only required field
  time: 1461765374, 
  // sybil supports ints (up to int64)
  age: 28,
  // sybil supports string columns
  country: "USA",
  state: "NY",
  favorite_food: "ice cream",
  gym_membership: "no"
  // and sybil supports sets
  favorite_bands: [ "the doors", "talking heads" ]
}
```
## 导入数据
```shell
sybil ingest -table my_first_table < json_samples.json
```

## 查询数据

```shell
sybil query -table my_table -group col1,col2,col3 -int col4,col5,col6
sybil query -tables
sybil query -table my_table -info
sybil query -table my_table -samples -limit 5
```

### 过滤数据 Filters
格式: `-*-filter col:op:val,col:op:val`

- -str-filter - supports string regexes using re and nre
- -int-filter - supports eq, neq, gt and lt
- -set-filter - supports in and nin
```shell
 -int-filter time_col:gt:`date --date="-1 hour" +%s`
```
### 时序数据

- -time, -time-col <FIELD>
- -time-bucket <SECONDS>

 
