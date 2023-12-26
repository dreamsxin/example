# PostgreSQL身份验证方式

PostgreSQL 支持下面这些身份验证方式：

- trust
- password
- peer
- ident
- GSSAPI
- SSPI
- Kerberos
- LDAP
- RADIUS
- certificate
- PAM

## trust 身份验证

当使用 trust 身份验证方式时， PostgreSQL 会认为凡是能连接到该数据库服务器的，都是可信任的，可以随意访问数据库，而不管他们用什么数据库用户名来访问。这里边包含了如下几个意思：

- trust 方式需要你提供正确的数据库用户名，但是不需要提供密码。用不存在的数据库用户名来登录，那是不行的。
- 数据库用户名可以没有对应的操作系统用户名（通常来说同名）。
- 对于本地连接（Unix domain socket）来说，该方式实际上是把身份验证的职责全权交给了操作系统，只要取得了操作系统的访问权就取得了 PostgreSQL 的访问权。
- 如果在TCP/IP连接上使用 trust 身份验证方式，那就等价于没有身份验证了。

## 基于密码的身份验证

基于密码的身份验证方式包括 md5 和 password。这两种方式非常相似，唯一的区别在于传输密码的时候前者是md5加密的，后者是明文的。同样的：

- 既需要提供正确的数据库用户名，也需要提供密码。需要特别注意的是，采用md5方式的时候，并不需要你提供md5加密的密码，直接输入明文密码即可。psql, pgadmin, jdbc 会自动进行md5加密的。
- 数据库用户名可以没有对应的操作系统用户名（通常来说同名）。
- 对于本地连接（Unix domain socket）来说，该方式既需要你获取操作系统访问权，还需要你提供数据库用户密码来获取 PostgreSQL 的访问权。
- 如果在TCP/IP连接上使用基于密码的身份验证方式，身份验证是基于密码的。

## ident 身份验证

ident 方式通过 Identification 协议，从 ident 服务器获得客户端操作系统用户名，然后把操作系统用户名作为数据库用户名。该方式只能用于TCP/IP连接。当 ident 被用于本地(非TCP/IP)链接时， 实际上采用的是 peer 方式。
RFC 1413 文档描述了 Identification 协议。几乎所有的Unix类操作系统都提供了 ident 服务器（openSUSE可以安装 oidentd或者 pidentd），这些 ident 服务器默认监听 TCP 113 端口。ident 服务器的基本功能就是回答类似“哪个用户建立了从你的X端口出来然后连入我的Y端口的连接？”的问题。由于 PostgreSQL 知道物理链接的 X 端口和 Y端口，它就可以询问链接客户端所在主机上的 ident 服务器，从而可以知道这条链接是哪个操作系统用户建立的。
这个流程的缺点是它依赖于客户端的完整性：如果客户端机器不可信，攻击者可以在113端口运行任意程序并返回任意用户名。因而这种身份验证方式只适合于封闭网络，而且该网络中的所有客户端机器都必须严格控制，数据库管理员和系统管理员必须紧密联系。换句话说，你必须信任运行 ident 服务器的机器。同时你还必须注意，RFC 1413协议明确说明了，Identification 协议的设计意图并不是为了身份验证和访问控制。
一些 ident 服务器提供了非标准选项，会导致返回加密了的用户名，而加密时使用的密钥只有 ident 服务器的管理员知道。这些 ident 服务器要和 PostgreSQL 协同工作，一定不能使用这些选项，因为 PostgreSQL 无法解密返回的字符串，无从知道真实用户名。

同样的：

- 需要提供正确的数据库用户名，不需要提供密码。且该数据库用户名必须和建立数据库链接的操作系统用户名一致，通常来说就是启动客户端应用的操作系统用户名。
- 数据库用户名必须有对应的操作系统用户名（通常来说同名）。
- 不能应用在本地连接上。
- 如果在TCP/IP连接上使用该验证方式，相当于身份验证完全交给了数据库客户端应用所在的机器。和 trust 方式一样，极其不安全。

## peer 身份验证

