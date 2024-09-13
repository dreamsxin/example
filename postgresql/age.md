https://github.com/apache/age

## Quick Start
To create a graph, use the create_graph function located in the ag_catalog namespace.
```sql
SELECT create_graph('graph_name');
```
To create a single vertex with label and properties, use the CREATE clause.
```sql
SELECT * 
FROM cypher('graph_name', $$
    CREATE (:label {property:"Node A"})
$$) as (v agtype);
SELECT * 
FROM cypher('graph_name', $$
    CREATE (:label {property:"Node B"})
$$) as (v agtype);
```
To create an edge between two nodes and set its properties:
```sql
SELECT * 
FROM cypher('graph_name', $$
    MATCH (a:label), (b:label)
    WHERE a.property = 'Node A' AND b.property = 'Node B'
    CREATE (a)-[e:RELTYPE {property:a.property + '<->' + b.property}]->(b)
    RETURN e
$$) as (e agtype);
```
And to query the connected nodes:
```sql
SELECT * from cypher('graph_name', $$
        MATCH (V)-[R]-(V2)
        RETURN V,R,V2
$$) as (V agtype, R agtype, V2 agtype);
```
