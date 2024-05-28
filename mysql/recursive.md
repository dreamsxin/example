##

```sql
with recursive rnums as (
  select 1 as n
      union all
  select n+1 as n from rnums
      where n <10
)
select * from rnums;
```

## Get the First Row per Group
```sql
with rows_and_position as 
  ( 
    select emp_id,
    last_name, 
    salary,
    dep_id,
    row_number() over (partition by dep_id order by salary desc) as position
    from employee
  )
select dep_id, last_name, salary
from  rows_and_position
where position = 1;
```
