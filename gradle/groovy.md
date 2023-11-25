# Groovy 语法

## 面向对象和过程

`groovy` 不仅可以面向对象进程编程，也可以创建面向过程的脚本开发：
```groovy
package variable

int i1 = 1
double d1 = 1
println(i1.class)
println(d1.class)
```

## 语法简洁
- 默认行尾不加分号
- 默认属性为 public
- 默认给类内部属性，创建setter和getter方法，外部只需要使用属性名访问即可

## 字符串
- 单引号
代表字符串只读

- 双引号
在字符串内部可以使用${}引用外部元素
`def s6 = "2+3 is ${2+3}"`

- 三引号
支持换行

## 弱声明 def
- def
编译过程会自动类型检测，这个属性和`kotlin`的`val/var`很像。

`def str = "this is groovy str"`

## 方法

使用 `return` 返回，如果省略`return`，则返回的是最后方法最后一行。

```groovy
def echo(){
    return "echo this"
}
println echo()
```
### 方法参数
```groovy
def echo(message,name = '123'){
    return "echo:"+message +name
}
echo("hello groovy")
```

## 方法调用流程

- invokeMethod
对于类中所有方法的调用：包括已定义和未定义方法，都会走到这个 invokeMethod 方法中。

```groovy
class Person implements GroovyInterceptable {
  def name
  def age
  def score
  @Override
  Object invokeMethod(String methodName, Object args) {
    return "this method name is $methodName"
  }
  def helloGroovy(){
     return "hello $name"
  }
}
def per = new Person(name: 'lily')
println per.helloGroovy()
println per.helloGroovy1()
```
结果：
```txt
this method name is helloGroovy
this method name is helloGroovy1
```

- methodMissing
对于未定义的方法，如果重写这个methodMissing，则会调用这个方法
```groovy
Object methodMissing(String name, Object args) {
  println "methodMissing : $name"
}
```

- 元编程 metaClass
可以在运行期注入属性和方法包括静态方法，对于一些第三方类库，可以使用这个方式在运行期动态创建方法，相当于对类库进行了一次扩展。
```groovy
class Person implements{
    def name
    def age
}
//注入属性
Person.metaClass.sex = 'male'
def person1 = new Person(name: 'yuhb',age: 29)
println person1.sex //结果：male

注入方法：
//注入方法，使用闭包
Person.metaClass.sexUpperCase = {  ->
    sex.toUpperCase()
}
println person1.sexUpperCase() //结果：MALE
```
## 集合分类
groovy中集合有三种：

- 列表List
对应java中的List
`def list = [1, 2, 3, 4,5,6]`
- 映射Map
对应java中的Map
`def map = [key1:'value',key2:'value2']`
注意：map中的key默认都是String类型的字符串，即使我们自己没加，编译器也会给我们加上

-范围Range
groovy中独有
`def range = [1..100]`
range其实就是指定了一个list的范围，而不需要一个一个列出来

如下使用：
```groovy
/******switch case*****/
println getGrade(87)
def getGrade(def number){
    def result
    switch (number){
            case 0..<60:
                    result = "不及格"
                    break
            case 60..<80:
                    result = "及格"
                    break
            case 80..100:
                    result = "优"
                    break
            default:
                    result = "不确定"
                    break
    }
    result
}
```
### 集合遍历
所有集合都可以使用each和eachWithIndex进行遍历，当然也可以使用java中的for循环，但在groovy中一般不这么用
```groovy
class Stu{
    def name
    def age

    @Override
    String toString() {
        return "name:$name age:$age"
    }
}
def students = [
        1:new Stu(name: 'lily',age: 12),
        2:new Stu(name:  'lucy',age:  13),
        3:new Stu(name:  'tom',age:  14),
        4:new Stu(name:  'sara',age:  15)
]
/**1.遍历**/
students.each {
    println it.value.toString()
}
/**带索引遍历**/
students.eachWithIndex{ Map.Entry<Integer, Stu> entry, int i ->
    println "index:$i key:$entry.key value:$entry.value "
}
```
### 查找
groovy中查找提供了find和findAll方法，用法如下:
```groovy
//find查找一项
def stu1 = students.find {
    it.value.age>12
}
println stu1.value
//findAll查找所有项
def stus = students.findAll {
    it.value.age>12
}
//count:统计个数
def stuCount = students.count {
    it.value.age>12
}

//多重查找
def stu2 = students.findAll {
    it.value.age>12
}.collect {
    it.value.name
}
```
### 分组：
使用 groupBy 关键字
```groovy
def group = students.groupBy {
    return it.value.age>12?"大":"小"
}
```
### 排序：
使用sort
```groovy
def sort = students.sort {student1,student2 ->
    student1.value.age == student2.value.age?0:student1.value.age < student2.value.age?-1:1
}
```
## 闭包
闭包在我们groovy中起着很大比重，如果想要学好groovy，闭包一定得掌握好， 在我们build.gradle其实就有很多闭包使用： 如:
```groovy
android{
    sourceSets {
        main{
                jniLibs.srcDirs = ['libs']
        }
    }
}
```
这里面的 android {} 其实就是一个闭包结构，其内部的sourceSets{}又是闭包中的闭包，可以看到闭包在我们的gradle中无处不在.

