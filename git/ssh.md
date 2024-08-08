## ~/.ssh/config

我们可以通过SSH配置文件来指定在 `git clone过` 程中使用特定的私钥。下面配置通过 host 选择不同公钥。
```conf
 Host github-work
     HostName github.com
     IdentityFile ~/.ssh/id_rsa_work
 ​
 Host github-personal
     HostName github.com
     IdentityFile ~/.ssh/id_rsa_personal
```
我们可以使用github-work作为其主机来指定SSH连接字符串。如：
```shell
git clone git@github-work:corporateA/webapp.git
git clone git@github-personal:bob/blog.git
```

## core.sshCommand

此配置将覆盖默认的SSH命令。
```shell
git clone -c "core.sshCommand=ssh -i ~/.ssh/id_rsa_work" git@github.com:corporateA/webapp.git
```
以上例子中，使用 `-c` 选项在运行时覆盖`core.sshCommand`。

将配置覆盖持久化到存储库级别，
```shell
git config core.sshCommand "ssh -i ~/.ssh/id_rsa_work"
```
```shell
git config --get core.sshCommand
···
