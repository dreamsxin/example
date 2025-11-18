const { chromium } = require('playwright');
const https = require('https');
const http = require('http');
const { faker } = require("@faker-js/faker");
const fs = require('fs');
const ProxyChain = require("proxy-chain");
const url = require('url');
const UserAgent = require('user-agents');

class BrowserRequestManager {
    constructor() {
        this.browser = null;
        this.context = null;
        this.page = null;
        this.capturedData = {
            url: '',
            cookies: [],
            headers: {},
            data: {}
        };
        this.usageCount = 0;
        this.maxUsageCount = 10;
        this.isRefreshing = false;
        this.refreshQueue = [];
    }

    async init() {
        const fingerprintSeed = Math.floor(Math.random() * 200000);
        // 启动浏览器
        this.browser = await chromium.launch({
            executablePath: "E:\\soft\\ungoogled-chromium_138.0.7204.183-1.1_windows_x64\\chrome.exe",
            headless: false,
            slowMo: 100,
            args: [
                `--fingerprint=${fingerprintSeed}`,
                "--no-first-run",
                "--no-default-browser-check",
                "--disable-default-apps",
                "--disable-blink-features=AutomationControlled",
                "--disable-background-timer-throttling",
                "--disable-backgrounding-occluded-windows",
                "--disable-renderer-backgrounding",
                "--disable-features=VizDisplayCompositor",
                "--disable-accelerated-2d-canvas",
                "--disable-gpu",
                '--disable-features=TranslateUI',
                '--disable-ipc-flooding-protection',
                '--enable-features=NetworkService,NetworkServiceInProcess',
                '--disable-renderer-backgrounding',
            ],
        });

        const newProxyUrl = await ProxyChain.anonymizeProxy("http://xxxxx");
        console.log("Anonymized proxy:", newProxyUrl);
        const userAgent = new UserAgent({ userAgent: /Safari/, deviceCategory: 'desktop' })
        this.context = await this.browser.newContext({
            proxy: newProxyUrl ? { server: newProxyUrl } : undefined,
            userAgent: userAgent.toString(),
        });

        this.page = await this.context.newPage();
    }

    async captureGoogleData(keyword = null) {
        try {
            console.log('正在访问 Google...');

            const headersPromise = this.captureHeaders();

            await this.page.goto('https://www.google.com', {
                waitUntil: 'commit',
                timeout: 5000
            });

            const searchBoxSelector = 'textarea[name="q"], input[name="q"]';
            await this.page.waitForSelector(searchBoxSelector, { timeout: 2000, state: 'visible' });

            this.capturedData.word = keyword || faker.word.sample();
            await this.page.fill(searchBoxSelector, this.capturedData.word);
            await this.page.keyboard.press("Enter");

            // 等待搜索结果页面加载
            await this.page.waitForURL("**/search*", {
                waitUntil: "commit",
                timeout: 5000,
            });

            this.capturedData.url = this.page.url();
            console.log('当前 URL:', this.capturedData.url);

            const cookies = await this.context.cookies();
            this.capturedData.cookies = cookies;
            console.log('获取到的 Cookies:', cookies.length, '个');

            this.capturedData.headers = await headersPromise;
            console.log('获取到的 Headers:', Object.keys(this.capturedData.headers).length, '个');

            this.capturedData.statusCode = 200;
            this.capturedData.data = await this.page.content();

            this.usageCount = 0; // 重置使用计数
            this.isRefreshing = false;

            // 处理等待中的请求
            while (this.refreshQueue.length > 0) {
                const { resolve, reject } = this.refreshQueue.shift();
                resolve(this.capturedData);
            }

            return this.capturedData;

        } catch (error) {
            console.error('捕获数据时出错:', error);
            this.isRefreshing = false;

            // 处理等待中的请求错误
            while (this.refreshQueue.length > 0) {
                const { reject } = this.refreshQueue.shift();
                reject(error);
            }
            throw error;
        }
    }

    async getCapturedData(keyword = null) {
        // 如果使用次数超过限制，重新获取数据
        if (this.usageCount >= this.maxUsageCount || !this.capturedData.url) {
            if (this.isRefreshing) {
                // 如果正在刷新，等待刷新完成
                return new Promise((resolve, reject) => {
                    this.refreshQueue.push({ resolve, reject });
                });
            }

            this.isRefreshing = true;
            await this.captureGoogleData(keyword);
        }

        return this.capturedData;
    }

    async captureHeaders() {
        return new Promise((resolve) => {
            const headers = {};

            this.page.on('request', (request) => {
                const requestHeaders = request.headers();
                if (request.url().includes('google.com')) {
                    Object.assign(headers, requestHeaders);
                }
            });

            setTimeout(() => {
                resolve(headers);
            }, 3000);
        });
    }

    buildCookieString() {
        return this.capturedData.cookies
            .map(cookie => `${cookie.name}=${cookie.value}`)
            .join('; ');
    }

    async makeHttpRequest(url, options = {}) {
        try {
            const capturedData = await this.getCapturedData();
            this.usageCount++;

            return new Promise((resolve, reject) => {
                const parsedUrl = new URL(url);
                const isHttps = parsedUrl.protocol === 'https:';
                const lib = isHttps ? https : http;

                const requestOptions = {
                    hostname: parsedUrl.hostname,
                    port: parsedUrl.port || (isHttps ? 443 : 80),
                    path: parsedUrl.pathname + parsedUrl.search,
                    method: 'GET',
                    headers: {
                        ...capturedData.headers,
                        ...options.headers
                    }
                };

                if (capturedData.cookies.length > 0) {
                    requestOptions.headers['Cookie'] = this.buildCookieString();
                }

                console.log(`发送请求到: ${url} (使用计数: ${this.usageCount}/${this.maxUsageCount})`);

                const req = lib.request(requestOptions, (res) => {
                    let data = '';

                    res.on('data', (chunk) => {
                        data += chunk;
                    });

                    res.on('end', () => {
                        const result = {
                            statusCode: res.statusCode,
                            headers: res.headers,
                            data: data
                        };
                        resolve(result);
                    });
                });

                req.on('error', (error) => {
                    reject(error);
                });

                req.setTimeout(30000, () => {
                    req.destroy(new Error('请求超时'));
                });

                req.end();
            });
        } catch (error) {
            console.error('请求失败:', error);
            throw error;
        }
    }