学好闭包真的很关键
常用闭包方式：
```groovy
{'abc'}
{ -> 'abc'}
{ -> "abc"+$it}
{ String name -> 'abc'}
{ name -> "abc${name}"}
{ name,age -> "abc${name}"+age}
```
### 闭包的定义及基本方法
闭包概念：其实就是一段代码段，你把闭包想象为java中的回调Callback即可， 闭包在Groovy中是groovy.lang.Closure 的实例,可以直接赋值给其他变量.
闭包的调用：
```groovy
def closer = {1234}
closer()
closer.call()
```
闭包参数：带参数的闭包 使用 -> 如果是一个参数可以直接使用it代替和kotlin中的lambda类型类似
```groovy
def closerParam = { name,age ->
    println "hello groovy:${name}:${age}"
    'return hei'
}
def result = closerParam("lily",123)

println result //打印结果：return hei
```
### 闭包使用详解
#### 与基本类型结合使用：
```groovy
//upto:实现阶乘
int x= fab_upTo(5)
println(x)
int fab_upTo(int number){
    int result = 1
    1.upto(number,{result*=it})
    return result
}
//downto:实现阶乘
int x1= fab_downTo(5)
println(x1)
int fab_downTo(int number){
    int result = 1
    number.downto(1){result*=it}
    return result
}
//times:实现累加
int x2 = cal(101)
println(x2)

int cal(int number){
	def result = 0;
	number.times {
	    result+=it
	}
	return result
}
```
#### 与String结合使用
```groovy
String str = "the 2 and 3 is 5"
//each:遍历查找,返回值是str自己
println str.each {temp ->
        print  temp.multiply(2)
}
//find查找一个符合条件的
println str.find {
        it.isNumber()
}
//findAll查找所有符合条件的，返回的是一个集合
println str.findAll {
        it.isNumber()
}
//any表示查找只要存在一个符合的就是true
println str.any { s ->
        s.isNumber()
}
//every表示全部元素都要符合的就是true
println str.every {
        it.isNumber()
}
//将所有字符进行转化后，放到一个List中返回
def list = str.collect {
        it.toUpperCase()
}
println(list)
```

### 闭包进阶详解
闭包关键变量:this, owner, delegate
#### 情况1：一般情况：
```groovy
def scriptCloser = {
    println "scriptCloser:this:${this}"
    println "scriptCloser:owner:${owner}"
    println "scriptCloser:delegate:${delegate}"
}
scriptCloser()
```
结果：
```txt
scriptCloser:this:variable.Closer@58a63629
scriptCloser:owner:variable.Closer@58a63629
scriptCloser:delegate:variable.Closer@58a63629
```
可以看到一般情况下：三种都是相等的：都代表当前闭包对象

#### 情况2：我们来看下面的情况：闭包中有闭包
```groovy
def nestClosure = {
    def innerClosure = {
            println "innerClosure:this:"+this.getClass()
            println "innerClosure:owner:${owner.getClass()}"
            println "innerClosure:delegate:${delegate.getClass()}"
    }
    innerClosure()
}
nestClosure()
```
结果：
```txt
innerClosure:this:class variable.Closer
innerClosure:owner:class variable.Closer$_run_closure10
innerClosure:delegate:class variable.Closer$_run_closure10
```
看到在闭包中调用闭包：

