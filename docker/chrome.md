```shell
docker run -d -p 2222:22 --name ubuntu-ssh -v app_data:/data dreamsxin/ubuntu-ssh-volumes:noble
```

```shell
sudo apt-get install xvfb
sudo apt-get install libnspr4 libnss3 libatk1.0-0t64 libatk-bridge2.0-0t64 libcups2t64 libxkbcommon0 libxcomposite1 libxdamage1 libxfixes3 libcairo2 libpango-1.0-0 libasound2t64

Xvfb :99 -screen 0 1024x768x16 -nolisten tcp -nolisten unix &
export DISPLAY=:99
```
