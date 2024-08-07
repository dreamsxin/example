#
Psycopg – PostgreSQL database adapter for Python

- https://github.com/psycopg/psycopg
- https://github.com/psycopg/psycopg2

>> Windows 下建议使用 Anaconda Powershell Prompt

## Install

```shell
pip install --upgrade pip           # upgrade pip to at least 20.3
pip install "psycopg[binary]"       # remove [binary] for PyPy
```
```shell
pip install psycopg
```
without [c] or [binary] extras you will obtain a pure Python implementation. This is particularly handy to debug and hack, but it still requires the system libpq to operate (which will be imported dynamically via ctypes).

## pandas

```python
import pandas as pd
import psycopg2 as pg

engine = pg.connect("dbname='my_db_name' user='pguser' host='127.0.0.1' port='15432' password='pgpassword'")
df = pd.read_sql('select * from Stat_Table', con=engine)
```
