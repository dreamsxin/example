## 测试用例四种形式

- TestXxxx(t testing.T)
基本测试用例
- BenchmarkXxxx(b testing.B)
压力测试的测试用例
- Example_Xxx()
测试控制台输出的例子
- TestMain(m *testing.M)
测试Main函数

## 变量

- test.short
一个快速测试的标记，在测试用例中可以使用`testing.Short()`来绕开一些测试
- test.outputdir
输出目录
- test.coverprofile
测试覆盖率参数，指定输出文件
- test.run
指定正则来运行某个/某些测试用例
- test.memprofile
内存分析参数，指定输出文件
- test.memprofilerate
内存分析参数，内存分析的抽样率
- test.cpuprofile
cpu分析输出参数，为空则不做cpu分析
- test.blockprofile
阻塞事件的分析参数，指定输出文件
- test.blockprofilerate
阻塞事件的分析参数，指定抽样频率
- test.timeout
超时时间
- test.cpu
指定cpu数量
- test.parallel
指定运行测试用例的并行数

- B
压力测试
- BenchmarkResult
压力测试结果
- Cover
代码覆盖率相关结构体
- CoverBlock
代码覆盖率相关结构体
- InternalBenchmark
内部使用的结构
- InternalExample
内部使用的结构
- InternalTest
内部使用的结构
- M
main测试使用的结构
- PB
Parallel benchmarks 并行测试使用结果
- T
普通测试用例
- TB
测试用例的接口

## testing.T

* 判定失败接口
- Fail
失败继续
- FailNow
失败终止

* 打印信息接口
- Log
数据流 （cout　类似）
- Logf
format (printf 类似）
- SkipNow
跳过当前测试
- Skiped
检测是否跳过
- Short
是否设置了 -test.short 标志。

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

```go
package pkg

import (
	"testing"
)

func BenchmarkUngzip(b *testing.B) {

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		Ungzip("../coffee.html.gz")
	}
}

func BenchmarkUnpgzip(b *testing.B) {

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		Unpgzip("../coffee.html.gz")
	}
}
```

```shell
go test -benchmem -run=^$ -bench ^BenchmarkUngzip$ test2/test14/pgzip/pkg -count=1
go test -benchmem -run=^$ -bench ^(BenchmarkUngzip|BenchmarkUnpgzip)$ test2/test14/pgzip/pkg -count=1
```

## testing.M

可以在测试函数执行之前做一些其他操作

## 禁用缓存

要禁用测试缓存，可以通过 -count=1 标志来实现。
`go test -v -count=1 -run ^TestStartSeo$`
