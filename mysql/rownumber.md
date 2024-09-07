
```sql
SET @row_number = 0; SELECT (@row_number:=@row_number + 1) AS num FROM test;
 SELECT (@i:=@i+1) as rownum FROM test,(select @i:=0) as it;
```
