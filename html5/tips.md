## 面试

- Cookie 的优缺点（限制或弊端）？
- 浏览器本地存储，html5中的Web Storage包括了两种存储方式：sessionStorage和localStorage。
- 介绍一下CSS的盒子模型
- 浮动原理和清除浮动
- 事件的代理/委托
- 闭包原理及应用
- JSONP原理

### 第一部分：Object Prototypes (对象原型)

刚开始很简单。我会让侯选人去定义一个方法，传入一个string类型的参数，然后将string的每个字符间加个空格返回，例如：

spacify('hello world') // => 'h e l l o  w o r l d'    

尽管这个问题似乎非常简单，其实这是一个很好的开始，尤其是对于那些未经过电话面试的侯选人——他们很多人声称精通JavaScript，但通常连一个简单的方法都不会写。

下面是正确答案，有时侯选人可能会用一个循环，这也是一种可接受的答案。

function spacify(str) {
  return str.split('').join(' ');
}

接下来，我会问侯选人，如何把这个方法放入String对象上面，例如：

'hello world'.spacify();

问这个问题可以让我考察侯选人是否对function prototypes(方法原型)有一个基本的理解。这个问题会经常引起一些有意思的讨论：直接在对象的原型（prototypes）上添加方法是否安全，尤其是在Object对象上。最后的答案可能会像这样：

String.prototype.spacify = function(){
  return this.split('').join(' ');
};

到这儿，我通常会让侯选人解释一下函数声明和函数表达式的区别。

### 第二部分：参数 arguments

下一步我会问一些简单的问题去考察侯选人是否理解参数（arguments）对象。我会让他们定义一个未定义的log方法作为开始。

log('hello world')

我会让侯选人去定义log，然后它可以代理console.log的方法。正确的答案是下面几行代码，其实更好的侯选人会直接使用apply.

function log(msg)　{
  console.log(msg);
}

他们一旦写好了，我就会说我要改变我调用log的方式，传入多个参数。我会强调我传入参数的个数是不定的，可不止两个。这里我举了一个传两个参数的例子。

log('hello', 'world');

希望你的侯选人可以直接使用apply。有时人他们可能会把apply和call搞混了，不过你可以提醒他们让他们微调一下。传入console的上下文也非常重要。

function log(){
  console.log.apply(console, arguments);
};

接下来我会让侯选人给每一个log消息添加一个"(app)"的前辍，比如：

'(app) hello world'

现在可能有点麻烦了。好的侯选人知道arugments是一个伪数组，然后会将他转化成为标准数组。通常方法是使用Array.prototype.slice，像这样：

function log(){
  var args = Array.prototype.slice.call(arguments);
  args.unshift('(app)');

  console.log.apply(console, args);
};

## 第三部分：上下文

下一组问题是考察侯选人对上下文和this的理解。我先定义了下面一个例子。注意count属性不是只读取当前下下文的。

var User = {
  count: 1,

  getCount: function() {
    return this.count;
  }
};

我又写了下面几行，然后问侯选人log输出的会是什么。

console.log(User.getCount());

var func = User.getCount;
console.log(func());

这种情况下，正确的答案是1和undefined。你会很吃惊，因为有很多人被这种最基础的上下文问题绊倒。func是在winodw的上下文中被执行的，所以会访问不到count属性。我向侯选人解释了这点，然后问他们怎么样保证User总是能访问到func的上下文，即返回正即的值：1

正确的答案是使用Function.prototype.bind，例如：

var func = User.getCount.bind(User);
console.log(func());

接下来我通常会说这个方法对老版本的浏览器不起作用，然后让侯选人去解决这个问题。很多弱一些的侯选人在这个问题上犯难了，但是对于你来说雇佣一个理解apply和call的侯选人非常重要。

Function.prototype.bind = Function.prototype.bind || function(context){
  var self = this;

  return function(){
    return self.apply(context, arguments);
  };
}

## 第四部分：弹出窗口（Overlay library）

面试的最后一部分，我会让侯选人做一些实践，通过做一个‘弹出窗口’的库。我发现这个非常有用，它可以全面地展示一名前端工程师的技能：HTML,CSS和JavaScript。如果侯选人通过了前面的面试，我会马上让他们回答这个问题。

实施方案是由侯选人自己决定的，但是我也希望他们能通过以下几点来实现：

在遮罩中最好使用position中的fixed代替absolute属性，这样即使在滚动的时侯，也能始终让遮罩始盖住整个窗口。当侯选人忽略时我会提示他们这一点，并让他们解释fixed和absolute定位的区别。

.overlay {
  position: fixed;
  left: 0;
  right: 0;
  bottom: 0;
  top: 0;
  background: rgba(0,0,0,.8);
}

他们如何让里面的内容居中也是需要考察的一点。一些侯选人会选择CSS和绝对定位，如果内容有固定的宽、高这是可行的。否则就要使用JavaScript.

.overlay article {
  position: absolute;
  left: 50%;
  top: 50%;
  margin: -200px 0 0 -200px;
  width: 400px;
  height: 400px;
}

我也会让侯选人确保当遮罩被点击时要自动关闭，这会很好地考查事件冒泡机制的机会。通常侯选人会在overlay上面直接绑定一个点击关闭的方法。

$('.overlay').click(closeOverlay);

这是个方法，不过直到你认识到点击窗口里面的东西也会关闭overlay的时侯——这明显是个BUG。解决方法是检查事件的触发对象和绑定对象是否一致，从而确定事件不是从子元素里面冒上来的，就像这样：

$('.overlay').click(function(e){
  if (e.target == e.currentTarget)
    closeOverlay();
});

## 其他方面

当然这些问题只能覆盖前端一点点的知识的，还有很多其他的方面你有可能会问到，像性能，HTML5 API, AMD和CommonJS模块模型，构造函数（constructors），类型和盒子模型（box model）。根据侯选人的情况，我经常会随机提些问题。