#PSR-2: 编码风格指南
本指南内容的延伸与扩展均基于PSR-1基本编码准则。

本指南的目的是为了减少在不同作者之间分享PHP代码时的接受难度，本文通过列举一系列格式化PHP代码时的常用规范来达到此目的。


# 1、概述

* 代码必须遵循“编码风格指南”PSR-1规范；
* 代码必须使用4个空格来进行缩进，而不是tab键（制表符）；
* 对于一行代码的长度不应该有硬限制；软限制必须为120个字符，每行代码建议不超过80个字符或者更少。
* 在命名空间声明语句 `namespace` 的下面必须有一行空行，并且在 `use` 声明语句块的下面也必须有一行空行；
* 类声明的开始花括号必须放到类名下单独自成一行，类声明的结束花括号必须放到类主体的下面单独自成一行。
* 方法声明的开始花括号必须放到方法名下面单独自成一行，方法声明的结束花括号必须放到方法主体的下面单独自成一行。
* 所有的属性和方法必须有可见性声明； `abstract` 和 `final` 声明必须在可见性声明之前进行声明； `static` 声明必须放在可见性声明之后。
* 控制结构关键字的后面必须有一个空格；方法和函数的声明后面不可以有空格；
* 控制结构的开始花括号必须与其放在同一行，结束花括号必须放在该控制结构主体代码的下面单独自成一行；
* 控制结构中的开始圆括号之后不能含有空格，结束圆括号之前也不能含有空格。

## 1.1、示例
下面的代码示例简单展示了上文中提到的一些编码规范：

```php
<?php
namespace Vendor\Package;

use FooInterface;
use BarClass as Bar;
use OtherVendor\OtherPackage\BazClass;

class Foo extends Bar implements FooInterface
{
    public function sampleFunction($a, $b = null)
    {
        if ($a === $b) {
            bar();
        } elseif ($a > $b) {
            $foo->bar($arg1);
        } else {
            BazClass::bar($arg2, $arg3);
        }
    }

    final public static function bar()
    {
        // method body
    }
}
```

# 2、常规代码风格规范
## 2.1、基础代码准则

代码必须遵循PSR-1列出的所有准则。

## 2.2、文件

所有PHP文件必须使用Unix LF（换行符）作为行结束符。

所有PHP文件必须以一行空行作为结束。

只含有PHP代码的文件必须省略文件的关闭标签 `?>`。

## 2.3、行

行长度不能有硬限制。

行长度的软限制必须是120个字符；对于软限制，代码风格检查器必须警告但不能抛出错误。

一行代码的长度不建议超过80个字符；较长的行建议拆分成多个不超过80个字符的子行。

在非空行后面不可有空格。

空行可以用来增强可读性并且区分相关代码块。

一行不可以超过一个语句。

## 2.4、缩进

代码必须使用4个空格来进行缩进，而不是tab键（制表符）；

>注意：在代码中只使用空格，且不可以与制表符混用，这样做会对避免代码差异，补丁，历史和注解中的一些问题起到帮助作用。空格的使用还可以通过调整细微的缩进来改进行间对齐变得更加简单。

## 2.5、关键字 和 True/False/Null

PHP关键字必须使用小写字符。

PHP常量 `true`，`false` 和 `null` 必须使用小写字母。

# 3、Namespace 和 Use 声明

命名空间 `namespace` 的声明后面必须有一行空行。

所有的 `use` 声明必须放在命名空间 `namespace` 声明的下面。

每个声明中，必须有一个导入 `use` 关键字。

在 `use` 声明代码块后面必须有一行空行。

示例：

```php
<?php
namespace Vendor\Package;

use FooClass;
use BarClass as Bar;
use OtherVendor\OtherPackage\BazClass;

// ... additional PHP code ...
```

# 4、类，属性，和方法

## 4.1、Extends 和 Implements 关键字

一个类的 `extends` 和 `implements` 关键字必须和类名放在同一行进行声明。

类主体的左花括号必须放在下面自成一行； 右花括号必须放在类主体的后面自成一行。

```php
<?php
namespace Vendor\Package;

use FooClass;
use BarClass as Bar;
use OtherVendor\OtherPackage\BazClass;

class ClassName extends ParentClass implements \ArrayAccess, \Countable
{
    // constants, properties, methods
}
```