this还是执行外部的Closer对象，而owner和delegate变为了Closer的内部闭包对象

#### 情况3：最后来看一种情况：使用delegate委托
```groovy
class Student{
    def name
    def pretty = {println "my name is ${name}"}
    void showName(){
            pretty.call()
    }
}
class Teacher{
    def name
}
Student stu1 = new Student(name: 'yuhb')
Teacher tea1 = new Teacher(name: 'lily')

//改变委托delegate
stu1.pretty.delegate = tea1
stu1.showName()
//设置委托策略
stu1.pretty.resolveStrategy = Closure.DELEGATE_FIRST
stu1.showName()
```
结果：
```txt
my name is yuhb
my name is lily
```
通过上面三种情况：

总结出：

`this`：指向最外部的 Closer 对象 `owner`：执行当前闭包的 Closer 对象，特指当前，所以对闭包中的闭包，指向内部的闭包delegate：这个是闭包的代理对象，如果有单独配置这个 delegate，且设置了委托策略 `DELEGATE_FIRST`， 则闭包中的所有内部属性都会优先使用delegate中的对象
下面我们就来讲解闭包的委托策略

## 闭包委托策略
闭包中给我提供了以下策略：
```groovy
//优先使用ower中的属性
public static final int OWNER_FIRST = 0;
//优先使用delegate中的属性
public static final int DELEGATE_FIRST = 1;
//只是有owner中的属性
public static final int OWNER_ONLY = 2;
//只是有delegate中的属性
public static final int DELEGATE_ONLY = 3;
//使用this中的属性
public static final int TO_SELF = 4;
```

## 文件
groovy文件操作完全兼容java的文件操作，但groovy集成了自己的高阶使用方式

```groovy
def file = new File('../../hello_groovy.iml')
def buf1 = file.withReader {reader ->
        char[] buf = new char[100]
        reader.read(buf)
        buf
}
println buf1
```
```groovy
//写文件：withWriter：实现文件拷贝操作
def result = copy('../../hello_groovy1.iml','../../hello_groovy.iml')   
println result
def copy(String desFilePath,String srcFilePath){
try {
    File desFile = new File(desFilePath)
    if(!desFile.exists()){
        desFile.createNewFile()
    }
    File srcFile = new File(srcFilePath)
    if(!srcFile.exists()){
        return false
    }else{
        srcFile.withReader {reader ->
            def lines = reader.readLines()
            desFile.withWriter {writer ->
                    lines.each {line ->
                            writer.write(line+'\r\n')
                    }
            }
            return true
        }
    }
}catch(Exception e){
    return false
}

}
```
```groovy
//读对象
def ob1 = readObject('../../person.bin')
println ob1
def readObject(String srcFilePath){
    try {
        File desFile = new File(srcFilePath)
        if(!desFile.exists()){
            return false
        }
        desFile.withObjectInputStream {
            def person = it.readObject()
            println person.name
        }
        return true
    }catch(Exception e){
            return false
    }
}
```
```groovy
//写对象：
Person person = new Person(name: 'uihb',age: 32)
saveObject(person,'../../person.bin')

def saveObject(Object obj,String desFilePath){
    try {
        File desFile = new File(desFilePath)
        if(!desFile.exists()){
            desFile.createNewFile()
        }
        if(obj != null){
            desFile.withObjectOutputStream {
                    it.writeObject(obj)
            }
        }
    }catch(Exception e){
            return false
    }
}
```
## Json
```groovy
//1.Object 转JSon
def personList = [
        new Person(name: 'lily',age: 12),
        new Person(name: 'lucy',age: 14),
        new Person(name: 'kare',age: 18)
]

def jsonPerson =  JsonOutput.toJson(personList)
println JsonOutput.prettyPrint(jsonPerson)
```
```groovy
//2.JSon转Object

def jsonSlurper = new JsonSlurper()
def obj = jsonSlurper.parseText(jsonPerson)
println(obj[0].name)
```
从网络获取Json数据操作
```groovy
// 这里引入OkHttp
def getNetWork(String url){
    OkHttpClient client = new OkHttpClient();
    Request request = new Request.Builder()
            .url(url)
            .get()
            .build();
    Call call = client.newCall(request)
    call.enqueue(new Callback() {
        @Override
        void onFailure(Request _request, IOException e) {

        }

        @Override
        void onResponse(Response response) throws IOException {
            def res = new String(response.body().bytes())
            println res
            JsonSlurper jsonSlurper1 = new JsonSlurper()
            Version objetres = (Version)jsonSlurper1.parseText(res)
            println  objetres.ecode
        }
    })
    sleep(10000)
}

class Version{
    int ecode
    String emsg
    CurrentVersion data
}
class CurrentVersion{
    String currentVersion
}
```

