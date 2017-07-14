创建表、修改表、创建策略：使用RLS
```sql
 CREATE TABLE ratings2 (  
  user_role_name NAME,
  rating_type_name TEXT,
  artist_name TEXT,
  rating INTEGER
);

ALTER TABLE ratings2 ENABLE ROW LEVEL SECURITY;

CREATE POLICY ratings2_user ON ratings2  
  USING(user_role_name = current_user);
```

上述封装了RLS最简单的例子。策略为每行返回一个布尔值。

这还可以通过声明每一种类型的声明以及它如何被授权的方式来定义。如果需要，选择和插入可以创建一个可读的附加表限于个人用户：
```sql
CREATE POLICY ratings2_user_select ON ratings2  
  FOR SELECT
  USING(user_role_name = current_user);

CREATE POLICY ratings2_user_insert ON ratings2  
  FOR INSERT 
  WITH CHECK(user_role_name = current_user);
```

或者，采用以下方式：

```sql
CREATE POLICY ratings2_user ON ratings2  
  USING(pg_has_role(current_user, user_role_name, 'member'));
```

用户组权限的改变，使用范围也随之改变。

```sql
CREATE ROLE group1;  
GRANT ALL ON ratings2 TO group1;

CREATE ROLE music1 LOGIN PASSWORD 'change';  
GRANT group1 to music1;

CREATE ROLE music2 LOGIN PASSWORD 'change';  
GRANT group1 TO music2;
```