`implements` 列表可以被拆分为多个缩进了一次的子行。如果要拆成多个子行，列表的第一项必须要放在下一行，并且每行必须只有一个接口。

```php
<?php
namespace Vendor\Package;

use FooClass;
use BarClass as Bar;
use OtherVendor\OtherPackage\BazClass;

class ClassName extends ParentClass implements
    \ArrayAccess,
    \Countable,
    \Serializable
{
    // constants, properties, methods
}
```

## 4.2、Extends 和 Implements 关键字

所有属性都必须声明其可见性。

`var` 关键字不可用来声明一个属性。

每个声明语句不得声明超过一个属性。

属性名不应该使用单个下划线作为其前缀来表明其保护或私有的可见性。

一个属性声明看起来应该像下面这样。

```php
<?php
namespace Vendor\Package;

class ClassName
{
    public $foo = null;
}
```

## 4.3、方法

所有的方法都必须声明其可见性。

方法名不应该使用单个下划线作为其前缀来表明其保护或私有的可见性。

方法名在其声明后面不可有空格跟随。 其左花括号必须放在下面自成一行，且右花括号必须放在方法主体的下面自成一行。 左括号后面不可有空格，且右括号前面也不可有空格。

一个方法声明看来应该像下面这样。 注意括号，逗号，空格和花括号的位置：

```php
<?php
namespace Vendor\Package;

class ClassName
{
    public function fooBarBaz($arg1, &$arg2, $arg3 = [])
    {
        // method body
    }
}
```

## 4.4、方法的参数

在参数列表中，逗号之前不可有空格，而逗号之后则必须要有一个空格。

方法中有默认值的参数必须放在参数列表的最后面。

```php
<?php
namespace Vendor\Package;

class ClassName
{
    public function foo($arg1, &$arg2, $arg3 = [])
    {
        // method body
    }
}
```

参数列表可以被拆分为多个缩进了一次的子行。 如果要拆分成多个子行， 参数列表的第一项必须放在下一行， 并且每行必须只有一个参数。

当参数列表被拆分成多个子行，右括号和左花括号之间必须有一个空格并且自成一行。

```php
<?php
namespace Vendor\Package;

class ClassName
{
    public function aVeryLongMethodName(
        ClassTypeHint $arg1,
        &$arg2,
        array $arg3 = []
    ) {
        // method body
    }
}
```
## 4.5、abstract, final, 和 static 关键字

当用到 `abstract` 和 `final` 来声明时，它们必须放在可见性声明的前面。

而当用到静态 `static` 来声明时，则必须放在可见性声明的后面。

```php
<?php
namespace Vendor\Package;

abstract class ClassName
{
    protected static $foo;

    abstract protected function zim();

    final public static function bar()
    {
        // method body
    }
}
```

## 4.6、调用方法和函数

调用一个方法或函数时，在方法名或者函数名和左括号之间不可有空格， 左括号之后不可有空格，右括号之前也不可有空格。 参数列表中，逗号之前不可有空格，逗号之后则必须有一个空格。

```php
<?php
bar();
$foo->bar($arg1);
Foo::bar($arg2, $arg3);
```

参数列表可以被拆分成多个缩进了一次的子行。 如果拆分成子行，列表中的第一项必须放在下一行， 并且每一行必须只能有一个参数。

```php
<?php
$foo->bar(
    $longArgument,
    $longerArgument,
    $muchLongerArgument
);
```

# 5、控制结构

下面是对于控制结构代码风格的概括：

* 控制结构的关键词之后必须有一个空格；
* 控制结构的左括号之后不可有空格；
* 控制结构的右括号之前不可有空格；
* 控制结构的右括号和左花括号之间必须有一个空格；
* 控制结构的代码主体必须进行一次缩进；
* 控制结构的右花括号必须主体的下一行；

每个控制结构的代码主体必须被括在花括号里。 这样可是使代码看上去更加标准化， 并且加入新代码的时候还可以因此而减少引入错误的可能性。

## 5.1、if, elseif, else

下面是一个 `if` 条件控制结构的示例，注意其中括号，空格和花括号的位置。 同时注意 else 和 elseif 要和前一个条件控制结构的右花括号在同一行。

```php
<?php
if ($expr1) {
    // if body
} elseif ($expr2) {
    // elseif body
} else {
    // else body;
}
```

