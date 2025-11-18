const http = require('http');
const https = require('https');
const readline = require('readline');
const { URL } = require('url');

class TestClient {
    constructor(baseUrl = 'http://localhost:3000') {
        this.baseUrl = baseUrl;
        this.rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
    }

    // 发送HTTP请求的通用方法
    async request(method, path, data = null) {
        return new Promise((resolve, reject) => {
            const url = new URL(path, this.baseUrl);
            const options = {
                hostname: url.hostname,
                port: url.port,
                path: url.pathname + url.search,
                method: method,
                headers: {
                    'Content-Type': 'application/json'
                }
            };

            const lib = url.protocol === 'https:' ? https : http;
            const req = lib.request(options, (res) => {
                let responseData = '';

                res.on('data', (chunk) => {
                    responseData += chunk;
                });

                res.on('end', () => {
                    try {
                        const parsedData = JSON.parse(responseData);
                        resolve({
                            statusCode: res.statusCode,
                            headers: res.headers,
                            data: parsedData
                        });
                    } catch (error) {
                        resolve({
                            statusCode: res.statusCode,
                            headers: res.headers,
                            data: responseData
                        });
                    }
                });
            });

            req.on('error', (error) => {
                reject(error);
            });

            if (data && (method === 'POST' || method === 'PUT')) {
                req.write(JSON.stringify(data));
            }

            req.end();
        });
    }

    // 搜索测试
    async testSearch(keyword) {
        try {
            console.log(`\n🔍 搜索测试: "${keyword}"`);
            const result = await this.request('GET', `/search?keyword=${encodeURIComponent(keyword)}`);
            
            if (result.statusCode === 200) {
                console.log('✅ 搜索成功');
                console.log(`   状态码: ${result.data.statusCode}`);
                console.log(`   数据长度: ${result.data.dataLength} 字符`);
                console.log(`   使用情况: ${result.data.usage.currentUsage}/${result.data.usage.maxUsage}`);
                console.log(`   需要刷新: ${result.data.usage.needsRefresh ? '是' : '否'}`);
            } else {
                console.log('❌ 搜索失败');
                console.log(`   错误: ${JSON.stringify(result.data)}`);
            }
            
            return result;
        } catch (error) {
            console.log('❌ 搜索请求失败:', error.message);
            return null;
        }
    }

    // 获取使用情况
    async testUsage() {
        try {
            console.log('\n📊 获取使用情况');
            const result = await this.request('GET', '/usage');
            
            if (result.statusCode === 200) {
                console.log('✅ 使用情况获取成功');
                console.log(`   当前使用: ${result.data.currentUsage}`);
                console.log(`   最大使用: ${result.data.maxUsage}`);
                console.log(`   需要刷新: ${result.data.needsRefresh ? '是' : '否'}`);
            } else {
                console.log('❌ 使用情况获取失败');
            }
            
            return result;
        } catch (error) {
            console.log('❌ 使用情况请求失败:', error.message);
            return null;
        }
    }

    // 手动刷新测试
    async testRefresh() {
        try {
            console.log('\n🔄 手动刷新测试');
            const result = await this.request('POST', '/refresh');
            
            if (result.statusCode === 200) {
                console.log('✅ 刷新成功');
                console.log(`   消息: ${result.data.message}`);
                console.log(`   使用情况: ${result.data.usage.currentUsage}/${result.data.usage.maxUsage}`);
            } else {
                console.log('❌ 刷新失败');
                console.log(`   错误: ${JSON.stringify(result.data)}`);
            }
            
            return result;
        } catch (error) {
            console.log('❌ 刷新请求失败:', error.message);
            return null;
        }
    }

    // 批量搜索测试
    async testBatchSearch(keywords) {
        console.log(`\n🚀 开始批量搜索测试 (${keywords.length} 个关键词)`);
        
        let successCount = 0;
        let failCount = 0;

        for (let i = 0; i < keywords.length; i++) {
            const keyword = keywords[i];
            console.log(`\n[${i + 1}/${keywords.length}] 测试关键词: "${keyword}"`);
            
            const result = await this.testSearch(keyword);
            
            if (result && result.data && result.data.success) {
                successCount++;
            } else {
                failCount++;
            }

            // 添加延迟，避免请求过快
            await this.delay(500);
        }

        console.log('\n📈 批量测试结果:');
        console.log(`   成功: ${successCount}`);
        console.log(`   失败: ${failCount}`);
        console.log(`   成功率: ${((successCount / keywords.length) * 100).toFixed(2)}%`);

        return { successCount, failCount };
    }

    // 自动测试使用次数限制
    async testUsageLimit() {
        console.log('\n🧪 测试使用次数限制机制');
        
        // 先获取当前使用情况
        const usage = await this.testUsage();
        if (!usage) return;

        const currentUsage = usage.data.currentUsage;
        const maxUsage = usage.data.maxUsage;
        const remaining = maxUsage - currentUsage;

        console.log(`   当前已使用: ${currentUsage} 次`);
        console.log(`   最大限制: ${maxUsage} 次`);
        console.log(`   剩余次数: ${remaining} 次`);

        if (remaining > 0) {
            console.log(`   将进行 ${remaining} 次搜索以触发限制...`);
            
            for (let i = 0; i < remaining; i++) {
                const keyword = `test_keyword_${i + 1}`;
                await this.testSearch(keyword);
                await this.delay(300);
            }

            // 再次检查使用情况
            console.log('\n📋 触发限制后检查:');
            await this.testUsage();
            
            // 再执行一次搜索，应该会自动刷新
            console.log('\n🔄 触发自动刷新测试:');
            await this.testSearch('auto_refresh_test');
        }
    }

