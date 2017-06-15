# 安装

```shell
wget -c http://static.phpmd.org/php/latest/phpmd.phar
```

## 命令行 Command line usage

```text
Type phpmd [filename|directory] [report format] [ruleset file], i.e
```

目前 PHPMD 带有以下三个渲染器:

- xml, 把报告格式化为XML
- text, 简单文本格式
- html, 简单HTML，但可能有问题。

可以给 PHPMD 传一个文件名，或是一个代码目录。
PHPMD Phar包里面包含了规则集文件，虽然"rulesets/codesize.xml"参数看起来很像一个文件。

### 命令行选项

* 默认输出是XML格式，你可以把它重定向到一个文件中，然后对其XSLT，或者做些其他操作。
* 对于内建规则集，可以使用简短的名字，比如：phpmd ./src xml codesize
* 命令行界面也接受附加参数：
	- –minimumpriority - 规则优先级; 更低优先级的规则不会使用。
	- –reportfile - 输出报告到文件，代替默认的STDOUT。
	- –suffixes - 有效文件后缀名，以逗号分隔多个文件。
	- –exclude - 忽略的目录，以逗号分隔多个目录。
	- –strict - 同时报告那些以@SuppressWarnings注释掉的节点。（注：@SuppressWarning意为禁止告警）

可以调用PHPMD命令，其中包含了规则集名：

```shell
phpmd /path/to/source text codesize
```

可以使用逗号传递多个规则名：

```shell
phpmd /path/to/source text codesize,unusedcode,naming
```

也可以在内建规则集中混合自定义规则集：

```shell
phpmd /path/to/source text codesize,/my/rules.xml
```

这就是说，可以自定义规则集组合检查代码。

PHPMD命令行工具当前定义了三个退出码。

- 0 表示一切正常。这意味着没有错误／异常，同时在本次测试中PHPMD没有检查到任何违规代码。
- 1 表示出现错误／异常，打断了PHPMD执行过程。
- 2 这个退出码表明PHPMD执行完了测试过程，没有被错误／异常打断。但是在分析过的代码里检测到了违规代码。

## 屏蔽警告

可以使用文档注释标记，从PHPMD排除方法或类，或者屏蔽某些特定软件工件的规则。

```php
/**
 * 屏蔽类中所有PMD警告
 *
 * @SuppressWarnings(PHPMD)
 */
class Bar {
    function  foo() {
        $baz = 23;
    }
}
```

也可以只屏蔽一个规则：
```php
/**
 *
 */
class Bar {
    /**
     * 在这个方法里屏蔽UnusedLocalVariable警告
     *
     * @SuppressWarnings(PHPMD.UnusedLocalVariable)
     */
    public function foo() {
        $baz = 42;
    }
}
```

`@SuppressWarnings` 注记支持通配符匹配，所以可以在一个注记中指定屏蔽多个告警规则。
```php
/**
 * 屏蔽类中所有包含"unused"的规则
 *
 * @SuppressWarnings("unused")
 */
class Bar {
    private $unusedPrivateField = 42;
    public function foo($unusedFormalParameter = 23)
    {
        $unusedLocalVariable = 17;
    }
    private function unusedPrivateMethod() {
    }
}
```

一个文档注释中可以包含多个`@SuppressWarnings`

```php
/**
 * 屏蔽这两个规则中的所有警告。
 *
 * @SuppressWarnings(PHPMD.LongVariable)
 * @SuppressWarnings(PHPMD.UnusedLocalVariable)
 */
class Bar {
    public function foo($thisIsALongAndUnusedVariable)
    {

    }
}
```



## 规则集列表

- Clean Code Rules: 强化代码整洁度的规则集，来自SOLID原则和“对象健身操”原则。
- Code Size Rules: 代码尺寸规则集.
- Controversial Rules: 有争议的代码规则.
- Design Rules: 软件设计的相关问题规则集.
- Naming Rules: 名称太长，规则太短，等等规则集.
- Unused Code Rules: 找到未使用的代码的规则集.