    async searchWithKeyword(keyword) {
        try {
            const capturedData = await this.getCapturedData(keyword);
            if (capturedData.word == keyword) {
                return {
                    statusCode: capturedData.statusCode,
                    headers: capturedData.headers,
                    data: capturedData.data
                };
            }
            this.usageCount++;

            // 构建搜索URL
            const searchUrl = new URL(capturedData.url);
            searchUrl.searchParams.set('q', keyword);

            return await this.makeHttpRequest(searchUrl.toString());
        } catch (error) {
            console.error('搜索失败:', error);
            throw error;
        }
    }

    async close() {
        if (this.browser) {
            await this.browser.close();
        }
    }

    getUsageInfo() {
        return {
            currentUsage: this.usageCount,
            maxUsage: this.maxUsageCount,
            needsRefresh: this.usageCount >= this.maxUsageCount
        };
    }
}

// HTTP 服务类
class HttpService {
    constructor(port = 3000) {
        this.port = port;
        this.browserManager = new BrowserRequestManager();
        this.isInitialized = false;
    }

    async initialize() {
        if (!this.isInitialized) {
            await this.browserManager.init();
            this.isInitialized = true;
            console.log('Browser manager initialized');
        }
    }

    start() {
        const server = http.createServer(async (req, res) => {
            // 设置CORS头
            res.setHeader('Access-Control-Allow-Origin', '*');
            res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
            res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

            if (req.method === 'OPTIONS') {
                res.writeHead(200);
                res.end();
                return;
            }

            try {
                const parsedUrl = url.parse(req.url, true);
                const pathname = parsedUrl.pathname;

                if (pathname === '/search' && req.method === 'GET') {
                    await this.handleSearch(req, res, parsedUrl.query);
                } else if (pathname === '/usage' && req.method === 'GET') {
                    await this.handleUsageInfo(req, res);
                } else if (pathname === '/refresh' && req.method === 'POST') {
                    await this.handleRefresh(req, res);
                } else {
                    this.sendResponse(res, 404, { error: '接口不存在' });
                }
            } catch (error) {
                console.error('HTTP服务错误:', error);
                this.sendResponse(res, 500, { error: '服务器内部错误' });
            }
        });

        server.listen(this.port, () => {
            console.log(`HTTP服务运行在 http://localhost:${this.port}`);
        });

        server.on('error', (error) => {
            console.error('服务器错误:', error);
        });
    }

    async handleSearch(req, res, query) {
        const keyword = query.keyword;

        if (!keyword) {
            this.sendResponse(res, 400, { error: '缺少关键词参数' });
            return;
        }

        try {
            if (!this.isInitialized) {
                await this.initialize();
            }

            const result = await this.browserManager.searchWithKeyword(keyword);

            if (result.statusCode === 200) {

                // 保存内容到文件
                const outputDir = "scraped-content";
                if (!fs.existsSync(outputDir)) {
                    fs.mkdirSync(outputDir);
                }
                fs.appendFile(
                    outputDir + "/" + query.keyword + "_" + (new Date().getTime()) + ".html",
                    result.data + "\r\n",
                    "utf8",
                    (err) => {
                        if (err) {
                            console.error("追加内容时发生错误:", err);
                        }
                        console.log("内容已追加到文件！");
                    }
                );
                this.sendResponse(res, 200, {
                    success: true,
                    keyword: keyword,
                    statusCode: result.statusCode,
                    dataLength: result.data.length,
                    usage: this.browserManager.getUsageInfo()
                });
            } else {
                this.sendResponse(res, 200, {
                    success: false,
                    keyword: keyword,
                    statusCode: result.statusCode,
                    error: '请求失败',
                    usage: this.browserManager.getUsageInfo()
                });
            }
        } catch (error) {
            console.error('搜索处理错误:', error);
            this.sendResponse(res, 500, {
                error: '搜索失败',
                message: error.message,
                usage: this.browserManager.getUsageInfo()
            });
        }
    }

    async handleUsageInfo(req, res) {
        try {
            const usageInfo = this.browserManager.getUsageInfo();
            this.sendResponse(res, 200, usageInfo);
        } catch (error) {
            this.sendResponse(res, 500, { error: '获取使用信息失败' });
        }
    }

    async handleRefresh(req, res) {
        try {
            await this.browserManager.captureGoogleData();
            this.sendResponse(res, 200, {
                message: '数据刷新成功',
                usage: this.browserManager.getUsageInfo()
            });
        } catch (error) {
            this.sendResponse(res, 500, { error: '数据刷新失败' });
        }
    }

    sendResponse(res, statusCode, data) {
        res.writeHead(statusCode, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(data, null, 2));
    }

    async close() {
        if (this.browserManager) {
            await this.browserManager.close();
        }
    }
}

// 使用示例
async function main() {
    const httpService = new HttpService(3000);

    // 启动HTTP服务
    httpService.start();

    // 优雅关闭
    process.on('SIGINT', async () => {
        console.log('正在关闭服务...');
        await httpService.close();
        process.exit(0);
    });
}

// 运行示例
if (require.main === module) {
    main();
}

module.exports = { BrowserRequestManager, HttpService };
