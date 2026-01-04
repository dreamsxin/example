- https://www.mongodb.com/try/download/compass
-
## 快速安装 MongoDB

‌**拉取官方镜像**‌
`docker pull mongodb/mongodb-enterprise-server:latest`

‌**运行容器**‌
`docker run --name mongodb -p 27017:27017 mongodb/mongodb-enterprise-server`
（将容器内 27017 端口映射到主机 27017 端口） 
