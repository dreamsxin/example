
## 上一条下一条
```sql
SET @row_number = 0; SELECT (@row_number:=@row_number + 1) AS num FROM test;
 SELECT (@i:=@i+1) as rownum FROM test,(select @i:=0) as it;

-- PREVIOUS RECORD
SELECT row_number, id,name,`date`
FROM (
    SELECT @row := @row + 1 AS row_number, id,name,`date`
    FROM `table` AS t
    JOIN (SELECT @row := 0) r
    ORDER BY `date` DESC, id DESC
) main
WHERE row_number = (
    SELECT current_row - 1
    FROM (
        SELECT @curRow := @curRow + 1 AS current_row, t.id,t.name,t.`date`
        FROM `table` AS t
        JOIN (SELECT @curRow := 0) r 
        ORDER BY `date` DESC, id DESC
    ) t1
    WHERE id = 8
);

-- NEXT RECORD
SELECT row_number, id,name,`date`
FROM (
    SELECT @row := @row + 1 AS row_number, id,name,`date`
    FROM `table` AS t
    JOIN (SELECT @row := 0) r
    ORDER BY `date` DESC, id DESC
) main
WHERE row_number = (
    SELECT current_row + 1
    FROM (
        SELECT @curRow := @curRow + 1 AS current_row, t.id,t.name,t.`date`
        FROM `table` AS t
        JOIN (SELECT @curRow := 0) r 
        ORDER BY `date` DESC, id DESC
    ) t1
    WHERE id = 8
);
```

```sql
-- Previous ID
WITH cte_desc AS (SELECT * FROM `table` ORDER BY `date` DESC, id DESC), cte_r AS (SELECT * FROM `table` WHERE id = @r_id)
SELECT id AS prev_id FROM cte_desc WHERE `date` < (SELECT `date` FROM cte_r) OR `date` = (SELECT `date` FROM cte_r) AND id < (SELECT id FROM cte_r) LIMIT 1;

-- Previous ID
WITH cte_desc AS (SELECT * FROM `table` ORDER BY sort DESC, id DESC)
SELECT id AS prev_id FROM cte_desc WHERE `date` < '2024-09-01 00:00:00' OR (`date` = '2024-09-01 00:00:00' AND id < 1) LIMIT 1;


-- Next ID
WITH cte_asc AS (SELECT * FROM `table` ORDER BY `date`, id),
     cte_r AS (SELECT * FROM `table` WHERE id = @r_id)
SELECT id AS next_id
FROM cte_asc
WHERE `date` > (SELECT `date` FROM cte_r)
   OR `date` = (SELECT `date` FROM cte_r) AND id > (SELECT id FROM cte_r)
LIMIT 1;
```
