```sql
with recursive rnums as (
  select 1 as n
      union all
  select n+1 as n from rnums
      where n <10
)
select * from rnums;
```