Clean Code Rules

- BooleanArgumentFlag: 布尔参数是违反单一责任原则 (SRP) 的可靠指标。你可以通过提取这个标志中的逻辑到其自己的类或方法来解决此问题。
- ElseExpression: if表达式中的else分支并不是必须的。你可以重写条件，去掉else，这样代码更简单。你可能在代码中就要使用return，或者把代码分成几个小的方法。对于更为简单的情况，可以直接用三元表达式。
- StaticAccess: 静态访问(Static acccess)导致对其他类无法改变的依赖关系，而且难以测试代码。应该想方设法避免静态访问。你可以通过构造器注入依赖的方式，代替静态访问。只有在使用工厂方法的时候，静态访问才是合适的。

Code Size Rules

- CyclomaticComplexity:复杂性是由方法内关键点的个数加1来确定的。关键点有’if’，‘while’，'for’和 ‘case labels’。一般来说，1-4复杂度低，5-7是适度的复杂性，8-10是高度复杂的，而11+有很高的复杂性。
- NPathComplexity:一个方法的NPath复杂性是通过其中非循环执行路径的数量来确定的。200被认为是必须降低复杂性的阀值。
- ExcessiveMethodLength: 违反此规则通常表明该方法做的太多了。通过创建辅助方法和消除任何复制/粘贴代码的形式尽量减小方法体。
- ExcessiveClassLength:过长的类文件表名它试图做的操作太多。可以通过拆分和减小类所管理的对象来重组它。
- ExcessiveParameterList:长参数列表表明一个新的对象需要被创建以包容众多参数。基本上，可以尝试分组参数。
- ExcessivePublicCount:类中有大量的公共方法和属性声明表明这个类需要阻断其增长，并进行彻底的测试。
- TooManyFields: 有太多字段的类可以考虑重新设计（可能是将一些信息打包为嵌套对象的方法）来减少字段。比如，类中的city/state/zip字段可以合并成一个Address字段。
- TooManyMethods: 一个类有太多的方法，将其重构是最好的选择，以减少其复杂性和找到更细粒度的对象。
- ExcessiveClassComplexity:确定一个类进行修改和维护需要多少时间和精力时，加权方法计数（WMC：The Weighted Method Count）是一个很好的指标。WMC定义为在类中声明的所有方法复杂度的总和。大量的方法也意味着这个类对其派生类有更大的潜在影响。

Controversial Rules

- Superglobals:直接访问超级全局变量是一个很差的做法。举个例子，这些变量应该被封装在由框架提供的对象里。
- CamelCaseClassName:最好使用CamelCase标记法命名类。
- CamelCasePropertyName: 最好使用CamelCase标记法命名属性。
- CamelCaseMethodName:最好使用CamelCase标记法命名方法。
- CamelCaseParameterName:最好使用CamelCase标记法命名参数。
- CamelCaseVariableName:最好使用CamelCase标记法命名变量。

Design Rules

- ExitExpression: 正常代码里存在exit-expression就会变得不可测试，所以要避免。可以将exit-expression转化为某种类型的启动脚本，其中错误/异常代码被返回到调用环境。
- EvalExpression: eval-expression是不可测试的，有安全风险，而且是一种坏实践。我们要避免它。应该用符合常规的代码替代它。
- GotoStatement:Goto使代码难以阅读，无法了解应用程序的控制流结构。因此必须避免。可以用常规的控制结构和分离的方法/函数代替。
- NumberOfChildren:一个类拥有过多的子类，是不平衡的类层次结构的一个指标。此时应该对这个类的层次结构进行重构。
- DepthOfInheritance:一个类的继承深度过多，是不平衡的类层次结构的一个指标。此时应该对这类层次结构进行重构。
- CouplingBetweenObjects:一个类存在过多的外部依赖会对其产生几个质量方面的负面影响：如稳定性、可维护性、可理解性。

Naming Rules

