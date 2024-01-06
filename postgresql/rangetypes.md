#

https://www.postgresql.org/docs/current/rangetypes.html

```sql
SELECT company_id, user_id, company_id, device_id, clientid, MIN(trace_time) login, MAX(trace_time) logout, EXTRACT(EPOCH FROM (MAX(trace_time) - MIN(trace_time))) FROM tg_user_event_reports GROUP BY company_id, user_id, company_id, device_id, clientid;
WITH cte AS (
    SELECT company_id, user_id, company_id, device_id, clientid, MIN(trace_time) login, MAX(trace_time) logout, EXTRACT(EPOCH FROM (MAX(trace_time) - MIN(trace_time))) FROM tg_user_event_reports GROUP BY company_id, user_id, company_id, device_id, clientid
)
SELECT * FROM cte WHERE tstzrange(login, logout) @> '2024-01-06 14:07:03'::timestamptz;
```
