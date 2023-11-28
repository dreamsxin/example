# service
Service：将运行在一组 Pods 上的应用程序公开为网络服务（微服务）的抽象方法。
k8s 以 Pod 应用部署的最小单位。k8s 根据调度算法为 Pod 分配运行节点，并随机分配 ip 地址。因此 Pod 的 IP 地址不固定，不方便通过 Pod 的 IP 地址访问服务。
因此，k8s 提供了 Service，对后端提供相同服务的一组 Pod 的聚合，通常是通过selector实现的。

一个 Service 提供一个虚拟的 Cluster IP，后端对应一个或者多个提供服务的 Pod。在集群中访问该 Service 时，采用 Cluster IP 即可，Kube-proxy 负责将发送到 Cluster IP 的请求转发到后端的Pod上。
## service 的四种类型

- ClusterIP：默认值，通过集群内部 ip 暴露服务。该服务只能在集群内部访问。
- NodePort：通过每个节点上的 IP 和静态端口（NodePort）暴露服务。除了集群内部访问，集群外部可以通过 IP:NodePort访问该服务。
- ExternalName：将服务映射到 DNS 名称，把集群外部的服务引入到集群内部来使用
- LoadBalancer：使用云提供商的负载均衡器向外部暴露服务。 将来自外部负载均衡器的流量路由到后端的 pod 上，不再需要内部的负载均衡。

## 创建 Service
微服务暴露端口：443, 80, 6379，k8s 为其分配一个集群 ip: ClusterIP。
```yml
 apiVersion: v1
 kind: Service
 metadata:
   name: myapp-svc
 spec:
   # service 发布类型
   type: ClusterIP
   # 标签选择器：选择 pod
   selector:
     app: myapp
   # 指定端口数组
   ports:
     # service 端口
   - port: 443
     # 协议
     protocol: TCP
     # 容器端口
     targetPort: 443
     # 端口名称 
     name: https
   - port: 80
     protocol: TCP
     targetPort: 80
     name: http
   - port: 6379
     protocol: TCP
     targetPort: 6379
     name: redis-tcp
```

### 没有`spec.selector`的 Service
`notselector-svc`
```yml
 apiVersion: v1
 kind: Service
 metadata:
   name: notselector-svc
 spec:
   ports:
   - protocol: TCP
     port: 8080
     targetPort: 80
```
新建 Endpoints

```yml
apiVersion: v1
 kind: Endpoints
 metadata:
   name: notselector-svc-points
 subsets:
 - addresses:
   - ip: 10.244.1.60
   ports:
   - port: 80
 - addresses:
   # 非集群节点的机器：集群外部服务
   - ip: 192.168.88.131
   ports:
   - port: 80
```
```syell
kubectl apply -f notselector-svc.yaml
kubectl describe -f notselector-svc.yaml
 ​
kubectl apply -f notselector-points.yaml
kubectl describe -f notselector-points.yaml
```