- ShortVariable:字段、局部变量或参数定义名称过短。
- LongVariable:字段、局部变量定义名称过长。
- ShortMethodName: 方法名过短。
- ConstructorWithNameAsEnclosingClass:构造函数方法不应该和封装的类有相同的名称，应参照PHP 5 __construct方法来命名。
- ConstantNamingConventions:类/接口常量必须用大写字母定义。
- BooleanGetMethodName:返回值为布尔型而以’getX()'格式命名的方法。习惯上此类方法的命名格式是 ‘isX()’ 或者 ‘hasX()’。

Unused Code Rules

- UnusedPrivateField:存在已声明或赋值的私有字段，但是却未使用。
- UnusedLocalVariable: 存在已声明或赋值的局部变量，但是却未使用。
- UnusedPrivateMethod:存在已声明但未使用的私有方法。
- UnusedFormalParameter: 避免将参数传递给方法或构造函数，然后不使用这些参数。

Remark

This document is based on a ruleset xml-file, that was taken from the original source of the PMD project. This means that most parts of the content on this page are the intellectual work of the PMD community and its contributors and not of the PHPMD project.
译注

* SOLID原则：面向对象5个基本原则
	- SRP		The Single Responsibility Principle	单一责任原则
	- OCP		The Open Closed Principle			开放封闭原则
	- LSP		The Liskov Substitution Principle	里氏替换原则
	- DIP		The Dependency Inversion Principle	依赖倒置原则
	- ISP		The Interface Segregation Principle	接口分离原则

* 对象健身操：即object calisthenics，由ThoughtWorks提出的一种设计规则集

	- 规则1：方法只使用一级缩进
		> 动机 ：庞大的方法往往缺少内聚性，一个常见的原则是将方法控制着5 行以内（不包括入参检查）。你可以尝试让每个方法只做一件事情——每个方法只保护一个控制结构或一个代码块。 
	- 规则2：拒绝 else 关键字
		> 动机 ：复杂的条件判决难于理解，而且易产生重复代码。 
	- 规则3：封装所有的原生类型和字符串
		> 动机 ：整数只是代表一个数量，没有实际的含义。用类来表示原生类型，比如年，月，日等。这样可以进行严格的类型检查。 
	- 规则4：一行代码只有一个“ . ”（或者“ -> ”）运算符
		> 动机 ：如果一行中含有多个“. ”，则说明很多职责的位置放置错了。尝试让对象为你做一些事情，而不要窥视对象内部的细节。封装的含义就是，不要让类的边界跨入到它不应该知道的类型中。 
	- 规则5 ：不要使用缩写
		> 动机 ：缩写的原因：1 ）一遍一遍的敲打同样的单词，则可能是因为某个方法调用过于频繁——要消除一下重复。 2 ）方法名字太长——可能是职责没有放在应该在的位置或者有确实的类。 
	- 规则6：保持对象实体简单清晰
		> 动机 ：超过50 行的类所做的事情通常都不止一个。这会导致他们难以理解和重用。这意味着每个类的长度不能超过 50 行。每个包所包含的文件不超过 10 个。
	- 规则7 ：任何人类中的实例变量不要超过两个
		> 动机 ：将一个对象从拥有大量属性的状态，解构成分层次的，相互关联的多个对象，会直接产生一个更实用的对象模型。将一个复杂的大的对象，分解成多个简单的对象。
	- 规则8：使用一类的集合
	- 规则9：不要使用任何的 Geter/Setter/Proerty
		> 动机：Geter/Setter/Proerty会对对象的封装性找出破坏。


## 编写规则

PHPMD 的所有规则一定是至少实现了`\PHPMD\Rule`接口。
也可以扩展抽象基类`\PHPMD\AbstractRule`，这个类已经提供了所有必需的基础设施的方法和应用逻辑的实现，所以你的唯一任务就是为规则编写具体的验证代码。
PHPMD规则接口声明了`apply()`方法，在代码分析阶段会被应用调用到，从而执行验证代码。
```php
require_once 'PHPMD/AbstractRule.php';

class Com_Example_Rule_NoFunctions extends \PHPMD\AbstractRule
{
    public function apply(\PHPMD\AbstractNode $node)
    {
        // Check constraints against the given node instance
    }
}
```