推荐用 `elseif` 来替代 `else if` ，以保持所有的条件控制关键字看起来像是一个单词。

## 5.2、switch, case

下面是一个 `switch` 条件控制结构的示例，注意其中括号，空格和花括号的位置。 `case` 语句必须要缩进一级， 而 `break`关键字（或其他中止关键字）必须和 `case` 结构的代码主体在同一个缩进层级。 如果一个有主体代码的 `case` 结构故意的继续向下执行， 则必须要有一个类似于 // no break 的注释。

```php
<?php
switch ($expr) {
    case 0:
        echo 'First case, with a break';
        break;
    case 1:
        echo 'Second case, which falls through';
        // no break
    case 2:
    case 3:
    case 4:
        echo 'Third case, return instead of break';
        return;
    default:
        echo 'Default case';
        break;
}
```
## 5.3、switch, case

下面是一个 `while` 循环控制结构的示例，注意其中括号，空格和花括号的位置。

```php
<?php
while ($expr) {
    // structure body
}
```

下面是一个 `do while` 循环控制结构的示例，注意其中括号，空格和花括号的位置。

```php
<?php
do {
    // structure body;
} while ($expr);
```
## 5.4、for

下面是一个 `for` 循环控制结构的示例，注意其中括号，空格和花括号的位置。

```php
<?php
for ($i = 0; $i < 10; $i++) {
    // for body
}
```
## 5.5、foreach

下面是一个 foreach 循环控制结构的示例，注意其中括号，空格和花括号的位置。

```php
<?php
foreach ($iterable as $key => $value) {
    // foreach body
}
```

## 5.6、try, catch

下面是一个 try catch 异常处理控制结构的示例，注意其中括号，空格和花括号的位置。

```php
<?php
try {
    // try body
} catch (FirstExceptionType $e) {
    // catch body
} catch (OtherExceptionType $e) {
    // catch body
}
```

# 6、闭包

声明闭包时所用的 `function` 关键字之后必须要有一个空格， 而 `use` 关键字的前后都要有一个空格。

闭包的左花括号必须跟其在同一行，而右花括号必须在闭包主体的下一行。

闭包的参数列表和变量列表的左括号后面不可有空格，右括号的前面也不可有空格。

闭包的参数列表和变量列表中逗号前面不可有空格，而逗号后面则必须有空格。

闭包的参数列表中带默认值的参数必须放在参数列表的结尾部分。

下面是一个闭包的示例。注意括号，空格和花括号的位置。

```php
<?php
$closureWithArgs = function ($arg1, $arg2) {
    // body
};

$closureWithArgsAndVars = function ($arg1, $arg2) use ($var1, $var2) {
    // body
};
```

参数列表和变量列表可以被拆分成多个缩进了一级的子行。 如果要拆分成多个子行，列表中的第一项必须放在下一行， 并且每一行必须只放一个参数或变量。

当列表（不管是参数还是变量）最终被拆分成多个子行， 右括号和左花括号之间必须要有一个空格并且自成一行。

下面是一个参数列表和变量列表被拆分成多个子行的示例。


```php
$longArgs_noVars = function (
    $longArgument,
    $longerArgument,
    $muchLongerArgument
) {
   // body
};

$noArgs_longVars = function () use (
    $longVar1,
    $longerVar2,
    $muchLongerVar3
) {
   // body
};

$longArgs_longVars = function (
    $longArgument,
    $longerArgument,
    $muchLongerArgument
) use (
    $longVar1,
    $longerVar2,
    $muchLongerVar3
) {
   // body
};

$longArgs_shortVars = function (
    $longArgument,
    $longerArgument,
    $muchLongerArgument
) use ($var1) {
   // body
};

$shortArgs_longVars = function ($arg) use (
    $longVar1,
    $longerVar2,
    $muchLongerVar3
) {
   // body
};
```


把闭包作为一个参数在函数或者方法中调用时，依然要遵守上述规则。

```php
<?php
$foo->bar(
    $arg1,
    function ($arg2) use ($var1) {
        // body
    },
    $arg3
);
```

# 7、总结

本指南有意的省略了许多元素的代码风格。主要包括：

* 全局变量和全局常量的声明；
* 函数声明；
* 操作符合赋值；
* 行间对齐；
* 注释和文档块；
* 类名的前缀和后缀；
* 最佳实践；

