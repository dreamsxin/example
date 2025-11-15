# JavaScript Hook 技术详细教程

Hook（钩子）技术是JavaScript逆向和调试中的重要手段，可以拦截和修改函数行为。以下是详细的Hook教程：

## 1. 基础Hook原理

### 什么是Hook？
Hook是通过替换原始函数来实现拦截、修改或监控函数调用的技术。

### 基本Hook模式
```javascript
// 保存原始函数
const originalFunction = targetObject.targetFunction;

// 替换为自定义函数
targetObject.targetFunction = function(...args) {
  console.log('函数被调用:', args);
  
  // 可选：修改参数
  const modifiedArgs = args.map(arg => {
    // 对参数进行处理
    return arg;
  });
  
  // 调用原始函数
  const result = originalFunction.apply(this, modifiedArgs);
  
  // 可选：修改返回值
  const modifiedResult = processResult(result);
  
  return modifiedResult;
};
```

## 2. Cookie Hook 实战

### 2.1 Hook document.cookie 的 setter
```javascript
// 方法1：使用Object.defineProperty Hook整个document.cookie
(function() {
  let originalCookie = document.cookie;
  
  Object.defineProperty(document, 'cookie', {
    get: function() {
      console.log('读取cookie:', originalCookie);
      return originalCookie;
    },
    set: function(value) {
      console.log('设置cookie:', value);
      
      // 捕获调用堆栈
      console.trace('Cookie设置堆栈:');
      
      // 特定cookie过滤
      if (value.includes('target_cookie_name')) {
        debugger; // 自动断点
        console.log('目标cookie被设置!', value);
      }
      
      // 继续执行原始逻辑
      originalCookie = value;
      return value;
    }
  });
})();
```

### 2.2 更精确的Cookie Hook
```javascript
// 方法2：Hook特定的cookie操作
function hookCookieOperations() {
  const cookieDescriptor = Object.getOwnPropertyDescriptor(Document.prototype, 'cookie');
  
  Object.defineProperty(document, 'cookie', {
    get: function() {
      const cookies = cookieDescriptor.get.call(document);
      // console.log('Cookie读取:', cookies);
      return cookies;
    },
    set: function(value) {
      // 分析cookie设置
      analyzeCookieSetting(value);
      
      // 继续设置cookie
      return cookieDescriptor.set.call(document, value);
    }
  });
  
  function analyzeCookieSetting(cookieStr) {
    const parts = cookieStr.split(';')[0].split('=');
    const name = parts[0].trim();
    const value = parts[1];
    
    console.group(`🍪 Cookie设置: ${name}`);
    console.log('值:', value);
    console.log('完整字符串:', cookieStr);
    console.trace('调用堆栈');
    console.groupEnd();
    
    // 对特定cookie进行特殊处理
    if (name === 'target_cookie') {
      debugger; // 自动断点调试
      logDetailedStack();
    }
  }
  
  function logDetailedStack() {
    const stack = new Error().stack;
    console.log('详细堆栈信息:', stack);
  }
}

// 执行Hook
hookCookieOperations();
```

## 3. 通用Hook工具函数

### 3.1 函数Hook工具
```javascript
class FunctionHook {
  constructor(target, functionName) {
    this.target = target;
    this.functionName = functionName;
    this.original = target[functionName];
    this.hooks = [];
  }
  
  // 添加前置Hook
  before(callback) {
    this.hooks.push({ type: 'before', callback });
    return this;
  }
  
  // 添加后置Hook
  after(callback) {
    this.hooks.push({ type: 'after', callback });
    return this;
  }
  
  // 应用Hook
  apply() {
    const self = this;
    
    this.target[this.functionName] = function(...args) {
      // 执行前置Hook
      self.hooks
        .filter(hook => hook.type === 'before')
        .forEach(hook => hook.callback(args, this));
      
      // 调用原始函数
      const result = self.original.apply(this, args);
      
      // 执行后置Hook
      self.hooks
        .filter(hook => hook.type === 'after')
        .forEach(hook => hook.callback(result, args, this));
      
      return result;
    };
    
    return this;
  }
  
  // 移除Hook
  remove() {
    this.target[this.functionName] = this.original;
  }
}

// 使用示例
const hook = new FunctionHook(console, 'log')
  .before((args) => {
    console.log('即将调用console.log:', args);
  })
  .after((result, args) => {
    console.log('console.log调用完成:', args);
  })
  .apply();
```

