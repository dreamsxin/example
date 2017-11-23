# Traefik

是一个与 Nginx、HAProxy 有些相似的HTTP反向代理服务器，兼有负载均衡的功能。
Nginx 和 HAProxy 都有一个相同的问题就是，后端服务（通常称之为upstream或backend）变化（是否能正常工作、上线、下线、扩展）时，不容易动态更新Nginx和HAProxy的配置文件和重载服务，尽管有一些类似于 Registrator, Consul 和 Consul-Template 这样的工具可以来做这样的事。

与 Nginx、HAProxy 不同的是，Traefik 更适合需要服务发现和服务注册的应用场景。
例如，Traefik 与 Docker 相结合非常容易，只需要指定 label 即可（虽然可以使用`docker run`指定 label，但更推荐使用`docker-compose.yml`指定）。
演示的例子直接可以参考官方网站，也可以参考下面的例子，非常简单并通俗易懂。需要指出的是 Traefik 并不是只能与Docker相结合，Docker, Swarm, Mesos/Marathon, Consul, Etcd, Zookeeper, BoltDB, Amazon ECS, Rest API, file...都可以。

## 一个简单的演示

1. 创建Traefik服务，可以使用Docker也可以使用命令行的方式。

注意：不熟悉docker-compose的可以先学习一下docker-compose的语法和应用，或者直接忽略它，再根据下文的相对目录结构、文件和命令创建。

docker-compose.yml 文件内容如下：
```yml
version: '2'    
services:     
  traefik:     
    image: traefik:latest     
    restart: unless-stopped     
    command: --web --docker --docker.domain=docker.localhost --logLevel=DEBUG     
    networks:     
      - webgateway     
    ports:     
      - "80:80"     
      - "8080:8080"     
      - "443:443"     
    volumes:     
      - /var/run/docker.sock:/var/run/docker.sock     
      - /dev/null:/traefik.toml
networks:    
  webgateway:     
    driver: bridge
```

2. 使用whoami应用作为一个简易的HTTP Web服务：

docker-compose.yml 文件内容如下：
```yml
version: '2'    
services:     
  whoami:     
    image: emilevauge/whoami     
    networks:     
      - web     
    labels:     
      - "traefik.backend=whoami"     
      - "traefik.frontend.rule=Host:whoami.docker.localhost"
networks:    
  web:     
    external:     
      name: traefikself_webgateway
```

3. 查看Traefik日志

通过`docker-compose logs traefik`命令：
```shell
docker-compose logs traefik|tail
```

4. 使用`docker-compose scale whoami=3`命令扩展成3个whoami应用，观察访问情况，通过CURL结果可以发现默认是轮询的 wrr。
5. Traefik 有一个`Dashboard Web UI`，可以通过网页了解当前 Traefik 中的运行情况和节点的健康状态。
