#PSR-4: 自动加载器

RFC 2119 中的必须(MUST)，不能(MUST NOT)， 建议(SHOULD)，不建议(SHOULD NOT)，可以/可能(MAY)等关键词将在本节用来做一些解释性的描述。

##1、概述

该 PSR 文档从文件路径的角度来描述了一个 `autoloading` 类的规范，它是完全可互操作的，并且可以额外使用其它的自动载入规范，包括 PSR-0 规范。该 PSR 文档还描述了如何规范的存放文件来到达自动载入的目的。

##2、规范

* "类" 是一个泛称；它包含类，结构，`traits` 以及其他类似的结构。
* 完整的类名应该遵循类似如下范例：
	```php
	\<NamespaceName>(\<SubNamespaceNames>)*\<ClassName>
	```
	*  一个完整的类名必须有一个顶级命名空间，也就是众所周知的 "vendor namespace"；
	*  一个完整的类名都可以有一个或多个子命名空间；
	*  一个完整的类名都必须有一个终止类名；
	*  下划线在完整的类名的任何部分都是没有特殊意义；
	*  一个完整的类名由大小写字母组成；
	*  所有的类名必须以区分大小写的方式引用；

* 当加载一个文件时应该对应一个完整的类名 ...

	*  一个连续的一个或多个主命名空间和子命名空间名称，不包括主命名空间分隔符， 在完整的类名（一个“命名空间前缀”）中必须对应于至少一个“基本目录”。
	*  在“命名空间前缀”后的连续子命名空间名称对应的子目录中的“基本目录”， 其中的命名空间分隔符表示目录分隔符， 子目录名称必须匹配子命名空间名称。
	*  最后的类名应该和 `.php` 文件名匹配。文件名的大小写必须匹配；

* 自动载入器的实现不能抛出任何异常，不能抛出任何等级的错误；也不能返回值。


##3、示例

如下表格展示的是完整的类名与其中相关文件路径的关系：

| 完整的类名                    | 命名空间前缀       |       基础目录           | 实际的类文件路径
| ----------------------------- |--------------------|--------------------------|-------------------------------------------
| \Acme\Log\Writer\File_Writer  | Acme\Log\Writer    | ./acme-log-writer/lib/   | ./acme-log-writer/lib/File_Writer.php
| \Aura\Web\Response\Status     | Aura\Web           | /path/to/aura-web/src/   | /path/to/aura-web/src/Response/Status.php
| \Symfony\Core\Request         | Symfony\Core       | ./vendor/Symfony/Core/   | ./vendor/Symfony/Core/Request.php
| \Zend\Acl                     | Zend               | /usr/includes/Zend/      | /usr/includes/Zend/Acl.php

用来示例的实现用例符合本文所提出的规范，请查阅 [examples file](https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-4-autoloader-examples.md) ；实现用例不能作为本指南的一部分；其用例内容可能随时被更改；