### 3.2 属性访问Hook
```javascript
function hookPropertyAccess(obj, prop, options = {}) {
  const descriptor = Object.getOwnPropertyDescriptor(obj, prop);
  
  if (!descriptor) {
    console.warn(`属性 ${prop} 不存在`);
    return;
  }
  
  const { onGet, onSet } = options;
  
  Object.defineProperty(obj, prop, {
    get: function() {
      const value = descriptor.get ? descriptor.get.call(this) : descriptor.value;
      
      if (onGet) {
        onGet(value, this);
      }
      
      return value;
    },
    set: function(newValue) {
      if (onSet) {
        onSet(newValue, this);
      }
      
      if (descriptor.set) {
        descriptor.set.call(this, newValue);
      } else {
        descriptor.value = newValue;
      }
    },
    configurable: true,
    enumerable: descriptor.enumerable
  });
}

// 使用示例
hookPropertyAccess(window, 'localStorage', {
  onSet: (value, target) => {
    console.log('localStorage被设置:', value);
    console.trace();
  },
  onGet: (value, target) => {
    console.log('localStorage被读取:', value);
  }
});
```

## 4. 高级Hook技术

### 4.1 异步函数Hook
```javascript
async function hookAsyncFunction(target, functionName) {
  const original = target[functionName];
  
  target[functionName] = async function(...args) {
    console.log(`异步函数 ${functionName} 被调用`, args);
    
    const startTime = Date.now();
    try {
      const result = await original.apply(this, args);
      const endTime = Date.now();
      
      console.log(`异步函数 ${functionName} 执行完成`, {
        参数: args,
        结果: result,
        耗时: `${endTime - startTime}ms`
      });
      
      return result;
    } catch (error) {
      console.error(`异步函数 ${functionName} 执行错误`, error);
      throw error;
    }
  };
}

// 使用示例
hookAsyncFunction(window, 'fetch');
```

### 4.2 原型链Hook
```javascript
// Hook所有XMLHttpRequest实例
function hookXMLHttpRequest() {
  const originalOpen = XMLHttpRequest.prototype.open;
  const originalSend = XMLHttpRequest.prototype.send;
  
  XMLHttpRequest.prototype.open = function(method, url, async, user, password) {
    this._method = method;
    this._url = url;
    
    console.log(`XHR Open: ${method} ${url}`);
    
    return originalOpen.apply(this, arguments);
  };
  
  XMLHttpRequest.prototype.send = function(data) {
    console.log(`XHR Send: ${this._method} ${this._url}`, data);
    
    // Hook响应
    const originalOnReadyStateChange = this.onreadystatechange;
    this.onreadystatechange = function() {
      if (this.readyState === 4) {
        console.log(`XHR Response: ${this._url}`, {
          status: this.status,
          response: this.responseText,
          headers: this.getAllResponseHeaders()
        });
      }
      
      if (originalOnReadyStateChange) {
        originalOnReadyStateChange.apply(this, arguments);
      }
    };
    
    return originalSend.apply(this, arguments);
  };
}

hookXMLHttpRequest();
```

## 5. 实用Hook场景

### 5.1 调试加密函数
```javascript
function hookCryptoFunctions() {
  // Hook CryptoJS加密函数
  if (window.CryptoJS) {
    const originalEncrypt = CryptoJS.AES.encrypt;
    
    CryptoJS.AES.encrypt = function(message, key, cfg) {
      console.group('🔐 AES加密调用');
      console.log('明文:', message);
      console.log('密钥:', key);
      console.trace('加密堆栈');
      console.groupEnd();
      
      return originalEncrypt.call(this, message, key, cfg);
    };
  }
  
  // Hook Web Crypto API
  if (window.crypto && window.crypto.subtle) {
    const originalEncrypt = crypto.subtle.encrypt;
    
    crypto.subtle.encrypt = function(algorithm, key, data) {
      console.group('🔐 Web Crypto加密调用');
      console.log('算法:', algorithm);
      console.log('密钥:', key);
      console.log('数据:', data);
      console.trace();
      console.groupEnd();
      
      return originalEncrypt.call(this, algorithm, key, data);
    };
  }
}
```