## 使用XmlSlurper进行解析
```groovy
final String xml = '''
    <response version-api="2.0">
        <value>
            <books id="1" classification="android">
                <book available="20" id="1">
                    <title>疯狂Android讲义</title>
                    <author id="1">李刚</author>
                </book>
                <book available="14" id="2">
                   <title>第一行代码</title>
                   <author id="2">郭林</author>
               </book>
               <book available="13" id="3">
                   <title>Android开发艺术探索</title>
                   <author id="3">任玉刚</author>
               </book>
               <book available="5" id="4">
                   <title>Android源码设计模式</title>
                   <author id="4">何红辉</author>
               </book>
           </books>
           <books id="2" classification="web">
               <book available="10" id="1">
                   <title>Vue从入门到精通</title>
                   <author id="4">李刚</author>
               </book>
           </books>
       </value>
    </response>
'''

//开始解析:XmlSlurper
def xmlSluper = new XmlSlurper()
def response = xmlSluper.parseText(xml)

println response.value.books[0].book[0].title
println response.value.books[1].book[0].@available

//xml遍历：遍历所有的李刚的书名
def list = []
response.value.books.each { books->
    books.book.each { book ->
        if(book.author == '李刚'){
            list.add(book.title)
        }
    }
}
println list.toListString() //打印结果：[疯狂Android讲义, Vue从入门到精通]
```
## xml节点遍历
深度遍历：遍历所有的李刚的书名
```groovy
def depFirst = response.depthFirst().findAll { book ->
    return book.author.text() == '李刚' ? true : false
}.collect { book ->
    book.title
}
println depFirst.toListString() //打印结果：[疯狂Android讲义, Vue从入门到精通]

//广度遍历
def name1 = response.value.books.children().findAll { node ->
    node.name() =='book' && node.@id == '2'‘
}.collect { node ->
    node.title
}
println name1
```
打印结果：[第一行代码]

## groovy中如何创建一个xml:使用MarkupBuilder
需求：生成xml格式数据
```groovy
/**
 * 生成xml格式数据
 * <langs type='current' count='3' mainstream='true'>
 <language flavor='static' version='1.5'>Java</language>
 <language flavor='dynamic' version='1.6.0'>Groovy</language>
 <language flavor='dynamic' version='1.9'>JavaScript</language>
 </langs>
 */
 //根据类动态生成xml文件
StringWriter sw = new StringWriter()
MarkupBuilder mb = new MarkupBuilder(sw)

Langs langs = new Langs(
        type: 'current',count:3,mainstream:true,
        languages: [
                 new Language(flavor: 'static',version:'1.5',value: 'Java'),
                 new Language(flavor: 'dynamic',version:'1.6.0',value: 'Groovy'),
                 new Language(flavor: 'dynamic',version:'1.9',value: 'JavaScript')
        ]
)

mb.langs(type: langs.type,count:langs.count,mainstream:langs.mainstream){
    langs.languages.each { _lang ->
        language(flavor:_lang.flavor,version:_lang.version,_lang.value)
    }
}

println sw.toString()
saveFile(sw.toString(),'../../release.xml')
def saveFile(String source,String desFilePath){
    try {
        File desFile = new File(desFilePath)
        if(!desFile.exists()){
            desFile.createNewFile()
        }
        desFile.withWriter {
            it.write(source)
        }
        return true
    }catch(Exception e){
        return false
    }
}
class Langs {
    String type
    int count
    boolean mainstream
    def languages = []
}
class Language {
    String flavor
    String version
    String value
}
```
查看文件release.xml
```xml
<langs type='current' count='3' mainstream='true'>
  <language flavor='static' version='1.5'>Java</language>
  <language flavor='dynamic' version='1.6.0'>Groovy</language>
  <language flavor='dynamic' version='1.9'>JavaScript</language>
</langs>
```
