# MOSN

MOSN（Modular Open Smart Network）是一款主要使用 Go 语言开发的云原生网络代理平台，由蚂蚁集团开源并经过双 11 大促几十万容器的生产级验证。 MOSN 为服务提供多协议、模块化、智能化、安全的代理能力，融合了大量云原生通用组件，同时也可以集成 Envoy 作为网络库，具备高性能、易扩展的特点。 MOSN 可以和 Istio 集成构建 Service Mesh，也可以作为独立的四、七层负载均衡，API Gateway、云原生 Ingress 等使用。

https://github.com/mosn/mosn/

## Layotto

Layotto (/leɪˈɒtəʊ/) 是一款使用 Golang 开发的应用运行时，旨在帮助开发人员快速构建云原生应用，帮助应用和基础设施解耦。它为应用提供了各种分布式能力，比如状态管理，配置管理，事件发布订阅等能力，以简化应用的开发。

Layotto 以开源的 MOSN 为底座，在提供分布式能力以外，提供了 Service Mesh 对于流量的管控能力。

Layotto 希望可以把 Multi-Runtime 跟 Service Mesh 两者的能力结合起来，无论你是使用 MOSN 还是 Envoy 或者其他产品作为 Service Mesh 的数据面，都可以在不增加新的 sidecar 的前提下，使用 Layotto 为这些数据面追加 Runtime 的能力。

https://github.com/mosn/layotto/