### 5.2 监控网络请求
```javascript
function hookNetworkRequests() {
  // Hook fetch
  const originalFetch = window.fetch;
  window.fetch = function(...args) {
    const [url, options] = args;
    
    console.group('🌐 Fetch请求');
    console.log('URL:', url);
    console.log('选项:', options);
    console.trace();
    console.groupEnd();
    
    return originalFetch.apply(this, args).then(response => {
      console.log('Fetch响应:', response.url, response.status);
      return response;
    });
  };
  
  // Hook WebSocket
  const originalWebSocket = window.WebSocket;
  window.WebSocket = function(url, protocols) {
    console.log('WebSocket连接:', url, protocols);
    
    const ws = new originalWebSocket(url, protocols);
    
    const originalSend = ws.send;
    ws.send = function(data) {
      console.log('WebSocket发送:', data);
      return originalSend.call(this, data);
    };
    
    ws.addEventListener('message', function(event) {
      console.log('WebSocket接收:', event.data);
    });
    
    return ws;
  };
}
```

## 6. Hook框架示例

### 6.1 完整的Hook管理器
```javascript
class HookManager {
  constructor() {
    this.hooks = new Map();
    this.enabled = true;
  }
  
  // 添加Hook
  addHook(target, property, options = {}) {
    const key = `${target.constructor.name}.${property}`;
    
    if (this.hooks.has(key)) {
      console.warn(`Hook ${key} 已存在`);
      return;
    }
    
    const descriptor = Object.getOwnPropertyDescriptor(target, property);
    if (!descriptor) {
      console.warn(`属性 ${property} 不存在`);
      return;
    }
    
    const hook = {
      target,
      property,
      original: descriptor,
      options,
      active: true
    };
    
    this.hooks.set(key, hook);
    this.applyHook(hook);
    
    return key;
  }
  
  // 应用Hook
  applyHook(hook) {
    const { target, property, original, options } = hook;
    
    Object.defineProperty(target, property, {
      get: function() {
        const value = original.get ? original.get.call(this) : original.value;
        
        if (hook.active && options.onGet) {
          options.onGet(value, this);
        }
        
        return value;
      },
      set: function(newValue) {
        if (hook.active && options.onSet) {
          options.onSet(newValue, this);
        }
        
        if (original.set) {
          original.set.call(this, newValue);
        } else {
          original.value = newValue;
        }
      },
      configurable: true,
      enumerable: original.enumerable
    });
  }
  
  // 移除Hook
  removeHook(key) {
    const hook = this.hooks.get(key);
    if (hook) {
      Object.defineProperty(hook.target, hook.property, hook.original);
      this.hooks.delete(key);
    }
  }
  
  // 启用/禁用Hook
  setHookState(key, active) {
    const hook = this.hooks.get(key);
    if (hook) {
      hook.active = active;
    }
  }
  
  // 获取所有Hook
  getHooks() {
    return Array.from(this.hooks.keys());
  }
}

// 使用示例
const hookManager = new HookManager();

// Hook cookie
hookManager.addHook(document, 'cookie', {
  onSet: (value) => {
    console.log('Cookie被设置:', value);
    if (value.includes('session')) {
      debugger;
    }
  }
});

// Hook localStorage
hookManager.addHook(window, 'localStorage', {
  onSet: (value) => {
    console.log('localStorage被设置:', value);
  }
});
```

## 7. 注意事项

### 7.1 避免无限循环
```javascript
function safeHook(target, property, hookFunction) {
  let inHook = false;
  
  const descriptor = Object.getOwnPropertyDescriptor(target, property);
  
  Object.defineProperty(target, property, {
    get: function() {
      if (inHook) return descriptor.get ? descriptor.get.call(this) : descriptor.value;
      
      inHook = true;
      const value = hookFunction('get', 
        descriptor.get ? descriptor.get.call(this) : descriptor.value, 
        this
      );
      inHook = false;
      
      return value;
    },
    set: function(newValue) {
      if (inHook) {
        if (descriptor.set) descriptor.set.call(this, newValue);
        else descriptor.value = newValue;
        return;
      }
      
      inHook = true;
      const value = hookFunction('set', newValue, this);
      inHook = false;
      
      if (descriptor.set) {
        descriptor.set.call(this, value);
      } else {
        descriptor.value = value;
      }
    },
    configurable: true
  });
}
```

### 7.2 性能考虑
- 避免在频繁调用的函数上使用复杂Hook
- 在生产环境中移除Hook代码
- 使用条件Hook来减少性能影响

这些Hook技术可以帮助你深入理解JavaScript应用的内部工作原理，特别是在逆向分析和调试复杂应用时非常有用。