`apply()`方法的参数是`\PHPMD\AbstractNode`实例。
PHPMD 使用所谓的标记接口。这些接口的唯一目的是给规则类打标签，告知 PHPMD 哪个才是我们规则感兴趣的。下面的列表显示可用的标记接口：

- \PHPMD\Rule\ClassAware
- \PHPMD\Rule\FunctionAware
- \PHPMD\Rule\InterfaceAware
- \PHPMD\Rule\MethodAware

有了这个标记接口，我们就可以扩展上一个例子，这样，该规则将在分析被测代码中发现函数时调用到。

```php
class Com_Example_Rule_NoFunctions
       extends \PHPMD\AbstractRule
    implements \PHPMD\Rule\FunctionAware
{
    public function apply(\PHPMD\AbstractNode $node)
    {
        // Check constraints against the given node instance
		// 如果禁止使用函数，通过addViolation() 方法将其报告出来。
		$this->addViolation($node);
    }
}
```

现在只需要在规则集中加入配置入口。规则配置中最重要的元素是：

- @name: 易读规则名。
- @message: 错误/违规信息。
- @class: 规则的全称类名。
- priority: 规则优先权，值可以为1-5，1是最高优先权，5最低。

```xml
<ruleset name="example.com rules"
       xmlns="http://pmd.sf.net/ruleset/1.0.0"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0 http://pmd.sf.net/ruleset_xml_schema.xsd"
       xsi:noNamespaceSchemaLocation="http://pmd.sf.net/ruleset_xml_schema.xsd">

    <rule name="FunctionRule"
          message = "Please do not use functions."
          class="Com_Example_Rule_NoFunctions"
          externalInfoUrl="http://example.com/phpmd/rules.html#functionrule">

        <priority>1</priority>
    </rule>
</ruleset>
```
假设我们的Com/Example/Rule/NoFunction.php是一个规则类，example-rule.xml是一个规则集。可以从命令行中测试：

```shell
phpmd /my/source/example.com text /my/rules/example-rule.xml
```

### 根据现有的软件度量编写规则

开发PHPMD的根本目标是为PHP_Depend实现一个简单友好的接口。本节你将会看到，如何开发一个规则类，并以输入数据的形式使用PDepend作为软件度量。

你会看到如何访问一个给定的`\PHPMD\AbstractNode`实例软件计量，以及如何使用phpmd配置后端，阈值等设置可定制而不改变任何PHP代码。此外，您将看到怎样改进错误信息的内容。

现在新规则需要一个软件度量标准作基础。完整并及时更新的可用软件计量标准列表可以从PHP_Depend的计量类目获得。在这里，我们选公共方法个数(npm:Number of Public Methods)标准，然后定义规则的上下限。我们设置上限10，如果存在更多的公共方法会暴露更多的类的内部信息，那么它应该被分成几个类了。下限可以设为1，因为如果类没有公有方法，那它就不会为周边应用提供任何服务。

你可以看到，它实现了`\PHPMD\Rule\ClassAware`接口，所以PHPMD知道这个规则只能被类调用。

```php
class Com_Example_Rule_NumberOfPublicMethods
       extends \PHPMD\AbstractRule
    implements \PHPMD\Rule\ClassAware
{
    const MINIMUM = 1,
          MAXIMUM = 10;

    public function apply(\PHPMD\AbstractNode $node)
    {
        // Check constraints against the given node instance
    }
}
```

接下来使用nmp规则对象，它和节点实例是关联的，所有节点计算的规则对象都可以使用节点实例的getMetric()方法直接访问。getMetric()接受一个参数，即规则的缩写。这些缩写可以从PHP_Depends规则分类中找到。

