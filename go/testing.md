## testing.T

判定失败接口
- Fail 失败继续
- FailNow 失败终止
打印信息接口
- Log 数据流 （cout　类似）
- Logf format (printf 类似）
- SkipNow 跳过当前测试
- Skiped 检测是否跳过
- 
综合接口产生：

- Error / Errorf 报告出错继续 [ Log / Logf + Fail ]
- Fatel / Fatelf 报告出错终止 [ Log / Logf + FailNow ]
- Skip / Skipf 报告并跳过 [ Log / Logf + SkipNow ]

## testing.B

首先 ， testing.B 拥有 testing.T 的全部接口。
- SetBytes( i uint64) 统计内存消耗， 如果你需要的话。
- SetParallelism(p int) 制定并行数目。
- StartTimer / StopTimer / ResertTimer 操作计时器
- testing.PB
- Next() 接口 。 判断是否继续循环

## testing.M

可以在测试函数执行之前做一些其他操作
