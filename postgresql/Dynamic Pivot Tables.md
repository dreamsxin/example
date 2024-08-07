# 透视表

## 例子

https://bender.io/2016/09/18/dynamic-pivot-tables-with-json-and-postgresql/

```sql
CREATE TABLE book (
  book_id INTEGER PRIMARY KEY,
  title TEXT NOT NULL,
  author TEXT NOT NULL,
  publication_year INTEGER NOT NULL,
  genre TEXT NOT NULL,
  price MONEY NOT NULL
);

CREATE TABLE customer (
  customer_id INTEGER PRIMARY KEY,
  firstname TEXT NOT NULL,
  lastname TEXT NOT NULL,
  state CHAR(2) NOT NULL
);

CREATE TABLE sale (
  sale_id INTEGER PRIMARY KEY,
  book_id INTEGER NOT NULL,
  customer_id INTEGER NOT NULL,
  sale_date DATE NOT NULL
);
```

## Crosstab Solution

```sql
CREATE EXTENSION IF NOT EXISTS tablefunc;

SELECT * FROM crosstab(
  $$
    SELECT
      date_part('year', sale_date) AS year,
      date_part('month', sale_date) AS month,
      COUNT(*)
    FROM sale
    GROUP BY sale_date
    ORDER BY 1
  $$,
  $$ SELECT m FROM generate_series(1,12) m $$
) AS (
  year int,
  "Jan" int,
  "Feb" int,
  "Mar" int,
  "Apr" int,
  "May" int,
  "Jun" int,
  "Jul" int,
  "Aug" int,
  "Sep" int,
  "Oct" int,
  "Nov" int,
  "Dec" int
);
```

查询返回值
```txt
year | Jan | Feb | Mar | Apr | May | Jun | Jul | Aug | Sep | Oct | Nov | Dec
-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----
2010 |   6 |   2 |   5 |   3 |   4 |   7 |   6 |   5 |   2 |   6 |   2 |   1
2011 |   2 |   7 |   1 |   7 |   5 |   4 |   1 |   5 |   1 |  10 |   1 |   1
2012 |   4 |   4 |   7 |   4 |   5 |   3 |   3 |   4 |   2 |   6 |   3 |   3
2013 |   7 |   4 |   7 |   3 |   2 |   6 |   5 |   4 |   2 |   2 |   2 |   6
2014 |   4 |   3 |   6 |   3 |   1 |   4 |   3 |   4 |   4 |   7 |   5 |   3
2015 |   2 |   3 |   3 |   4 |   2 |   3 |   3 |   5 |   3 |   4 |   1 |   4
2016 |   5 |   5 |   6 |   8 |   2 |   5 |   4 |   4 |   6 |   1 |   3 |   6
```

## JSON Solution
```sql
WITH month_total AS (
  SELECT
    date_part('year', sale_date) AS year,
    to_char(sale_date, 'Mon') AS month,
    COUNT(*) AS total
  FROM sale
  GROUP BY date_part('year', sale_date), to_char(sale_date, 'Mon')
)

SELECT
  year,
  jsonb_object_agg(month,total) AS months
FROM month_total
GROUP BY year
ORDER BY year;
```

```txt
year |                                  months
-----+-------------------------------------------------------------------------------
2014 | {"Jan": 132, "Feb": 109, "Mar": 118, "Apr": 126, "May": 131, "Jun": 111, ... }
2015 | {"Jan": 103, "Feb": 115, "Mar": 113, "Apr": 137, "May": 113, "Jun": 128, ... }
2012 | {"Jan": 110, "Feb": 109, "Mar": 115, "Apr": 109, "May": 126, "Jun": 96, ... }
2013 | {"Jan": 122, "Feb": 121, "Mar": 120, "Apr": 120, "May": 106, "Jun": 128, ... }
2011 | {"Jan": 128, "Feb": 121, "Mar": 109, "Apr": 116, "May": 120, "Jun": 104, ... }
2010 | {"Jan": 124, "Feb": 111, "Mar": 120, "Apr": 111, "May": 115, "Jun": 112, ... }
2016 | {"Jan": 118, "Feb": 122, "Mar": 116, "Apr": 111, "May": 112, "Jun": 111, ... }
```

## Extending this Technique
Let’s explore how simple this makes it to adjust which slice of our little OLAP cube we’re viewing:
```sql
WITH state_total AS (
  SELECT
    c.state,
    b.genre,
    COUNT(*) AS total
  FROM sale s
  INNER JOIN book b USING(book_id)
  INNER JOIN customer c USING(customer_id)
  GROUP BY c.state, b.genre
)

SELECT
  state,
  jsonb_object_agg(genre, total) AS months
FROM state_total
GROUP BY state
ORDER BY state;
```
