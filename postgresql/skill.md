# 常用技巧

## 插入忽略

### 使用rule规则

```sql
CREATE RULE rule_insert_ignore AS ON insert TO table_name WHERE exists (SELECT 1 FROM table_name WHERE id = new.id) DO INSTEAD NOTHING;
```

### 使用触发器

```sql
CREATE OR REPLACE FUNCTION sp_insert_ignore() RETURNS trigger AS $$
BEGIN
	PERFORM 1 FROM table_name WHERE id = new.id;
	IF FOUND THEN
		RETURN null;
	END IF;
	RETURN new;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trigger_ignore BEFORE INSERT ON table_name FOR EACH ROW EXECUTE PROCEDURE sp_insert_ignore(); 
```

### 使用 WITH

```sql
INSERT INTO table_name 
WITH t(f1,f2) as (values(6,current_time),(3,current_time))   
SELECT a.* FROM t as a where a.f1 not in (SELECT id FROM table_name );                            
```

### 使用 ON CONFLICT

如果是冲突更新需要定义 conflict_target，conflict_action 类型有 DO NOTHING、DO UPDATE

```sql
INSERT INTO customer VALUES (100, 'Non-paying customer') ON CONFLICT (id) DO UPDATE SET name = EXCLUDED.name;
```
