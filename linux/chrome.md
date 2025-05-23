## 注意事项

**无法启动**

下载的安装文件所属用户分组都要是分 root，否则安装时会有一个警告提示，而且无法启动。
`N: Download is performed unsandboxed as root as file '/home/ubuntu/Downloads/xxxx_amd64_06-03_20.04.deb' couldn't be accessed by user '_apt'. - pkgAcquire::Run (13: Permission denied)`
