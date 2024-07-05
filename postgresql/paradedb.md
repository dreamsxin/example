#

https://github.com/paradedb/paradedb

## Docker

This will start a ParadeDB instance with default user postgres and password postgres
```shell
docker exec -it paradedb psql -U postgres
```

```shell
docker run \
  --name paradedb \
  -e POSTGRESQL_USERNAME=<user> \
  -e POSTGRESQL_PASSWORD=<password> \
  -e POSTGRESQL_DATABASE=<dbname> \
  -e POSTGRESQL_POSTGRES_PASSWORD=<superuser_password> \
  -v paradedb_data:/bitnami/postgresql \
  -p 5432:5432 \
  -d \
  paradedb/paradedb:latest
```
