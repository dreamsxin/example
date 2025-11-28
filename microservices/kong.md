## kong

- https://developer.konghq.com/gateway/install/

```bash
curl -1sLf "https://packages.konghq.com/public/gateway-312/gpg.875433A518B93006.key" |  gpg --dearmor | sudo tee /usr/share/keyrings/kong-gateway-312-archive-keyring.gpg > /dev/null
curl -1sLf "https://packages.konghq.com/public/gateway-312/config.deb.txt?distro=debian&codename=$(lsb_release -sc)" | sudo tee /etc/apt/sources.list.d/kong-gateway-312.list > /dev/null
sudo apt-get update && sudo apt install -y kong-enterprise-edition=3.12.0.1
```

## goma

```bash
docker run --rm --name goma-gateway -v "${PWD}/config:/etc/goma/" jkaninda/goma-gateway config init --output /etc/goma/config.yml
docker run --rm --name goma-gateway -v "${PWD}/config:/etc/goma/" jkaninda/goma-gateway config check --config /etc/goma/config.yml
```

