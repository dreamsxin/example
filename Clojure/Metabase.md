
```shell
docker run -d -p 3000:3000 -v /mnt/docker_data/metabase:/tmp -e "MB_DB_FILE=/tmp/metabase.db" --name metabase metabase/metabase
```
**用 docker-compose去启动管理**
```yml
version: '2'

services:
  metabase:
    image: metabase/metabase:latest
    ports:
      - "3000:3000"
    volumes:
      - /mnt/docker_data/metabase:/tmp
    environment:
      - MB_DB_FILE=/tmp/metabase.db
```
