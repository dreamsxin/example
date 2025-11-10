## SSL/TLS全面诊断
```sh
#!/bin/bash
DOMAIN="$1"
PORT="443"

echo "=== SSL/TLS全面诊断 ==="
echo "目标: $DOMAIN:$PORT"

# 1. 基础连接测试
echo -e "\n1. 基础连接测试:"
timeout 10 openssl s_client -connect $DOMAIN:$PORT -servername $DOMAIN < /dev/null > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "✓ SSL连接成功"
else
    echo "✗ SSL连接失败"
    exit 1
fi

# 2. 证书信息
echo -e "\n2. 证书信息:"
echo | openssl s_client -servername $DOMAIN -connect $DOMAIN:$PORT 2>/dev/null | openssl x509 -noout -subject -issuer -dates

# 3. 协议支持
echo -e "\n3. 协议支持:"
for proto in ssl2 ssl3 tls1 tls1_1 tls1_2 tls1_3; do
    echo -n "测试 $proto: "
    openssl s_client -connect $DOMAIN:$PORT -$proto < /dev/null > /dev/null 2>&1
    [ $? -eq 0 ] && echo "支持" || echo "不支持"
done

# 4. 性能测试
echo -e "\n4. SSL握手性能:"
for i in {1..3}; do
    time=$(curl -w "%{time_appconnect}" -o /dev/null -s https://$DOMAIN)
    echo "第$i次握手: ${time}秒"
done
```