```php
class Com_Example_Rule_NumberOfPublicMethods
       extends \PHPMD\AbstractRule
    implements \PHPMD\Rule\ClassAware
{
    const MINIMUM = 1,
          MAXIMUM = 10;

    public function apply(\PHPMD\AbstractNode $node)
    {
        $npm = $node->getMetric('npm');
        if ($npm < self::MINIMUM || $npm > self::MAXIMUM) {
            $this->addViolation($node);
        }
    }
}
```

以上就是规则的代码部分。现在我们把它加到规则集文件中。

```xml
<ruleset name="example.com rules"
       xmlns="http://pmd.sf.net/ruleset/1.0.0"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0 http://pmd.sf.net/ruleset_xml_schema.xsd"
       xsi:noNamespaceSchemaLocation="http://pmd.sf.net/ruleset_xml_schema.xsd">

    <!-- ... -->

    <rule name="NumberOfPublics"
          message = "The context class violates the NPM metric."
          class="Com_Example_Rule_NumberOfPublicMethods"
          externalInfoUrl="http://example.com/phpmd/rules.html#numberofpublics">

        <priority>3</priority>
    </rule>
</ruleset>
```

现在运行PHPMD，它会报告不满足NPM规则的所有类。但正如我们所承诺的，我们将使这个规则更加可定制，以便可以调整它适应不同的项目要求。我们将MINIMUM和MAXIMUM替换为在规则集文件中可以配置的属性。

```xml
<ruleset name="example.com rules"
       xmlns="http://pmd.sf.net/ruleset/1.0.0"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0 http://pmd.sf.net/ruleset_xml_schema.xsd"
       xsi:noNamespaceSchemaLocation="http://pmd.sf.net/ruleset_xml_schema.xsd">

    <!-- ... -->

    <rule name="NumberOfPublics"
          message = "The context class violates the NPM metric."
          class="Com_Example_Rule_NumberOfPublicMethods"
          externalInfoUrl="http://example.com/phpmd/rules.html#numberofpublics">

        <priority>3</priority>
        <properties>
            <property name="minimum"
                      value="1"
                      description="Minimum number of public methods." />
            <property name="maximum"
                      value="10"
                      description="Maximum number of public methods." />
        </properties>
    </rule>
</ruleset>
```

PMD规则集文件中你可以随便定义几个属性，随你喜欢。它们会被PHPMD运行时环境注入到一个规则实例中，然后可以通过getProperty()方法访问。当前PHPMD支持以下几个getter方法。

- getBooleanProperty
- getIntProperty

现在我们来修改规则类，用可配置属性替换硬编码常量。
```php
class Com_Example_Rule_NumberOfPublicMethods
       extends \PHPMD\AbstractRule
    implements \PHPMD\Rule\ClassAware
{
    public function apply(\PHPMD\AbstractNode $node)
    {
        $npm = $node->getMetric('npm');
        if ($npm < $this->getIntProperty('minimum') ||
            $npm > $this->getIntProperty('maximum')
        ) {
            $this->addViolation($node);
        }
    }
}
```

### 自定义消息

现在我们差不多完成了，还有一个小小细节。我们执行这个规则，用户将收到消息“The context class violates the NPM metric.“这个消息不是真的有用，因为它必须手动检查上下阈值与实际阈值。可以用PHPMD最简模版/占位符引擎得到更详细的信息，你可以定义（违规信息）占位符，这会被替换为真实值。占位符的格式是`{\d}`。

```xml
<ruleset name="example.com rules"
       xmlns="http://pmd.sf.net/ruleset/1.0.0"
       xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
       xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0 http://pmd.sf.net/ruleset_xml_schema.xsd"
       xsi:noNamespaceSchemaLocation="http://pmd.sf.net/ruleset_xml_schema.xsd">

    <!-- ... -->

    <rule name="NumberOfPublics"
          message = "The class {0} has {1} public method, the threshold is {2}."
          class="Com_Example_Rule_NumberOfPublicMethods"
          externalInfoUrl="http://example.com/phpmd/rules.html#numberofpublics">

        <priority>3</priority>
        <properties>
            <property name="minimum" value="1"
                      description="Minimum number of public methods." />
            <property name="maximum" value="10"
                      description="Maximum number of public methods." />
        </properties>
    </rule>
</ruleset>
```

