import { firefox } from 'playwright';

(async () => {
  // 启动浏览器，并添加一些启动参数以增强隐蔽性
  const browserServer = await firefox.launchServer({
    executablePath: 'E:\\soft\\camoufox-135.0.1-beta.24-win.x86_64\\camoufox.exe',
    headless: false, // 调试时可设置为 false 观察行为
    port: 9222,          // 指定 WS 端口（可选，默认自动分配）
    args: [], // 额外浏览器参数
    env: {
    },
    firefoxUserPrefs: {
      'permissions.default.image': 2,
    }
  });

  // 输出 WebSocket 连接端点
  console.log('WebSocket Endpoint:', browserServer.wsEndpoint());

  // 保持服务器运行（按 Ctrl+C 退出）
  process.on('SIGINT', () => browserServer.close());
})();
