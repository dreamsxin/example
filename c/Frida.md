```c
#include <stdio.h>
#include <unistd.h>

void hello() {
    printf("Hello from hello() function!\n");
}

int add(int a, int b) {
    return a + b;
}

int main() {
    printf("Program started\n");
    
    for (int i = 0; i < 5; i++) {
        hello();
        int result = add(i, i * 10);
        printf("add(%d, %d) = %d\n", i, i*10, result);
        sleep(1);
    }
    
    printf("Program exiting\n");
    return 0;
}
```

```js
// hello_script.js 高级功能示例

// 1. 查找并挂钩所有函数
console.log("扫描所有函数:");

// 获取主模块
var mainModule = Process.enumerateModules()[0];
console.log("主模块: " + mainModule.name);

// 2. 函数调用统计
var callCount = {};
Interceptor.attach(DebugSymbol.findFunctionsNamed("hello")[0], {
    onEnter: function(args) {
        if (!callCount["hello"]) callCount["hello"] = 0;
        callCount["hello"]++;
        console.log("hello() 第 " + callCount["hello"] + " 次调用");
    }
});

// 3. 函数参数修改
var addAddr = DebugSymbol.findFunctionsNamed("add")[0];
Interceptor.attach(addAddr, {
    onEnter: function(args) {
        console.log("原始参数: " + args[0] + ", " + args[1]);
        
        // 修改参数（例如，将所有加法变为乘法）
        var originalA = args[0].toInt32();
        var originalB = args[1].toInt32();
        
        // 保存原始参数以便恢复
        this.originalA = originalA;
        this.originalB = originalB;
        
        // 修改参数（示例：参数都加100）
        args[0] = ptr(originalA + 100);
        args[1] = ptr(originalB + 100);
        console.log("修改后参数: " + (originalA+100) + ", " + (originalB+100));
    },
    onLeave: function(retval) {
        // 恢复原始参数（如果需要）
        // args[0] = ptr(this.originalA);
        // args[1] = ptr(this.originalB);
        
        console.log("原始返回值: " + retval);
        
        // 修改返回值（示例：结果乘以2）
        var originalResult = retval.toInt32();
        var newResult = originalResult * 2;
        retval.replace(ptr(newResult));
        console.log("修改后返回值: " + newResult);
    }
});

// 4. 定时打印统计信息
setInterval(function() {
    console.log("\n=== 统计信息 ===");
    for (var func in callCount) {
        console.log(func + ": " + callCount[func] + " 次调用");
    }
}, 5000);

console.log("高级挂钩已启动");
```