peer 身份验证方式通过客户端操作系统内核来获取用户名，并作为数据库用户名来进行登录。该方式只能应用在本地连接上。peer 身份验证方式只能在那些提供了getpeereid()函数，SO_PEERCRED socket参数或者类似机制的操作系统中使用。当前包括 Linux,大多数 BSD系统比如 Mac OS X 和 Solaris。

- 需要提供正确的数据库用户名，不需要提供密码。且该数据库用户名必须和启动客户端应用的操作系统用户名一致，比如使用 bruce 用户启动了 pgadmin， 那么在连接数据库时就必须提供 bruce 用这个数据库用户名。
- 数据库用户名必须有对应的操作系统用户名（通常来说同名）。
- 如果在本地连接上使用该验证方式，相当于身份验证完全交给了数据库客户端应用所在的机器。和 trust 方式一样，极其不安全。
- 不能应用在 TCP/IP 链接上。

* peer 验证失败的原因

错误：`Peer authentication failed for user "postgres"`

我们先看看 `/var/lib/pgsql/data/pg_hba.conf` 的默认设置:
```conf
# TYPE  DATABASE        USER            ADDRESS                 METHOD

# "local" is for Unix domain socket connections only
local   all             all                                     peer
# IPv4 local connections:
host    all             all             127.0.0.1/32            ident
# IPv6 local connections:
host    all             all             ::1/128                 ident
```

可以看出，默认情况下本地连接采用 peer 验证方式， TCP/IP连接采用 ident验证方式。
而由于我们执行 psql 命令时并没有指明用 -h 选项来指明主机名，所以 psql采用的是本地连接，这样一来，验证方式就是 peer 验证了。
那么为什么又报验证失败的错误呢？ 这是由于提供了数据库用户名和启动psql命令的操作系统用户名(bruce)不一致导致的。
切换到 postgres 用户之后再执行 `psql -U postgres - W` 就可以了。 
或
```shell
sudo -u postgres psql
```

## reject
无条件地拒绝连接。这有助于从一个组中"过滤出"特定主机，例如一个reject行可以阻塞一个特定的主机连接，而后面一行允许一个特定网络中的其余主机进行连接。

## gss
用 GSSAPI 认证用户。只对TCP/IP 连接可用。

## sspi
用 SSPI 来认证用户。只在Windows 上可用。

## ldap
使用LDAP服务器认证。

## radius
用 RADIUS 服务器认证。

## cert
使用 SSL 客户端证书认证。

## pam
使用操作系统提供的可插入认证模块服务（PAM）认证。

## bsd
使用由操作系统提供的 BSD 认证服务进行认证。

## 用户管理

