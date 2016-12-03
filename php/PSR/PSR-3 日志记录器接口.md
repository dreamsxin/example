#PSR-3: 日志系统接口
本文档描述了一些编写一个日志系统库需要的通用接口。

本文的主要目的是允许编写的类库接收一个 `Psr\Log\LoggerInterface` 对象并且该类库可以以一个简单和通用的方式来向其中写入日志文档。 有自定义需求的框架和CMS系统可以扩展这些接口来实现他们特定的需求和功能，但是应该与本文提出的内容保持兼容性。这确保了第三方库的应用程序使用可以写入到集中的应用程序日志。

本文中的 `implementor` 一词被翻译为某人在一个日志相关的库或框架中实现了 `LoggerInterface` 接口。用户记录被称为用户。

 

##1、规格
###1.1、基础

* `LoggerInterface` 接口暴露出了八个方法，这八个方法分别用来写入八种不同等级的日志。这八种不同等级的日志根据RFC 5424标准被分为（debug, info, notice, warning, error, critical, alert, emergency）

* 第九个方法 `log` 方法，接受一个日志级别作为第一个参数。调用此方法并传入一个代表日志等级的常量必须与直接调用对应以上八个不同级别的日志方法有同样的结果。如果调用此方法时传入一个无法识别的日志等级必须抛出一个 `Psr\Log\InvalidArgumentException` 异常。如果该方法的实现没有说明支持的日志等级。用户不应该在不清楚当前实现的 `log` 方法是否支持自定义等级时使用一个自定义的日志等级。

###1.2、消息

* 每一个方法都接收一个字符串或者一个含有 `__toString()` 的对象作为消息体。

* message参数中可能包含一些可以被 `context` 参数的数值所替换的占位符。

* 占位符名字必须和 `context` 数组类型参数的键名对应。

* 占位符名字必须使用一对花括号来作为分隔符。在占位符和分隔符之间不能有任何空格。

* 占位符名字应该只能由`A-Z，a-z，0-9，下划线_和句号.组成。其它的字符作为以后占位符规范的保留字。

*实现者 可以使用占位符来实现不同的转义和翻译日志成文。因为用户并不知道上下文数据会是什么，所以不推荐提前转义占位符。

下面提供一个占位符替换的例子，仅作为参考：

```php
<?php
  /**
   * 用上下文变量替换消息中的占位符  
   */
  function interpolate($message, array $context = array())
  {   
      // 构造一个替换数组，并用花括号将数组的键包裹起来
      $replace = array();
      foreach ($context as $key => $val) {
          $replace['{' . $key . '}'] = $val;
      }
      // 插入要替换的值到消息中并返回
      return strtr($message, $replace);
  }
 
  // 一个包含以括号分割的占位符的消息
  $message = "User {username} created";
 
  // 一个包含占位符的上下文数组 名字 => 替换的值
  $context = array('username' => 'bolivar');
 
  // 打印 "Username bolivar created"
  echo interpolate($message, $context);