以后的代码规范中可能会修正或扩展本指南中规定的代码风格。

# 附录A 调查

为了写这个风格指南，我们调查了各个项目以最终确定通用的代码风格。 并把这次调查在这里公布出来。

## A.1、调查数据

```php
url,http://www.horde.org/apps/horde/docs/CODING_STANDARDS,http://pear.php.net/manual/en/standards.php,http://solarphp.com/manual/appendix-standards.style,http://framework.zend.com/manual/en/coding-standard.html,http://symfony.com/doc/2.0/contributing/code/standards.html,http://www.ppi.io/docs/coding-standards.html,https://github.com/ezsystems/ezp-next/wiki/codingstandards,http://book.cakephp.org/2.0/en/contributing/cakephp-coding-conventions.html,https://github.com/UnionOfRAD/lithium/wiki/Spec%3A-Coding,http://drupal.org/coding-standards,http://code.google.com/p/sabredav/,http://area51.phpbb.com/docs/31x/coding-guidelines.html,https://docs.google.com/a/zikula.org/document/edit?authkey=CPCU0Us&hgd=1&id=1fcqb93Sn-hR9c0mkN6m_tyWnmEvoswKBtSc0tKkZmJA,http://www.chisimba.com,n/a,https://github.com/Respect/project-info/blob/master/coding-standards-sample.php,n/a,Object Calisthenics for PHP,http://doc.nette.org/en/coding-standard,http://flow3.typo3.org,https://github.com/propelorm/Propel2/wiki/Coding-Standards,http://developer.joomla.org/coding-standards.html
voting,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,no,no,no,?,yes,no,yes
indent_type,4,4,4,4,4,tab,4,tab,tab,2,4,tab,4,4,4,4,4,4,tab,tab,4,tab
line_length_limit_soft,75,75,75,75,no,85,120,120,80,80,80,no,100,80,80,?,?,120,80,120,no,150
line_length_limit_hard,85,85,85,85,no,no,no,no,100,?,no,no,no,100,100,?,120,120,no,no,no,no
class_names,studly,studly,studly,studly,studly,studly,studly,studly,studly,studly,studly,lower_under,studly,lower,studly,studly,studly,studly,?,studly,studly,studly
class_brace_line,next,next,next,next,next,same,next,same,same,same,same,next,next,next,next,next,next,next,next,same,next,next
constant_names,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper,upper
true_false_null,lower,lower,lower,lower,lower,lower,lower,lower,lower,upper,lower,lower,lower,upper,lower,lower,lower,lower,lower,upper,lower,lower
method_names,camel,camel,camel,camel,camel,camel,camel,camel,camel,camel,camel,lower_under,camel,camel,camel,camel,camel,camel,camel,camel,camel,camel
method_brace_line,next,next,next,next,next,same,next,same,same,same,same,next,next,same,next,next,next,next,next,same,next,next
control_brace_line,same,same,same,same,same,same,next,same,same,same,same,next,same,same,next,same,same,same,same,same,same,next
control_space_after,yes,yes,yes,yes,yes,no,yes,yes,yes,yes,no,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes,yes
always_use_control_braces,yes,yes,yes,yes,yes,yes,no,yes,yes,yes,no,yes,yes,yes,yes,no,yes,yes,yes,yes,yes,yes
else_elseif_line,same,same,same,same,same,same,next,same,same,next,same,next,same,next,next,same,same,same,same,same,same,next
case_break_indent_from_switch,0/1,0/1,0/1,1/2,1/2,1/2,1/2,1/1,1/1,1/2,1/2,1/1,1/2,1/2,1/2,1/2,1/2,1/2,0/1,1/1,1/2,1/2
function_space_after,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no,no
closing_php_tag_required,no,no,no,no,no,no,no,no,yes,no,no,no,no,yes,no,no,no,no,no,yes,no,no
line_endings,LF,LF,LF,LF,LF,LF,LF,LF,?,LF,?,LF,LF,LF,LF,?,,LF,?,LF,LF,LF
static_or_visibility_first,static,?,static,either,either,either,visibility,visibility,visibility,either,static,either,?,visibility,?,?,either,either,visibility,visibility,static,?
control_space_parens,no,no,no,no,no,no,yes,no,no,no,no,no,no,yes,?,no,no,no,no,no,no,no
blank_line_after_php,no,no,no,no,yes,no,no,no,no,yes,yes,no,no,yes,?,yes,yes,no,yes,no,yes,no
class_method_control_brace,next/next/same,next/next/same,next/next/same,next/next/same,next/next/same,same/same/same,next/next/next,same/same/same,same/same/same,same/same/same,same/same/same,next/next/next,next/next/same,next/same/same,next/next/next,next/next/same,next/next/same,next/next/same,next/next/same,same/same/same,next/next/same,next/next/next
```

