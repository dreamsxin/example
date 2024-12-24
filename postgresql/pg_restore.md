## pg\_restore

pg\_restore — 从一个由pg\_dump创建的归档文件恢复一个PostgreSQL数据库

## 大纲

`pg_restore` \[*`connection-option`*...\] \[*`option`*...\] \[*`filename`*\]

## 描述

pg\_restore 是一个用来从 pg\_dump 创建的非文本格式归档恢复PostgreSQL数据库的工具。它将发出必要的命令把该数据库重建成它被保存时的状态。这些归档文件还允许pg\_restore选择恢复哪些内容或者在恢复前对恢复项重排序。这些归档文件被设计为可以在不同的架构之间迁移。

pg\_restore可以在两种模式下操作。如果指定了一个数据库名称，pg\_restore会连接那个数据库并且把归档内容直接恢复到该数据库中。否则，会创建一个脚本，其中包含着重建该数据库所必要的 SQL 命令，它会被写入到一个文件或者标准输出。这个脚本输出等效于pg\_dump的纯文本输出格式。因此，一些控制输出的选项与pg\_dump的选项类似。

显然，pg\_restore无法恢复不在归档文件中的信息。例如，如果归档使用“以`INSERT`命令转储数据”选项创建， pg\_restore将无法使用`COPY`语句装载数据。

## 选项

pg\_restore接受下列命令行参数。

*`filename`*

指定要被恢复的归档文件（对于一个目录格式的归档则是目录）的位置。如果没有指定，则使用标准输入。

`-a`  
`--data-only`

只恢复数据，不恢复模式（数据定义）。如果在归档中存在，表数据、大对象和序列值会被恢复。

这个选项类似于指定`--section=data`，但是由于历史原因两者不完全相同。

`-c`  
`--clean`

在重新创建数据库对象之前清除（丢弃）它们（除非使用了`--if-exists`，如果有对象在目标数据库中不存在，这可能会生成一些无害的错误消息）。

`-C`  
`--create`

在恢复一个数据库之前先创建它。如果还指定了`--clean`，在连接到目标数据库之前丢弃并且重建它。

在使用这个选项时，`-d`提到的数据库只被用于发出初始的`DROP DATABASE`和`CREATE DATABASE`命令。所有要恢复到该数据库名中的数据都出现在归档中。

``-d *`dbname`*``  
``--dbname=*`dbname`*``

连接到数据库*`dbname`*并且直接恢复到该数据库中。

`-e`  
`--exit-on-error`

在发送 SQL 命令到该数据库期间如果碰到一个错误就退出。默认行为是继续并且在恢复结束时显示一个错误计数。

``-f *`filename`*``  
``--file=*`filename`*``

为生成的脚本或列表（当使用`-l`时）指定输出文件。默认是标准输出。

``-F *`format`*``  
``--format=*`format`*``

指定归档的格式。并不一定要指定该格式，因为pg\_restore将会自动决定格式。如果指定，可以是下列之一：

`c`  
`custom`

归档是pg\_dump的自定义格式。

`d`  
`directory`

归档是一个目录归档。

`t`  
`tar`

归档是一个`tar`归档。

``-I *`index`*``  
``--index=*`index`*``

只恢复提及的索引的定义。可以通过写多个`-I`开关指定多个索引。

``-j *`number-of-jobs`*``  
``--jobs=*`number-of-jobs`*``

使用并发任务运行pg\_restore中最耗时的部分 — 载入数据、创建索引或者创建约束。对于一个运行在多处理器机器上的服务器，这个选项能够大幅度减少恢复一个大型数据库的时间。

每一个任务是一个进程或者一个线程，这取决于操作系统，它们都使用一个独立的服务器连接。

这个选项的最佳值取决于服务器、客户端以及网络的硬件设置。因素包括 CPU 的核心数和磁盘设置。一个好的建议是服务器上 CPU 的核心数，但是更大的值在很多情况下也能导致更快的恢复时间。当然，过高的值会由于超负荷反而导致性能降低。

这个选项只支持自定义和目录归档格式。输入必须是一个常规文件或目录（例如，不能是一个管道）。当发出一个脚本而不是直接连接到数据库服务器时会忽略这个选项。还有，多任务不能和选项`--single-transaction`一起用。

`-l`  
`--list`

