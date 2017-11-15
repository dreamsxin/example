# Vuls

https://github.com/future-architect/vuls

## 安装

接下来我们需要创建一些日志目录。 与系统的用户更改 `user_for_scanner`。

```shell
sudo mkdir /var/log/vuls
sudo chown user_for_scanner /var/log/vuls
sudo chmod 700 /var/log/vuls
```

现在我们要安装一个漏洞数据库，编译 `go-cve-dictionary` ：
```shell
mkdir -p $GOPATH/src/github.com/kotakanbe
cd $GOPATH/src/github.com/kotakanbe
git clone https://github.com/kotakanbe/go-cve-dictionary.git
cd go-cve-dictionary
make install
```

下载数据库：
```shell
for i in {2002..2016}; do go-cve-dictionary fetchnvd -years $i; done
```

安装 vuls
```shell
mkdir -p $GOPATH/src/github.com/future-architect
cd $GOPATH/src/github.com/future-architect
git clone https://github.com/future-architect/vuls.git
cd vuls
make install
```

## 利用

现在，一切都安装好了，我们将向您展示如何使用vulns。 首先，我们将在$HOME中创建一个名为 “config.toml” 的配置文件：

```shell
cd $HOME
touch config.toml
```

然后添加以下内容：
```toml
[servers]

[servers.localhost]
host = "localhost"
port = "local"
```

开始执行 :

```shell
vuls scan
```

如果出现问题，尝试执行`vuls configtest`来检查配置文件是否正确。

您可以创建报告：
```shell
vuls report -format-one-line-text -cvedb-path=$PWD/cve.sqlite3
```

## 附录

`Solving Error: cannot find package "github.com/mattn/go-sqlite3" in any of:`

如果由于某种原因您可能会出现以下这个错误：
```text
main.go:9:2: cannot find package "github.com/google/subcommands" in any of:
        /usr/local/go/src/github.com/google/subcommands (from $GOROOT)
        /home/leonardo/go/src/github.com/google/subcommands (from $GOPATH)
main.go:10:2: cannot find package "github.com/kotakanbe/go-cve-dictionary/commands" in any of:
        /usr/local/go/src/github.com/kotakanbe/go-cve-dictionary/commands (from $GOROOT)
        /home/leonardo/go/src/github.com/kotakanbe/go-cve-dictionary/commands (from $GOPATH)
main.go:12:2: cannot find package "github.com/mattn/go-sqlite3" in any of:
        /usr/local/go/src/github.com/mattn/go-sqlite3 (from $GOROOT)
        /home/leonardo/go/src/github.com/mattn/go-sqlite3 (from $GOPATH)
```
试试这个来解决问题
```shell
go get github.com/google/subcommands
go get github.com/kotakanbe/go-cve-dictionary
go get github.com/mattn/go-sqlite3
make install
```