## A.2、调查说明

`indent_type`：缩进类型。`tab` = “使用tab制表符键”，`2` or `4` = “空格数量”。

`line_length_limit_soft`：用字符个数表示的行长度的“软”限制。`?` = 不识别或者没有响应，`no` 意为不限制。

`line_length_limit_hard`：用字符个数表示的行长度的“硬”限制。 `?` = 不识别或者没有响应, `no` 意为不限制。

`class_names`：怎样命名类名。 `lower` = 只是小写, `lower_under` = 小写加下划线, `studly` = 骆驼命名法。

`class_brace_line`：定义类主体的开始花括号是与 `class` 关键字放在同一行？还是单独另起一行？

`constant_names`：类常量如何命名？`upper` = 大写加下划线分隔符。

`true_false_null`：  `true` ,  `false` , 和 `null` 关键字全部小写或者全部大写？

`method_names`：方法名如何命名？`camel` = 驼峰命名法,  `lower_under` = 小写加下划线分隔符。

`method_brace_line`：定义方法主体的开始花括号是与 `function` 关键字放在同一行？还是单独另起一行？

`control_brace_line`：流程控制结构主体的开始花括号是与控制结构关键字放在同一行？还是单独另起一行？

`control_space_after`：控制结构关键字之后是否有空格？

`always_use_control_braces`：控制结构是否总是使用花括号？

`else_elseif_line`：当使用 `else` 和 `elseif` 时，是与花括号放在同一行，还是单独另起一行？

`case_break_indent_from_switch`: 在 `swith` 语句使用 `case` 和 `break` 关键字时缩进多少次？

`function_space_after`：函数调用时的函数名和左圆括号之间是否有空格？

`closing_php_tag_required`：如过是纯PHP文件，关闭标签 `?>` 是否需要？

`line_endings`：使用哪种行结束符？

`static_or_visibility_first`：在定义方法时，是先写 `static` 关键字还是先写访问修饰符？

`control_space_parens`：在控制结构表达式中，左圆括号后面和右圆括号的前面是否有一个空格？`yes` = `if ( $expr )`, `no` = `if ($expr)`。

`blank_line_after_php`：在PHP文件的开始标签后面是否需要一个空行？

`class_method_control_brace`：开始花括号在类，方法和控制结构中的位置。

## A.3、调查结果

```php
indent_type:
    tab: 7
    2: 1
    4: 14
line_length_limit_soft:
    ?: 2
    no: 3
    75: 4
    80: 6
    85: 1
    100: 1
    120: 4
    150: 1
line_length_limit_hard:
    ?: 2
    no: 11
    85: 4
    100: 3
    120: 2
class_names:
    ?: 1
    lower: 1
    lower_under: 1
    studly: 19
class_brace_line:
    next: 16
    same: 6
constant_names:
    upper: 22
true_false_null:
    lower: 19
    upper: 3
method_names:
    camel: 21
    lower_under: 1
method_brace_line:
    next: 15
    same: 7
control_brace_line:
    next: 4
    same: 18
control_space_after:
    no: 2
    yes: 20
always_use_control_braces:
    no: 3
    yes: 19
else_elseif_line:
    next: 6
    same: 16
case_break_indent_from_switch:
    0/1: 4
    1/1: 4
    1/2: 14
function_space_after:
    no: 22
closing_php_tag_required:
    no: 19
    yes: 3
line_endings:
    ?: 5
    LF: 17
static_or_visibility_first:
    ?: 5
    either: 7
    static: 4
    visibility: 6
control_space_parens:
    ?: 1
    no: 19
    yes: 2
blank_line_after_php:
    ?: 1
    no: 13
    yes: 8
class_method_control_brace:
    next/next/next: 4
    next/next/same: 11
    next/same/same: 1
    same/same/same: 6
```