列出归档的内容的表。这个操作的输出能被用作`-L`选项的输入。注意如果把`-n`或`-t`这样的过滤开关与`-l`一起使用，它们将会限制列出的项。

``-L *`list-file`*``  
``--use-list=*`list-file`*``

只恢复在*`list-file`*中列出的归档元素，并且按照它们出现在该文件中的顺序进行恢复。注意如果把`-n`或`-t`这样的过滤开关与`-L`一起使用，它们将会进一步限制要恢复的项。

*`list-file`*通常是编辑一个`-l`操作的输出来创建。行可以被移动或者移除，并且也可以通过在行首放一个（`;`）将其注释掉。例子见下文。

``-n *`schema`*``  
``--schema=*`schema`*``

只恢复在被提及的模式中的对象。可以用多个`-n`开关来指定多个模式。这可以与`-t`选项组合在一起只恢复一个指定的表。

``-N *`schema`*``  
``--exclude-schema=*`schema`*``

不要恢复命名模式中的对象。可以使用多个`-N`开关指定要排除的多个模式。

当为同一个模式名同时给出`-n`和`-N`时， `-N`开关胜利并且排除它指出的模式。

`-O`  
`--no-owner`

不要输出将对象的所有权设置为与原始数据库匹配的命令。默认情况下，pg\_restore会发出`ALTER OWNER`或者`SET SESSION AUTHORIZATION`语句来设置已创建的模式对象的所有权。 如果最初的数据库连接不是由超级用户 (或者是拥有脚本中所有对象的同一个用户)发起的，那么这些语句将失败。 通过`-O`，任何用户名都可以被用于初始连接，并且这个用户将会拥有所有被创建的对象。

``-P *`function-name(argtype [, ...])`*``  
``--function=*`function-name(argtype [, ...])`*``

只恢复被提及的函数。要小心地拼写函数的名称和参数使它们正好就是出现在转储文件的内容表中的名称和参数。可以使用多个`-P`开关指定多个函数。

`-R`  
`--no-reconnect`

这个选项已被废弃，但是出于向后兼容性的目的，系统仍然还接受它。

`-s`  
`--schema-only`

只恢复归档中的模式（数据定义）不恢复数据。

这个选项是`--data-only`的逆选项。它与指定`--section=pre-data --section=post-data`相似，但是由于历史原因并不完全相同。

（不要把这个选项和`--schema`选项弄混，后者把词“schema”用于一种不同的含义）。

``-S *`username`*``  
``--superuser=*`username`*``

指定在禁用触发器时要用的超级用户名。只有使用`--disable-triggers`时这个选项才相关。

``-t *`table`*``  
``--table=*`table`*``

只恢复所提及的表的定义和数据。出于这个目的，“table”包括视图、物化视图、序列和外部表。可以写上多个`-t`开关可以选择多个表。这个选项可以和`-n`选项结合在一起指定一个特定模式中的表。

### 注意

在指定`-t`时，pg\_restore不会尝试恢复所选表可能依赖的任何其他数据库对象。因此，无法确保能成功地把一个特定表恢复到一个干净的数据库中。

### 注意

这个标志的行为和pg\_dump的`-t`标志不一样。在pg\_restore中当前没有任何通配符匹配的规定，也不能在其`-t`选项中包括模式的名称。

### 注意

在PostgreSQL 9.6之前的版本中，这个标志只匹配表，而并不匹配其他类型的关系。

``-T *`trigger`*``  
``--trigger=*`trigger`*``

只恢复所提及的触发器。可以用多个`-T`开关指定多个触发器。

`-v`  
`--verbose`

指定冗长模式。

`-V`  
`--version`

打印该pg\_restore的版本并退出。

`-x`  
`--no-privileges`  
`--no-acl`

阻止恢复访问特权（授予/收回命令）。

`-1`  
`--single-transaction`

将恢复作为单一事务执行（即把发出的命令包裹在`BEGIN`/`COMMIT`中）。这可以确保要么所有命令完全成功，要么任何改变都不被应用。这个选项隐含了`--exit-on-error`。

`--disable-triggers`

只有在执行一个只恢复数据的恢复时，这个选项才相关。它指示pg\_restore在装载数据时执行命令临时禁用目标表上的触发器。如果你在表上有参照完整性检查或者其他触发器并且你不希望在数据载入期间调用它们时，请使用这个选项。

