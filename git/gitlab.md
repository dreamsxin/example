# 

- https://github.com/gitlabhq/gitlabhq

## 安装

- https://about.gitlab.com/install/
- https://gitlab.com/gitlab-org/gitlab/-/blob/master/doc/install/installation.md

```shell
# run as root!
sudo su
apt-get update -y
apt-get upgrade -y
apt-get install sudo -y

sudo apt-get install -y build-essential zlib1g-dev libyaml-dev libssl-dev libgdbm-dev libre2-dev \
  libreadline-dev libncurses5-dev libffi-dev curl openssh-server libxml2-dev libxslt-dev \
  libcurl4-openssl-dev libicu-dev libkrb5-dev logrotate rsync python3-docutils pkg-config cmake \
  runit-systemd
```

## SSH KEY 生成

```shell
ssh-keygen -t rsa -C "dreamsxin@qq.com"
```

## 查看 public key

```shell
cat ~/.ssh/id_rsa.pub
```

# 错误

```output
Started GET "/test/Test" for 127.0.0.1 at 2016-09-28 17:19:33 +0800
Processing by ProjectsController#show as HTML
  Parameters: {"namespace_id"=>"test", "id"=>"Test"}
Completed 500 Internal Server Error in 50ms (ActiveRecord: 9.1ms)

Rugged::OdbError (Object not found - failed to find pack entry (0232100132054d12018ce4e49675000b50ec1867)):
  app/models/repository.rb:16:in `new'
  app/models/repository.rb:16:in `initialize'
  app/models/project.rb:268:in `new'
  app/models/project.rb:268:in `repository'
  app/controllers/application_controller.rb:136:in `repository'
```

```shell
# 查看 检查仓库的数据完整性
git fsck
cd repositories/test/Test.git
rm –fr ref/head/<branchname>
git fsck
```

如果指定 `--full` 选项，该命令显示所有未被其他对象引用 (指向) 的所有对象：

```shell
git fsck --full
dangling blob d670460b4b4aece5915caf5c68d12f560a9fe3e4
dangling commit ab1afef80fac8e34258ff41fc1b867c702daa24b
dangling tree aea790b9a58f6cf6f2804eeac9f0abbe9631e4c9
dangling blob 7108f7ecb345ee9d0084193f147cdad4d2998293
```

本例中，可以从 dangling commit 找到丢失了的 commit。用相同的方法就可以恢复它，即创建一个指向该 SHA 的分支。

## Nginx 代理配置
```conf
upstream  git{
    # 域名对应 gitlab配置中的 external_url
    # 端口对应 gitlab 配置中的 nginx['listen_port']
    server  域名:端口;
}


server{
    listen 80;
    # 此域名是提供给最终用户的访问地址
    server_name 域名;

    location / {
        # 这个大小的设置非常重要，如果 git 版本库里面有大文件，设置的太小，文件push 会失败，根据情况调整
        client_max_body_size 50m;

        proxy_redirect off;
        #以下确保 gitlab中项目的 url 是域名而不是 http://git，不可缺少
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        # 反向代理到 gitlab 内置的 nginx
        proxy_pass http://git;
        index index.html index.htm;
    }
}

```