现在我们可以以这种方式调整规则类，自动填充占位符 {0}, {1}, {2}

```php
class Com_Example_Rule_NumberOfPublicMethods
       extends \PHPMD\AbstractRule
    implements \PHPMD\Rule\ClassAware
{
    public function apply(\PHPMD\AbstractNode $node)
    {
        $min = $this->getIntProperty('minimum');
        $max = $this->getIntProperty('maximum');
        $npm = $node->getMetric('npm');

        if ($npm < $min) {
            $this->addViolation($node, array(get_class($node), $npm, $min));
        } else if ($npm > $max) {
            $this->addViolation($node, array(get_class($node), $npm, $max));
        }
    }
}
```

### 编写基于抽象语法树的规则

现在我们将学习如何开发一个PHPMD规则，利用PHP_Depend的抽象语法树检测被分析源代码的违规或可能的错误。语法树可以通过`\PHPMD\AbstractNode`类的`getFirstChildOfType`和`findChildrenOfType`方法访问。

在这个例子中我们将实现一个规则，用来检测新但有争议的PHP新功能，goto。因为我们都知道并同意Basic中的goto非常糟糕，我们想阻止我们的开发者使用坏的特征。因此，我们实现了一个PHPMD规则，通过`PHP_Depend`搜索goto语言构造。

goto语句不会出现在类和对象中，但会在方法和函数里出现。新规则类必需实现两个标记接口，`\PHPMD\Rule\FunctionAware` 和 `\PHPMD\Rule\MethodAware`。
```php
namespace PHPMD\Rule\Design;

use PHPMD\AbstractNode;
use PHPMD\AbstractRule;
use PHPMD\Rule\MethodAware;
use PHPMD\Rule\FunctionAware;

class GotoStatement extends AbstractRule implements MethodAware, FunctionAware
{
    public function apply(AbstractNode $node)
    {
        foreach ($node->findChildrenOfType('GotoStatement') as $goto) {
            $this->addViolation($goto, array($node->getType(), $node->getName()));
        }
    }
}
```

如你所见，我们在上面例子中查询字符串`GotoStatement`。PHPMD使用这个快捷注记定位具体`PHP_Depend`语法树节点。所有PDepend抽象语法树类都像这样：`\PDepend\Source\AST\ASTGotoStatement`，其中`\PDepend\Source\AST\AST`是固定的，其他部分取决于节点类型。固定的这部分搜索抽象语法书节点时可以省略。你应该看看PHP_Depend’s代码包，可以找到所有当前支持的代码节点，以便实现附加规则。

## 如何创建自定义规则集

如果只想挑选PHPMD自带的部分规则，或者修改预定义阈值，你可以创建自定义规则集文件来注明(你自己的)自定义规则集合。
以一个空ruleset.xml开始

开始一个新规则集最简单的方式是拷贝一个现有文件然后从文件体中去掉所有规则标签。或者你可以使用模板作为规则集文件。记得修改@name和<description />的内容。

```xml
<?xml version="1.0"?>
<ruleset name="My first PHPMD rule set"
         xmlns="http://pmd.sf.net/ruleset/1.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0
                     http://pmd.sf.net/ruleset_xml_schema.xsd"
         xsi:noNamespaceSchemaLocation="
                     http://pmd.sf.net/ruleset_xml_schema.xsd">
    <description>
        My custom rule set that checks my code...
    </description>
</ruleset>
```

### 添加规则引用到新ruleset.xml

首先是要在新规则集文件中加入整个unused code规则。<rule />元素可以引用PHPMD内建的这个规则集。