```

###1.3、上下文

* 每个方法接受一个数组作为上下文数据，用来存储不适合在字符串中填充的多余的信息。数组可以包括任何东西。实现者必须确保他们尽可能包容的对 `context` 参数进行处理。一个 `context` 参数的给定值不可导致抛出异常，也不可产生任何PHP错误，警告或者提醒。

* 如果在 `context` 参数中传入了一个异常对象，它必须以 `exception` 作为键名。记录异常轨迹是通用的模式，并且可以在日志系统支持的情况下从异常中提取出整个调用栈。实现者在将 `exception` 当做异常对象来使用之前必须去验证它是不是一个异常对象，因为它可能包含着任何东西。

###1.4、助手类和接口


* `Psr\Log\AbstractLogger` 类可以让你通过继承它并实现通用的 `log` 方法来方便的实现 `LoggerInterface` 接口。而其他八个方法将会把消息和上下文转发给 `log` 方法。

* 类似的，使用 `Psr\Log\LoggerTrait` 只需要你实现通用的 `log` 方法。注意特性是不能用来实现接口的，所以你依然需要在你的类中实现 `LoggerInterface`。

* `Psr\Log\NullLogger` 是和接口一起提供的。它在没有可用的日志记录器时，可以为使用日志接口的用户们提供一个后备的“黑洞”。但是，当context参数的构建非常耗时的时候，直接判断是否需要记录日志可能是个更好的选择。

* `Psr\Log\LoggerAwareInterface` 只有一个 `setLogger(LoggerInterface $logger)` 方法，它可以在框架中用来随意设置一个日志记录器。

* `Psr\Log\LoggerAwareTrait` 特性可以被用来在各个类中轻松实现相同的接口。通过它可以访问到 `$this->logger`。

* `Psr\Log\LogLevel` 类包含了八个代表不同日志等级的常量。



##2、包

`psr/log` 包中提供了上文描述过的接口和类，以及相关的异常类，还有一组用来验证你的实现是否规范的测试工具。

##2、Psr\Log\LoggerInterface

```php
<?php

namespace Psr\Log;
/**
 * 描述一个日志记录器实例；
 *
 * 日志的消息体必须是一个字符串或者是一个实现了__toString()方法的对象；
 * 
 * 日志消息体中可以含有格式如“{foo}”的占位符，该占位符会被上下文数据中的键为“foo”的值所代替；
 * 
 * 
 * 上下文数组可以包含任意的数据，但唯一需要注意的是当一个Exception实例被用来产生一个堆栈跟踪时，它必须被放置在一个键名为“exception”的变量中；
 * 
 * 
 * 
 * 可以参考 https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-3-logger-interface.md 来获得更加全面的接口规范；
 * 
 */
interface LoggerInterface
{
    /**     
     * 系统不稳定的状态；   
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function emergency($message, array $context = array());
 
    /**     
     * 必须采取某一措施的状态； 
     *     
     * 比如: 整个网站无法运行, 数据库不可用, 等等。此时应该触发“短信”提醒警告；
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function alert($message, array $context = array());
 
    /**     
     * 严重的情况.     
     *     
     * 比如: 程序的某一组件不可用, 非预期的异常； 
     *    
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function critical($message, array $context = array());
 
    /**     
     * 运行时错误，此时不需要立即采取相应措施，但是需要被记录下来以备检测；    
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function error($message, array $context = array());
 
    /**     
     * 出现非错误性的异常；     
     *     
     * 比如: 使用了被弃用的API, 错误地使用了API或其他不赞成的事务而导致了不必要的错误；
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function warning($message, array $context = array());
 
    /**     
     * 正常但是很重要的事情；  
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function notice($message, array $context = array());
 
    /**     
     * 感兴趣的事件；   
     *     
     * 比如: 用户登录, SQL语句的记录；
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function info($message, array $context = array());
 
    /**     
     * 详细的调试信息；    
     *     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function debug($message, array $context = array());
 
    /**     
     * 任意等级的日志记录；
     *     
     * @param mixed $level     
     * @param string $message     
     * @param array $context     
     * @return null     
     */
    public function log($level, $message, array $context = array());
}
```

##4、Psr\Log\LoggerAwareInterface

```php
<?php

namespace Psr\Log;
 
/**
 * logger-aware 定义实例
 */
interface LoggerAwareInterface
{
    /**
     * 在一个对象上部署日志记录器实例
     *     
     * @param LoggerInterface $logger     
     * @return null     
     */
    public function setLogger(LoggerInterface $logger);
}
```

##5、Psr\Log\LogLevel

```php
<?php

namespace Psr\Log;
 
/**
 * 该类描述了日志的不同等级
 */
class LogLevel
{
    const EMERGENCY = 'emergency';
    const ALERT     = 'alert';
    const CRITICAL  = 'critical';
    const ERROR     = 'error';
    const WARNING   = 'warning';
    const NOTICE    = 'notice';
    const INFO      = 'info';
    const DEBUG     = 'debug';
}
```