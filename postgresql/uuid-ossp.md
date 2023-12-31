# uuid-ossp

uuid-ossp 模块提供函数使用几种标准算法之一产生通用唯一标识符（UUID）。还提供产生某些特殊 UUID 常量的函数。

## 安装

```psql
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
```

```sql
CREATE TABLE PINK_FLOYD (
	id uuid DEFAULT uuid_generate_v4(),
	album_name VARCHAR NOT NULL,
	release_date DATE NOT NULL,
	PRIMARY KEY (id)
);
```

```sql
CREATE TABLE PINK_FLOYD (
	id SERIAL PRIMARY KEY,
	album_name TEXT NOT NULL,
	release_date DATE NOT NULL…
	PRIMARY KEY (id)
);
```

## 相关函数

相关标准 ITU-T Rec. X.667、ISO/IEC 9834-8:2005 以及 RFC 4122 指定了四种用于产生 UUID 的算法，分别用版本号 1、3、4、5 标识（没有版本 2 的算法）。这些算法中的每一个都适合于不同的应用集合。

* UUID 常量的函数

- uuid_nil()	

一个"nil" UUID 常量，它不作为一个真正的 UUID 发生。

- uuid_ns_dns()	

为 UUID 指定 DNS 名字空间的常量。

- uuid_ns_url()	

为 UUID 指定 URL 名字空间的常量。

- uuid_ns_oid()	

为 UUID 指定 ISO 对象标识符（OID） 名字空间的常量（这属于 ASN.1 OID，它与PostgreSQL使用的 OID 无关）。

- uuid_ns_x500()	

为 UUID 指定 X.500 可识别名（DN）名字空间的常量。Constant designating the X.500 distinguished name (DN) namespace for UUIDs. 

* 用于 UUID 产生的函数

- uuid_generate_v1()	

这个函数产生一个版本 1 的 UUID。这涉及到计算机的 MAC 地址和一个时间戳。注意这种 UUID 会泄露产生该标识符的计算机标识以及产生的时间，因此它不适合某些对安全性很敏感的应用。

- uuid_generate_v1mc()	

这个函数产生一个版本 1 的 UUID，但是使用一个随机广播 MAC 地址而不是该计算机真实的 MAC 地址。

- uuid_generate_v3(namespace uuid, name text)	

这个函数使用指定的输入名称在给定的名字空间中产生一个版本 3 的 UUID。该名字空间应该是由uuid_ns_*()函数（如表 F-34所示）产生的特殊常量之一（理论上它可以是任意 UUID）。名称是选择的名字空间中的一个标识符。

例如：
```sql
SELECT uuid_generate_v3(uuid_ns_url(), 'http://www.postgresql.org');
```

名称参数将使用 MD5 进行哈希，因此从产生的 UUID 中得不到明文。采用这种方法的 UUID 生成没有随机性并且不涉及依赖于环境的元素，因此是可以重现的。

- uuid_generate_v4()	

这个函数产生一个版本 4 的 UUID，它完全从随机数产生。

- uuid_generate_v5(namespace uuid, name text)	

这个函数产生一个版本 5 的 UUID，它和版本 3 的 UUID 相似，但是采用的是 SHA-1 作为哈希方法。版本 5 比版本 3 更好，因为 SHA-1 被认为比 MD5 更安全。 
