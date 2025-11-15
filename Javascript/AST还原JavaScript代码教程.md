# AST还原JavaScript代码教程

- https://github.com/sxzz/ast-explorer

AST（抽象语法树）还原是将混淆或压缩的JavaScript代码还原为可读性更高的形式的过程。以下是详细的教程：

## 1. 理解AST基础

### 什么是AST？
AST是源代码的抽象语法结构的树状表示，每个节点代表源代码中的一种结构。

### 常用工具库
- **Babel Parser** - 将JS代码解析为AST
- **Babel Traverse** - 遍历和修改AST
- **Babel Generator** - 将AST生成回JS代码
- **Babel Types** - 创建和验证AST节点

## 2. 基本还原流程

### 安装依赖
```bash
npm install @babel/parser @babel/traverse @babel/generator @babel/types
```

### 基础代码结构
```javascript
const parser = require('@babel/parser');
const traverse = require('@babel/traverse').default;
const generate = require('@babel/generator').default;
const t = require('@babel/types');

function deobfuscate(code) {
  // 1. 解析为AST
  const ast = parser.parse(code);
  
  // 2. 遍历和修改AST
  traverse(ast, {
    // 各种visitor函数
  });
  
  // 3. 生成还原后的代码
  return generate(ast).code;
}
```

## 3. 常见还原技术

### 3.1 字符串解密
```javascript
traverse(ast, {
  CallExpression(path) {
    // 处理字符串加密函数调用
    if (t.isIdentifier(path.node.callee, { name: 'decryptFunc' })) {
      const args = path.node.arguments;
      // 计算字符串真实值
      const decryptedValue = decryptString(args[0].value);
      path.replaceWith(t.stringLiteral(decryptedValue));
    }
  }
});
```

### 3.2 控制流扁平化还原
```javascript
traverse(ast, {
  SwitchStatement(path) {
    const switchNode = path.node;
    const discriminant = switchNode.discriminant.name;
    const cases = switchNode.cases;
    
    // 查找控制变量
    let controlVar;
    path.getStatementParent().traverse({
      VariableDeclarator(varPath) {
        if (varPath.node.id.name === discriminant) {
          controlVar = varPath;
        }
      }
    });
    
    // 重构控制流
    const newStatements = reconstructControlFlow(cases, controlVar);
    path.replaceWithMultiple(newStatements);
  }
});
```

### 3.3 常量传播
```javascript
traverse(ast, {
  VariableDeclarator(path) {
    if (t.isLiteral(path.node.init)) {
      const varName = path.node.id.name;
      const value = path.node.init.value;
      
      // 替换所有对该变量的引用
      path.scope.bindings[varName].referencePaths.forEach(refPath => {
        refPath.replaceWith(t.valueToNode(value));
      });
      
      // 移除变量声明
      if (path.parent.declarations.length === 1) {
        path.parentPath.remove();
      } else {
        path.remove();
      }
    }
  }
});
```

### 3.4 死代码消除
```javascript
traverse(ast, {
  IfStatement(path) {
    const test = path.node.test;
    
    // 处理永远为true的条件
    if (t.isBooleanLiteral(test) && test.value === true) {
      path.replaceWithMultiple(path.node.consequent.body);
    }
    // 处理永远为false的条件
    else if (t.isBooleanLiteral(test) && test.value === false) {
      if (path.node.alternate) {
        path.replaceWithMultiple(path.node.alternate.body);
      } else {
        path.remove();
      }
    }
  }
});
```

## 4. 完整示例

### 还原混淆代码
```javascript
const deobfuscateAdvanced = (code) => {
  const ast = parser.parse(code);
  
  // 多轮遍历确保完全还原
  for (let i = 0; i < 3; i++) {
    traverse(ast, {
      // 字符串解密
      CallExpression: stringDecryptionVisitor,
      // 控制流还原
      SwitchStatement: controlFlowVisitor,
      // 常量折叠
      BinaryExpression: constantFoldingVisitor,
      // 死代码消除
      IfStatement: deadCodeVisitor
    });
  }
  
  return generate(ast, { concise: false }).code;
};

// 具体的visitor实现
const stringDecryptionVisitor = {
  CallExpression(path) {
    // 实现字符串解密逻辑
    if (isDecryptionCall(path.node)) {
      const decrypted = evaluateDecryption(path.node);
      path.replaceWith(t.stringLiteral(decrypted));
    }
  }
};
```

## 5. 高级技巧

### 符号执行
```javascript
class SymbolicExecutor {
  constructor() {
    this.symbolTable = new Map();
  }
  
  evaluateExpression(node) {
    if (t.isLiteral(node)) {
      return node.value;
    }
    if (t.isBinaryExpression(node)) {
      const left = this.evaluateExpression(node.left);
      const right = this.evaluateExpression(node.right);
      return this.applyOperator(node.operator, left, right);
    }
    // 处理其他表达式类型
  }
  
  applyOperator(operator, left, right) {
    switch(operator) {
      case '+': return left + right;
      case '-': return left - right;
      // 其他操作符...
    }
  }
}
```

### 作用域分析
```javascript
function analyzeScopes(ast) {
  const scopes = new Map();
  
  traverse(ast, {
    enter(path) {
      if (path.isFunction() || path.isProgram()) {
        const scope = path.scope;
        scopes.set(path.node, {
          bindings: new Map(Object.entries(scope.bindings)),
          references: scope.references
        });
      }
    }
  });
  
  return scopes;
}
```

## 6. 实用工具函数

```javascript
// 检查节点是否可静态求值
function isStaticallyEvaluable(node) {
  try {
    const result = evaluateNode(node);
    return result !== undefined;
  } catch {
    return false;
  }
}

// 静态求值节点
function evaluateNode(node) {
  if (t.isLiteral(node)) return node.value;
  if (t.isBinaryExpression(node)) {
    const left = evaluateNode(node.left);
    const right = evaluateNode(node.right);
    switch(node.operator) {
      case '+': return left + right;
      case '-': return left - right;
      case '*': return left * right;
      case '/': return left / right;
    }
  }
  throw new Error('Cannot evaluate node');
}
```

## 7. 注意事项

1. **多次遍历**：某些还原需要多次遍历AST
2. **依赖顺序**：某些转换有依赖关系，需要注意执行顺序
3. **错误处理**：添加适当的错误处理机制
4. **性能考虑**：对于大型文件，注意性能优化

## 8. 完整工具示例

```javascript
class JSDeobfuscator {
  constructor() {
    this.visitors = [
      this.constantPropagation.bind(this),
      this.stringDecryption.bind(this),
      this.controlFlowDeobfuscation.bind(this),
      this.deadCodeElimination.bind(this)
    ];
  }
  
  deobfuscate(code) {
    let ast = parser.parse(code);
    
    // 应用所有visitor
    this.visitors.forEach(visitor => {
      traverse(ast, visitor);
    });
    
    return generate(ast, {
      comments: true,
      concise: false
    }).code;
  }
  
  constantPropagation(path) {
    // 实现常量传播
  }
  
  stringDecryption(path) {
    // 实现字符串解密
  }
  
  // ... 其他方法
}
```

这个教程涵盖了AST还原的主要技术点。实际应用中需要根据具体的混淆技术进行调整和扩展。