```xml
<?xml version="1.0"?>
<ruleset name="My first PHPMD rule set"
         xmlns="http://pmd.sf.net/ruleset/1.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0
                     http://pmd.sf.net/ruleset_xml_schema.xsd"
         xsi:noNamespaceSchemaLocation="
                     http://pmd.sf.net/ruleset_xml_schema.xsd">
    <description>
        My custom rule set that checks my code...
    </description>

    <!-- Import the entire unused code rule set -->
    <rule ref="rulesets/unusedcode.xml" />
</ruleset>
```

很好。自定义规则集适用于分析源代码的整个unused code代码规则。
我们也经常在自定义规则集中使用codesize规则集的cyclomatic complexity规则。可以使用<rule />元素@ref属性达到目的。

```xml
<?xml version="1.0"?>
<ruleset name="My first PHPMD rule set"
         xmlns="http://pmd.sf.net/ruleset/1.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0
                     http://pmd.sf.net/ruleset_xml_schema.xsd"
         xsi:noNamespaceSchemaLocation="
                     http://pmd.sf.net/ruleset_xml_schema.xsd">
    <description>
        My custom rule set that checks my code...
    </description>

    <!-- Import the entire unused code rule set -->
    <rule ref="rulesets/unusedcode.xml" />

    <!-- Import the entire cyclomatic complexity rule -->
    <rule ref="rulesets/codesize.xml/CyclomaticComplexity" />
</ruleset>
```

现在，我们想改变cyclomatic complexity规则的某些属性。我们先把规则的优先权改成最高值1，然后降低报告冲突时的阈值。使用的xml元素可以从原始的规则集文件找。

```xml
<?xml version="1.0"?>
<ruleset name="My first PHPMD rule set"
         xmlns="http://pmd.sf.net/ruleset/1.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0
                     http://pmd.sf.net/ruleset_xml_schema.xsd"
         xsi:noNamespaceSchemaLocation="
                     http://pmd.sf.net/ruleset_xml_schema.xsd">
    <description>
        My custom rule set that checks my code...
    </description>

    <!-- Import the entire unused code rule set -->
    <rule ref="rulesets/unusedcode.xml" />

    <!--
        Import the entire cyclomatic complexity rule and
        customize the rule configuration.
    -->
    <rule ref="rulesets/codesize.xml/CyclomaticComplexity">
        <priority>1</priority>
        <properties>
            <property name="reportLevel" value="5" />
        </properties>
    </rule>
</ruleset>
```

PHPMD附加处理自定义设置。这就是说PHPMD保留所有没有自定义设置的原始配置。

### 排除规则

最后我们要重用PHPMD的naming规则集。但是我们必须排除两个经常变化的naming规则。通过在规则引用中声明<exclude />元素可以实现排除。这个元素有一个属性@name，它指定了排除规则的名字。

```xml
<?xml version="1.0"?>
<ruleset name="My first PHPMD rule set"
         xmlns="http://pmd.sf.net/ruleset/1.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://pmd.sf.net/ruleset/1.0.0
                     http://pmd.sf.net/ruleset_xml_schema.xsd"
         xsi:noNamespaceSchemaLocation="
                     http://pmd.sf.net/ruleset_xml_schema.xsd">
    <description>
        My custom rule set that checks my code...
    </description>

    <!-- Import the entire unused code rule set -->
    <rule ref="rulesets/unusedcode.xml" />

    <!--
        Import the entire cyclomatic complexity rule and
        customize the rule configuration.
    -->
    <rule ref="rulesets/codesize.xml/CyclomaticComplexity">
        <priority>1</priority>
        <properties>
            <property name="reportLevel" value="5" />
        </properties>
    </rule>

    <!-- Import entire naming rule set and exclude rules -->
    <rule ref="rulesets/naming.xml">
        <exclude name="ShortVariable" />
        <exclude name="LongVariable" />
    </rule>
</ruleset>
```