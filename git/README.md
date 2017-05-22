# GIT 使用

工作区、版本库、暂存区（index）

![Aaron Swartz](https://raw.githubusercontent.com/dreamsxin/example/master/git/img/0.png)

## 设置用户名和邮箱
```shell
git config --global user.name "姓名全拼"
git config --global user.email "gitlab留得邮箱地址"
```

## 显示中文路径以及文件名

```shell
git config --global core.quotepath false
```

## 忽略权限修改

```shell
git config core.filemode false
```

## 统计指定用户一个月以来的代码行数变化
```shell
git log --since=1.month.ago --author="liguodong" --pretty=tformat: --numstat | gawk '{ add += $1 ; subs += $2 ; loc += $1 - $2 } END { printf "added lines: %s removed lines : %s total lines: %s\n",add,subs,loc }' -
```

## 统计提交次数

```shell
git log --since=1.month.ago --author="liguodong" | grep -e 'commit [a-zA-Z0-9]*' | wc -l
```

## 查询当前状态

列出未跟踪、尚未暂存以备提交的变更的文件。

```shell
git status
git status -s
```

命令查看指定目录中被修改或已删除但没有提交的文件。

```shell
git add -i path
```

## 查看差异

查看尚未暂存的文件更新了哪些部分

```shell
git diff
```

查看已经暂存(git add)起来的文件(staged)和上次提交时的快照之间(HEAD)的差异

```shell
git diff --cached
git diff --staged
```

显示工作版本(Working tree)和HEAD的差别

```shell
git diff HEAD
```

直接将两个分支上最新的提交做diff

```shell
git diff topic master
git diff topic..master
```

查看简单的diff结果，可以加上--stat参数
```shell
git diff --stat
```

# 提交到版本库

```shell
git commit -m '提交已经add过的文件'
git commit -am '会先把所有已经改动过的track文件add进来'
git commit --amend -m '增补提交，覆盖上一次提交'
```

## 显示commit日志

```shell
git log
// 显示最近两次记录
git log -2
```

查看file1文件file2文件的提交记录

```shell
git log file1 file2   
```

查看file文件夹下所有文件的提交记录

```shell
git log file/         
```


以简单的图形方式列出提交记录

```shell
git log --graph
```

仅显示指定时间之后的提交(不包含当前日期)

```shell
git log --since={2010-04-18}
git log --after={2010-04-18}
```

仅显示指定时间之前的提交(包含当前日期)

```shell
git log --until={3,weeks,ago}
git log --before={3,weeks,ago}
```

查询commit之前的记录，包含commit

```shell
git log commit
```

查询commit1与commit2之间的记录，包括commit1和commit2

```shell
git log commit1 commit2
```

同上，但是不包括commit1
```shell
git log commit1..commit2
```

查看所有合并过的提交历史记录

```shell
git log --mergs
```

查看所有未被合并过的提交信息

```shell
git log --no-merges
```

查询指定作者的提交记录

```shell
git log --author=朱宗鑫
```

指定日志格式，默认为`medium`：oneline,short,medium,full,fuller,email,raw以及format
常见的format选项：
- %H      提交对象（commit）的完整哈希字串
- %h      提交对象的简短哈希字串
- %T      树对象（tree）的完整哈希字串
- %t      树对象的简短哈希字串
- %P      父对象（parent）的完整哈希字串
- %p      父对象的简短哈希字串
- %an     作者（author）的名字
- %ae     作者的电子邮件地址
- %ad     作者修订日期（可以用 -date= 选项定制格式）
- %ar     作者修订日期，按多久以前的方式显示
- %cn     提交者(committer)的名字
- %ce     提交者的电子邮件地址
- %cd     提交日期
- %cr     提交日期，按多久以前的方式显示
- %s      提交说明
```shell
git log --oneline
git log --pretty=oneline
git log --pretty=format:"%an %ae %ad %cn %ce %cd %cr %s" --graph
```

## 查看某个提交变动

```shell
git show db2de933d46f784ac5cfc9e363afb932825864a8
```

## 查看某个提交变动

```shell
git show db2de933d46f784ac5cfc9e363afb932825864a8
```

## 合并某个提交

```shell
git cherry-pick db2de933d46f784ac5cfc9e363afb932825864a8
```

## 回退

参数 `--hard`、`--soft`、`--mixed` 决定了是否重置暂存区或工作区。


`HEAD`

这是当前分支版本顶端的别名，也就是在当前分支你最近的一个提交

`Index`

index也被称为staging area，是指一整套即将被下一个提交的文件集合。他也是将成为HEAD的父亲的那个commit

`Working Copy`

working copy代表你正在工作的那个文件集

如果没有进行任何`git add`操作，或者进行了`git commit`操作，执行下面命令

```shell
git reset HEAD
```

任何事情都不会发生，这是因为我们告诉GIT重置这个分支到HEAD，而这个正是它现在所在的位置。

`--soft` 只会改变 `HEAD` 也就是说，还保留暂存区信息。

## 删除分支

```shell
# 删除本地
git branch -D branch-name

# 删除远程
git branch -r -d origin/branch-name
git push origin :branch-name
```

## 代理设置

```shell
git config --global http.proxy http://host:port
ssh -qTfnN -D 7070 username@host -p port
export all_proxy=socks5://127.0.0.1:7070
```

## CVS 导入

```shell
sudo apt-get intsall git-cvs
git cvsimport -o origin -v -d :pserver:anonymous@cvs.schmorp.de/schmorpforge libeio
```