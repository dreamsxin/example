## secret 引擎

## 查看已创建的 secret 引擎
```shell
vault secrets list
```
## 启用 secret 引擎
```
# 默认路径与引擎名相同
vault secrets enable kv
vault secrets enable -path=kv kv
```

## 禁用 secret 引擎
```shell
vault secrets disable kv/
```

## 创建读取删除 secret

```shell
vault kv put kv/hello target=world
vault kv get kv/hello
vault kv delete kv/my-secret
```

## Enable the AWS secrets engine

```shell
vault secrets enable -path=aws aws
```
The AWS secrets engine is now enabled at aws/. Different secrets engines allow for different behavior. In this case, the AWS secrets engine generates dynamic, on-demand AWS access credentials.

### Configure the AWS secrets engine

```shell
export AWS_ACCESS_KEY_ID=<aws_access_key_id>
export AWS_SECRET_ACCESS_KEY=<aws_secret_key>

vault write aws/config/root \
    access_key=$AWS_ACCESS_KEY_ID \
    secret_key=$AWS_SECRET_ACCESS_KEY \
    region=us-east-1
```

### Create a role

```shell
vault write aws/roles/my-role \
        credential_type=iam_user \
        policy_document=-<<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "Stmt1426528957000",
      "Effect": "Allow",
      "Action": [
        "ec2:*"
      ],
      "Resource": [
        "*"
      ]
    }
  ]
}
EOF
```

### Generate the secret

```shell
vault read aws/creds/my-role
```
```output
Key                Value
---                -----
lease_id           aws/creds/my-role/0bce0782-32aa-25ec-f61d-c026ff22106e
lease_duration     768h
lease_renewable    true
access_key         xxx
secret_key         xxx
security_token     <nil>
```

### Revoke the secret

```shell
vault lease revoke aws/creds/my-role/0bce0782-32aa-25ec-f61d-c026ff22106
```
