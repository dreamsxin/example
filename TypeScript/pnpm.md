## On Windows
Using PowerShell:
```shell
iwr https://get.pnpm.io/install.ps1 -useb | iex
```
## On POSIX systems
```shell
curl -fsSL https://get.pnpm.io/install.sh | sh -
```
If you don't have curl installed, you would like to use wget:
```shell
wget -qO- https://get.pnpm.io/install.sh | sh -
```
