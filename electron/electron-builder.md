## electron 打包

在Electron中，你可以通过修改`package.json`文件中的build字段来指定生成的EXE文件的图标。以下是一个简化的例子：

首先，确保你有一个图标文件，比如 `app-icon.ico`，并将其放在你的项目中一个合适的地方。

然后，在 `package.json` 中，找到 build 对象，并在win对象中添加一个icon字段，指向你的图标文件：

```json
{
  "name": "your-app",
  "version": "1.0.0",
  "description": "Your Electron app description",
  "main": "main.js",
  "build": {
    "appId": "com.yourapp.id",
    "productName": "YourAppName",
    "directories": {
      "output": "build"
    },
    "win": {
      "icon": "path/to/app-icon.ico"
    }
    // 其他平台的构建配置...
  }
  // 其他配置...
}
```

```shell
npx electron-builder --win
```
