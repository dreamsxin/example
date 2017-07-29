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