目前，为`--disable-triggers`发出的命令必须以超级用户身份完成。因此你还应该用`-S`指定一个超级用户名，或者更好的方法是以一个PostgreSQL超级用户运行pg\_restore。

`--enable-row-security`

只有在恢复具有行安全性的表的内容时，这个选项才相关。默认情况下，pg\_restore将把[row\_security](https://www.runoob.com/manual/PostgreSQL/runtime-config-client.html#GUC-ROW-SECURITY)设置为 off 来确保所有数据都被恢复到表中。如果用户不拥有足够绕过行安全性的特权，那么会抛出一个错误。这个参数指示pg\_restore把[row\_security](https://www.runoob.com/manual/PostgreSQL/runtime-config-client.html#GUC-ROW-SECURITY)设置为 on允许用户尝试恢复启用了行安全性的表的内容。如果用户没有从转储向表中插入行的权限，这仍将失败。

注意当前这个选项还要求转储处于`INSERT`格式，因为`COPY FROM`不支持行安全性。

`--if-exists`

在清理数据库对象时使用条件命令（即增加一个`IF EXISTS`子句）。只有指定了`--clean`时，这个选项才有效。

`--no-data-for-failed-tables`

默认情况下，即便表的创建命令失败（例如因为表已经存在），表数据也会被恢复。通过这个选项，对这类表的数据会被跳过。如果目标数据库已经包含了想要的表内容，这种行为又很有有用。例如，PostgreSQL扩展（如PostGIS）的辅助表可能已经被载入到目标数据库中，指定这个选项就能阻止把重复的或者废弃的数据载入到这些表中。

只有当直接恢复到一个数据库中时这个选项才有效，在产生 SQL脚本输出时这个选项不会产生效果。

`--no-publications`

不要输出命令来恢复发布，即使存档包含它们。

`--no-security-labels`

不要输出恢复安全标签的命令，即使归档中包含安全标签。

`--no-subscriptions`

不要输出命令来恢复订阅，即使存档包含它们。

`--no-tablespaces`

不输出命令选择表空间。通过这个选项，所有的对象都会被创建在恢复时的默认表空间中。

``--section=*`sectionname`*``

只恢复提及的小节。小节的名称可以是`pre-data`、`data`或者`post-data`。可以把这个选项指定多次来选择多个小节。默认值是恢复所有小节。

数据小节包含实际的表数据以及大对象定义。Post-data 项由索引定义、触发器、规则和除已验证的检查约束之外的约束构成。Pre-data 项由所有其他数据定义项构成。

`--strict-names`

要求每一个模式（`-n`/`--schema`）以及表（`-t`/`--table`）限定词匹配备份文件中至少一个模式/表。

`--use-set-session-authorization`

输出 SQL 标准的`SET SESSION AUTHORIZATION`命令取代`ALTER OWNER`命令来决定对象拥有权。这会让转储更加兼容标准，但是依赖于转储中对象的历史，可能无法正确恢复。

`-?`  
`--help`

显示有关pg\_restore命令行参数的帮助，并且退出。

pg\_restore也接受下列用于连接参数的命令行参数：

``-h *`host`*``  
``--host=*`host`*``

指定服务器正在运行的机器的主机名。如果该值开始于一个斜线，它被用作一个 Unix 域套接字的目录。默认是从`PGHOST`环境变量中取得（如果被设置），否则将尝试一次 Unix 域套接字连接。

``-p *`port`*``  
``--port=*`port`*``

指定服务器正在监听连接的 TCP 端口或本地 Unix 域套接字文件扩展名。默认是放在`PGPORT`环境变量中（如果被设置），否则使用编译在程序中的默认值。

``-U *`username`*``  
``--username=*`username`*``

要作为哪个用户连接。

`-w`  
`--no-password`

不发出一个口令提示。如果服务器要求口令认证并且没有其他方式提供口令（例如一个`.pgpass`文件），那么连接尝试将失败。这个选项对于批处理任务和脚本有用，因为在其中没有一个用户来输入口令。

`-W`  
`--password`

强制pg\_restore在连接到一个数据库之前提示要求一个口令。

这个选项不是必须的，因为如果服务器要求口令认证，pg\_restore将自动提示要求一个口令。但是，pg\_restore将浪费一次连接尝试来发现服务器想要一个口令。在某些情况下，值得键入`-W`来避免额外的连接尝试。

``--role=*`rolename`*``

指定一个用来创建该转储的角色名。这个选项导致pg\_restore在连接到数据库后发出一个`SET ROLE` *`rolename`*命令。当已认证用户（由`-U`指定）缺少pg\_restore所需的特权但是能够切换到一个具有所需权利的角色时，这个选项很有用。一些安装有针对直接作为超级用户登录的策略，使用这个选项可以让转储在不违反该策略的前提下完成。

## 环境

`PGHOST`  
`PGOPTIONS`  
`PGPORT`  
`PGUSER`

默认连接参数

和大部分其他PostgreSQL工具相似，这个工具也使用 libpq 支持的环境变量。

## 诊断

当使用`-d`选项指定一个直接数据库连接时，pg\_restore在内部执行`SELECT`语句。如果你运行pg\_restore时出现问题，确定你能够从正在使用的数据库中选择信息，例如 psql。此外，libpq前端-后端库所使用的任何默认连接设置和环境变量都将适用。

## 注解

如果你的数据库集簇对于`template1`数据库有任何本地添加，要注意将pg\_restore的输出载入到一个真正的空数据库。否则你很可能由于以增加对象的重复定义而得到错误。要创建一个不带任何本地添加的空数据库，从`template0`而不是`template1`复制它，例如：

```
CREATE DATABASE foo WITH TEMPLATE template0;
```

下面将详细介绍pg\_restore的局限性。

+   在恢复数据到一个已经存在的表中并且使用了选项`--disable-triggers`时，pg\_restore会在插入数据之前发出命令禁用用户表上的触发器，然后在完成数据插入后重新启用它们。如果恢复在中途停止，可能会让系统目录处于错误的状态。
    
+   pg\_restore不能有选择地恢复大对象，例如只恢复特定表的大对象。如果一个归档包含大对象，那么所有的大对象都会被恢复，如果通过`-L`、`-t`或者其他选项进行了排除，它们一个也不会被恢复。
    

pg\_dump 的局限性详见 pg\_dump 文档。

一旦完成恢复，应该在每一个被恢复的表上运行`ANALYZE`，这样优化器能得到有用的统计信息。

## 示例

假设我们已经以自定义格式转储了一个叫做`mydb`的数据库：

```
$ pg_dump -Fc mydb > db.dump
```

要删除该数据库并且从转储中重新创建它：

```
$ dropdb mydb
$ pg_restore -C -d postgres db.dump
```

`-d`开关中提到的数据库可以是任何已经存在于集簇中的数据库，pg\_restore只会用它来为`mydb`发出`CREATE DATABASE`命令。通过`-C`，数据总是会被恢复到出现在归档文件的数据库名中。

要把转储重新载入到一个名为`newdb`的新数据库中：

```
$ createdb -T template0 newdb
$ pg_restore -d newdb db.dump
```

注意我们不使用`-C`，而是直接连接到要恢复到其中的数据库。还要注意我们是从`template0`而不是`template1`创建了该数据库，以保证它最初是空的。

要对数据库项重排序，首先需要转储归档的表内容：

```
$ pg_restore -l db.dump > db.list
```

列表文件由一个头部和一些行组成，这些行每一个都用于一个项，例如：

```
;
; Archive created at Mon Sep 14 13:55:39 2009
;     dbname: DBDEMOS
;     TOC Entries: 81
;     Compression: 9
;     Dump Version: 1.10-0
;     Format: CUSTOM
;     Integer: 4 bytes
;     Offset: 8 bytes
;     Dumped from database version: 8.3.5
;     Dumped by pg_dump version: 8.3.8
;
;
; Selected TOC Entries:
;
3; 2615 2200 SCHEMA - public pasha
1861; 0 0 COMMENT - SCHEMA public pasha
1862; 0 0 ACL - public pasha
317; 1247 17715 TYPE public composite pasha
319; 1247 25899 DOMAIN public domain0 pasha
```

分号表示开始一段注释，行首的数字表明了分配给每个项的内部归档 ID。

文件中的行可以被注释掉、删除以及重排序。例如：

```
10; 145433 TABLE map_resolutions postgres
;2; 145344 TABLE species postgres
;4; 145359 TABLE nt_header postgres
6; 145402 TABLE species_records postgres
;8; 145416 TABLE ss_old postgres
```

把这样一个文件作为pg\_restore的输入将会只恢复项 10 和 6，并且先恢复 10 再恢复 6。

```
$ pg_restore -L db.list db.dump
```