    // 延迟函数
    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    // 交互式测试菜单
    async showMenu() {
        console.log('\n🎯 Google搜索测试客户端');
        console.log('========================');
        console.log('1. 单次搜索测试');
        console.log('2. 获取使用情况');
        console.log('3. 手动刷新数据');
        console.log('4. 批量搜索测试');
        console.log('5. 测试使用次数限制');
        console.log('6. 运行完整测试套件');
        console.log('0. 退出');

        return new Promise((resolve) => {
            this.rl.question('\n请选择测试项目 (0-6): ', (answer) => {
                resolve(answer.trim());
            });
        });
    }

    // 处理菜单选择
    async handleMenu(choice) {
        switch (choice) {
            case '1':
                const keyword = await this.prompt('请输入搜索关键词: ');
                await this.testSearch(keyword);
                break;

            case '2':
                await this.testUsage();
                break;

            case '3':
                await this.testRefresh();
                break;

            case '4':
                const keywordsInput = await this.prompt('请输入关键词列表 (用逗号分隔): ');
                const keywords = keywordsInput.split(',').map(k => k.trim()).filter(k => k);
                if (keywords.length > 0) {
                    await this.testBatchSearch(keywords);
                } else {
                    console.log('❌ 请输入有效的关键词列表');
                }
                break;

            case '5':
                await this.testUsageLimit();
                break;

            case '6':
                await this.runFullTestSuite();
                break;

            case '0':
                console.log('👋 再见！');
                this.rl.close();
                return false;

            default:
                console.log('❌ 无效选择，请重新输入');
        }
        return true;
    }

    // 提示输入
    prompt(question) {
        return new Promise((resolve) => {
            this.rl.question(question, (answer) => {
                resolve(answer.trim());
            });
        });
    }

    // 运行完整测试套件
    async runFullTestSuite() {
        console.log('\n🧪 开始完整测试套件\n');

        // 1. 测试服务连通性
        console.log('1. 测试服务连通性...');
        try {
            await this.testUsage();
            console.log('✅ 服务连通性测试通过\n');
        } catch (error) {
            console.log('❌ 服务连通性测试失败，请确保服务正在运行\n');
            return;
        }

        // 2. 单次搜索测试
        console.log('2. 单次搜索测试...');
        await this.testSearch('javascript');
        await this.delay(1000);

        // 3. 批量搜索测试
        console.log('\n3. 批量搜索测试...');
        const testKeywords = ['nodejs', 'python', 'java', 'golang', 'rust'];
        await this.testBatchSearch(testKeywords);
        await this.delay(1000);

        // 4. 使用情况检查
        console.log('\n4. 使用情况检查...');
        await this.testUsage();
        await this.delay(1000);

        // 5. 手动刷新测试
        console.log('\n5. 手动刷新测试...');
        await this.testRefresh();
        await this.delay(1000);

        // 6. 最终使用情况
        console.log('\n6. 最终使用情况...');
        await this.testUsage();

        console.log('\n🎉 完整测试套件执行完成！');
    }

    // 启动交互式客户端
    async start() {
        console.log('🚀 启动测试客户端...');
        console.log(`📡 目标服务: ${this.baseUrl}`);
        console.log('💡 请确保HTTP服务正在运行\n');

        let running = true;
        while (running) {
            const choice = await this.showMenu();
            running = await this.handleMenu(choice);
            
            if (running) {
                await this.prompt('\n按回车键继续...');
            }
        }
    }
}

// 命令行参数处理
function parseArgs() {
    const args = process.argv.slice(2);
    const config = {
        url: 'http://localhost:3000',
        auto: false
    };

    for (let i = 0; i < args.length; i++) {
        if (args[i] === '--url' && args[i + 1]) {
            config.url = args[i + 1];
            i++;
        } else if (args[i] === '--auto') {
            config.auto = true;
        } else if (args[i] === '--help' || args[i] === '-h') {
            console.log(`
使用方法:
  node test-client.js [选项]

选项:
  --url <url>    设置服务URL (默认: http://localhost:3000)
  --auto         自动运行完整测试套件
  --help, -h     显示帮助信息

示例:
  node test-client.js
  node test-client.js --url http://192.168.1.100:3000
  node test-client.js --auto
            `);
            process.exit(0);
        }
    }

    return config;
}

// 主函数
async function main() {
    const config = parseArgs();
    const client = new TestClient(config.url);

    try {
        if (config.auto) {
            // 自动模式：运行完整测试
            await client.runFullTestSuite();
        } else {
            // 交互模式
            await client.start();
        }
    } catch (error) {
        console.error('❌ 客户端运行错误:', error);
    }
}

// 运行客户端
if (require.main === module) {
    main().catch(console.error);
}

module.exports = TestClient;
