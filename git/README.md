# GIT 使用

## 设置用户名和邮箱
```shell
git config --global user.name "姓名全拼"
git config --global user.email "gitlab留得邮箱地址"
```

## 统计指定用户一个月以来的代码行数变化
```shell
git log --since=1.month.ago --author="liguodong" --pretty=tformat: --numstat | gawk '{ add += $1 ; subs += $2 ; loc += $1 - $2 } END { printf "added lines: %s removed lines : %s total lines: %s\n",add,subs,loc }' -
```

## 统计提交次数

```shell
git log --since=1.month.ago --author="liguodong" | grep -e 'commit [a-zA-Z0-9]*' | wc -l
```