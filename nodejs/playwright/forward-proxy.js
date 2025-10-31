import http from 'http';
import net from 'net';
import url from 'url';

// 代理配置
const PROXY_CONFIG = {
  // 本地代理端口
  localPort: 8899,
  // 上游代理配置 (如果有的话)
  upstreamProxy: {
    protocol: 'http',
    host: 'localhost',  // 上游代理主机
    port: 80,         // 上游代理端口
    auth: {
       username: '',
       password: ''
    }
  },
  // 是否使用上游代理
  useUpstreamProxy: false
};

// 创建简单的 HTTP 代理服务器
class ForwardProxy {
  constructor(config) {
    this.config = config;
    this.server = null;
  }

  start() {
    return new Promise((resolve, reject) => {
      this.server = http.createServer();
      
      // 添加服务器级别的错误处理
      this.server.on('clientError', (err, socket) => {
        console.error('[代理] 客户端错误:', err);
        if (socket.writable) {
          socket.end('HTTP/1.1 400 Bad Request\r\n\r\n');
        }
      });

      this.server.on('error', (err) => {
        console.error('[代理] 服务器错误:', err);
        if (err.code === 'EADDRINUSE') {
          console.error(`[代理] 端口 ${this.config.localPort} 已被占用`);
        }
      });
      
      this.server.on('request', (req, res) => {
        //console.log(`[代理] HTTP 请求: ${req.method} ${req.url}`);
        
        // 添加错误处理
        req.on('error', (err) => {
          console.error('[代理] 请求错误:', err);
          if (!res.headersSent) {
            res.writeHead(500);
            res.end('Request Error');
          }
        });

        res.on('error', (err) => {
          console.error('[代理] 响应错误:', err);
        });
        
        const parsedUrl = url.parse(req.url);
        const options = {
          hostname: parsedUrl.hostname,
          port: parsedUrl.port || 80,
          path: parsedUrl.path,
          method: req.method,
          headers: req.headers,
          timeout: 30000, // 30秒超时
          rejectUnauthorized: false // 忽略SSL证书错误
        };

        // 如果使用上游代理，修改请求选项
        if (this.config.useUpstreamProxy) {
          options.hostname = this.config.upstreamProxy.host;
          options.port = this.config.upstreamProxy.port;
          options.path = req.url;
          options.headers = {
            ...req.headers,
            'Proxy-Connection': 'keep-alive'
          };

          // 如果需要认证
          if (this.config.upstreamProxy.auth) {
            const auth = Buffer.from(
              `${this.config.upstreamProxy.auth.username}:${this.config.upstreamProxy.auth.password}`
            ).toString('base64');
            options.headers['Proxy-Authorization'] = `Basic ${auth}`;
          }
        }

        const proxyReq = http.request(options, (proxyRes) => {
          try {
            res.writeHead(proxyRes.statusCode, proxyRes.headers);
            proxyRes.pipe(res);
          } catch (err) {
            console.error('[代理] 响应处理错误:', err);
            if (!res.headersSent) {
              res.writeHead(500);
              res.end('Response Error');
            }
          }
        });

        proxyReq.on('error', (err) => {
          console.error('[代理] 代理请求错误:', err);
          if (!res.headersSent) {
            res.writeHead(500);
            res.end('Proxy Request Error');
          }
        });

        proxyReq.on('timeout', () => {
          console.error('[代理] 代理请求超时');
          proxyReq.destroy();
          if (!res.headersSent) {
            res.writeHead(504);
            res.end('Gateway Timeout');
          }
        });

        req.on('aborted', () => {
          //console.log('[代理] 客户端中止连接');
          proxyReq.destroy();
        });

        req.pipe(proxyReq);
      });

      this.server.on('connect', (req, socket, head) => {
        //console.log(`[代理] HTTPS 连接: ${req.url}`);
        
        const [hostname, port] = req.url.split(':');
        const targetPort = port || 443;

        // 添加socket错误处理
        socket.on('error', (err) => {
          console.error('[代理] 客户端socket错误:', err);
        });

        // 创建到目标服务器的连接
        const serverSocket = net.connect(targetPort, hostname, () => {
          try {
            socket.write('HTTP/1.1 200 Connection Established\r\n\r\n');
            serverSocket.write(head);
            serverSocket.pipe(socket);
            socket.pipe(serverSocket);
          } catch (err) {
            console.error('[代理] HTTPS连接建立错误:', err);
            socket.end();
          }
        });

        serverSocket.on('error', (err) => {
          //console.error('[代理] HTTPS 目标服务器连接错误:', err);
          try {
            socket.end();
          } catch (socketErr) {
            console.error('[代理] socket关闭错误:', socketErr);
          }
        });

        serverSocket.on('timeout', () => {
          //console.error('[代理] HTTPS 连接超时');
          serverSocket.destroy();
          socket.end();
        });

        socket.on('close', () => {
          //console.log('[代理] 客户端连接关闭');
          serverSocket.destroy();
        });

        serverSocket.on('close', () => {
          //console.log('[代理] 目标服务器连接关闭');
          socket.end();
        });
      });

      this.server.listen(this.config.localPort, (err) => {
        if (err) {
          console.error(`[代理] 服务器启动失败:`, err);
          reject(err);
        } else {
          //console.log(`[代理] 本地代理服务器启动在端口 ${this.config.localPort}`);
          resolve();
        }
      });

      // 设置服务器超时
      this.server.timeout = 60000; // 60秒超时
    });
  }

  stop() {
    return new Promise((resolve) => {
      if (this.server) {
        this.server.close(() => {
          //console.log('[代理] 本地代理服务器已停止');
          resolve();
        });
      } else {
        resolve();
      }
    });
  }
}

export { PROXY_CONFIG, ForwardProxy };