```psql
-- 查看当前连接的用户名： 
select * from current_user;
select user;
-- 查看所有用户名： 
\du
create user dev with password '123456';

CREATE USER/ROLE name [ [ WITH ] option [ ... ] ]  : 关键词 USER,ROLE； name 用户或角色名； 

-- 创建使用 trust 方式登陆的用户：
CREATE ROLE loginuser LOGIN;
-- 修改 pg_hba.conf
-- 本地登陆：local   all    all    trust
-- 远程登陆：host   all    all    192.168.163.132/32     trust

-- 创建需要密码登陆的用户：
CREATE USER loginuser WITH PASSWORD '123456';
-- 和 CREATE ROLE的区别是：USER 自带 LOGIN属性。也需要修改 pg_hba.conf 文件（后面会对该文件进行说明），加入：
-- host    all     all     192.168.163.132/32    md5

-- 创建有时间限制的用户
CREATE ROLE loginuser WITH LOGIN PASSWORD '123456' VALID UNTIL '2019-05-30';

-- 创建有创建数据库和管理角色权限的用户admin：
CREATE ROLE admin WITH CREATEDB CREATEROLE;

-- 创建具有超级权限的用户：admin
CREATE ROLE admin WITH SUPERUSER LOGIN PASSWORD 'admin';

-- 创建复制账号：repl 
postgres=# CREATE USER repl REPLICATION LOGIN ENCRYPTED PASSWORD 'repl';

where option can be:

      SUPERUSER | NOSUPERUSER      :超级权限，拥有所有权限，默认nosuperuser。
    | CREATEDB | NOCREATEDB        :建库权限，默认nocreatedb。
    | CREATEROLE | NOCREATEROLE    :建角色权限，拥有创建、修改、删除角色，默认nocreaterole。
    | INHERIT | NOINHERIT          :继承权限，可以把除superuser权限继承给其他用户/角色，默认inherit。
    | LOGIN | NOLOGIN              :登录权限，作为连接的用户，默认nologin，除非是create user（默认登录）。
    | REPLICATION | NOREPLICATION  :复制权限，用于物理或则逻辑复制（复制和删除slots），默认是noreplication。
    | BYPASSRLS | NOBYPASSRLS      :安全策略RLS权限，默认nobypassrls。
    | CONNECTION LIMIT connlimit   :限制用户并发数，默认-1，不限制。正常连接会受限制，后台连接和prepared事务不受限制。
    | [ ENCRYPTED ] PASSWORD 'password' | PASSWORD NULL :设置密码，密码仅用于有login属性的用户，不使用密码身份验证，则可以省略此选项。可以选择将空密码显式写为PASSWORD NULL。
                                                         加密方法由配置参数password_encryption确定，密码始终以加密方式存储在系统目录中。
    | VALID UNTIL 'timestamp'      :密码有效期时间，不设置则用不失效。
    | IN ROLE role_name [, ...]    :新角色将立即添加为新成员。
    | IN GROUP role_name [, ...]   :同上
    | ROLE role_name [, ...]       :ROLE子句列出一个或多个现有角色，这些角色自动添加为新角色的成员。 （这实际上使新角色成为“组”）。
    | ADMIN role_name [, ...]      :与ROLE类似，但命名角色将添加到新角色WITH ADMIN OPTION，使他们有权将此角色的成员资格授予其他人。
    | USER role_name [, ...]       :同上
    | SYSID uid                    :被忽略，但是为向后兼容性而存在。


-- 示例
-- 创建复制用户
CREATE USER abc REPLICATION LOGIN ENCRYPTED PASSWORD '';
CREATE USER abc REPLICATION LOGIN ENCRYPTED PASSWORD 'abc';
ALTER USER work WITH ENCRYPTED password '';

-- 创建scheme 角色
CREATE ROLE abc;
CREATE DATABASE abc WITH OWNER abc ENCODING UTF8 TEMPLATE template0;
\c abc

-- 创建schema
CREATE SCHEMA abc;
ALTER SCHEMA abc OWNER to abc;
revoke create on schema public from public;

-- 创建用户
create user abc with ENCRYPTED password '';
GRANT abc to abc;
ALTER ROLE abc WITH abc;

-- 创建读写账号
CREATE ROLE abc_rw;
CREATE ROLE abc_rr;

-- 赋予访问数据库权限，schema权限
grant connect ON DATABASE abc to abc_rw;
GRANT USAGE ON SCHEMA abc TO abc_rw;

-- 赋予读写权限
grant select,insert,update,delete ON  ALL TABLES IN SCHEMA abc to abc;

-- 赋予序列权限
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA abc to abc;

-- 赋予默认权限
ALTER DEFAULT PRIVILEGES IN SCHEMA abc GRANT select,insert,update,delete ON TABLES TO abc;

-- 赋予序列权限
ALTER DEFAULT PRIVILEGES IN SCHEMA abc GRANT ALL PRIVILEGES ON SEQUENCES TO abc;


-- 用户对db要有连接权限
grant connect ON DATABASE abc to abc;

-- 用户要对schema usage 权限，不然要select * from schema_name.table ,不能用搜索路径
GRANT USAGE ON SCHEMA abc TO abc;
grant select ON ALL TABLES IN SCHEMA abc to abc;
ALTER DEFAULT PRIVILEGES IN SCHEMA abc GRANT select ON TABLES TO abc;

create user abc_w with ENCRYPTED password '';
create user abc_r with ENCRYPTED password '';

GRANT abc_rw to abc_w；

GRANT abc_rr to abc_r;

```
