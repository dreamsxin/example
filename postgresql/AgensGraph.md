# AgensGraph
- https://github.com/bitnine-oss/agensgraph

## CREATE Graph
```sql
CREATE GRAPH graphname;
SHOW graph_path;
SET graph_path = graphname;    
```

## DROP Graph
```sql
DROP GRAPH graphname CASCADE;
```

## CREATE Labels

```sql
CREATE VLABEL person;
CREATE VLABEL friend inherits (person);

CREATE ELABEL knows;
CREATE ELABEL live_together;
CREATE ELABEL room_mate inherits (knows, live_together);
```

## DROP Labels
```sql
DROP VLABEL friend;
DROP VLABEL person;

DROP ELABEL knows CASCADE;
```
