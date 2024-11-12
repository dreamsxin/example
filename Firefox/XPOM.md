## XPCOM的
目标是让不同开发者独立开发实现相应的软件片段（组件）。 为了解决应用程序里各组件之间的互连问题，XPCOM把组件的实现和接口分离开来（相关主题在 Interface[https://developer.mozilla.org/en-US/docs/Creating_XPCOM_Components/An_Overview_of_XPCOM#Interfaces] 章节讨论），XPCOM也提供了几个工具和库，以便加载和维护这些组件，服务，帮助开发者写模块化的跨平台的代码和版本支持，因此组件可以进行不中断替换 和升级。 使用XPCOM，开发者创建的组件可以在不同的应用程序里重用。

XPCOM 不仅仅支持组件化软件开发，他也提供了很多开发平台提供的功能，例如：

- 组件管理 ( component management )
- 抽象文件 ( file abstraction )
- 对象消息传输 ( object message passing )
- 内存管理 ( Memory management )

## `xpcom\components\nsServiceManagerUtils.h`
## `dist\xpcom\components\StaticComponents.cpp`
此文件构建时会生成，生成脚本  `xpcom\components\gen_static_components.py`
```c++
const StaticModule gStaticModules[]
```

## nsISupports 基接口
在基于接口和组件的编程中有两个最基本的问题：组件的生命周期(也叫做对象所有权)和接口查询，或者说能够在运行时获得组件支持的接口。

### 对象所有者
在 XPCOM 里，组件可能实现任意数量的接口，组件需要对接口引用进行计数。当一个组件创建时，就会对其接口被客户使用的情况进行计数（就是大家知道的引用计数功能）。当客户实例化组件时，引用计数就会自动增长；然后在组件的整个生命周期里，引用计数或增或减。 当所有客户不再使用组件，引用计数就会变为0，组件就会删除自己。


## XPCOM 里的指针

XPCOM 里，指针指的是接口指针。 接口指针和常规指针都是内存里的地址，他们之间有点细微的不同。 一个接口指针必须实现 `nsISupports` 基接口，所以可以调用 `AddRef`,`Release`和`QueryInterface`方法。

**一个简单的实现**
接口定义文件：`Sample.idl`

```c++
class Sample : public nsISupports
{
private:
    nsrefcnt mRefCnt;

public:
    Sample();
    virtual ~Sample();

    NS_IMETHOD QueryInterface(const nsIID &aIID, void **aResult);
    NS_IMETHOD_(nsrefcnt)
    AddRef(void);
    NS_IMETHOD_(nsrefcnt)
    Release(void);
};
```
```c++
// initialize the reference count to 0
Sample::Sample() : mRefCnt(0)
{
}
Sample::~Sample()
{
}

// typical, generic implementation of QI
NS_IMETHODIMP Sample::QueryInterface(const nsIID &aIID,
                                     void **aResult)
{
    if (!aResult)
    {
        return NS_ERROR_NULL_POINTER;
    }
    *aResult = NULL;
    if (aIID.Equals(kISupportsIID))
    {
        *aResult = (void *)this;
    }
    if (!*aResult)
    {
        return NS_ERROR_NO_INTERFACE;
    }
    // add a reference
    AddRef();
    return NS_OK;
}

NS_IMETHODIMP_(nsrefcnt) Sample::AddRef()
{
    return ++mRefCnt;
}

NS_IMETHODIMP_(nsrefcnt) Sample::Release()
{
    if (--mRefCnt == 0)
    {
        delete this;
        return 0;
    }
    // optional: return the reference count
    return mRefCnt;
}
```

## 对象接口发现

在 XPCOM，所有接口类从 `nsISupports` 类派生，因此所有的对象都是 `nsISupports`，但他们还是其他类，拥有更多的特色的类（你需要在 运行时找出此特别的类）。
在 XPCOM 里，通过 `nsISupports` 成员函数 `QueryInterface` 函数，我们可以发现它提供的其他不同的接口。
`QueryInterface` 方法的第一个参数 `nsIID` 是对 `IID` 的一个封装。`IID` 包含了3个方法，`Equals`，`Parse` 和 `ToString.Equals`。

当你实现nsISupports类时，你必须要确保QueryInterface返回一个有效值。QueryInterface因该支持组件支持的所有接口。

在实现QueryInterface函数时，IID参数会被检查。 如果匹配，则返回对象的指针（转换为void）,并增加引用计数。 不匹配，类就返回一个错误码，指针为NULL。

继承是面向对象语言的一个重要特点。 当一个类从另外一个类继承时，这个类就可以重载基类函数的缺省行为，这对于创建更加特别的类是很有效的，如下面所示，Circle 继承自 Shape 类：
```c++
class Shape
{
private:
    int m_x;
    int m_y;

public:
    virtual void Draw() = 0;
    Shape();
    virtual ~Shape();
};

class Circle : public Shape
{
private:
    int m_radius;

public:
    virtual Draw();
    Circle(int x, int y, int radius);
    virtual ~Circle();
};
```
在 C++ 里，你可以使用 `dynamic_cast<>` 将一个基类对象指针（或引用）转换到继承类指针，如果 Shape 对象不能转换为 Circle，他会抛出异常。 但是开启exceptions和RTTI会带来性能上问题，并且还需要考虑在不同平台上的兼容性，因此XPCOM没有这么做。

## XPCOM里的exception
XPCOM 不直接支持 C++ exceptions。 在组件内产生的异常必须在传递到接口边界之前全部由组件自己处理掉。在 XPCOM 里所有的接口方法应该返回一个 `nsresult` 值，记录错误代码。 有错误代码返回，表示 XPCOM 处理出现异常。

替代C++的RTTI，XPCOM使用了特别的QueryInterface方法，他会把对象转换为相应的接口，前提是支持这个接口。

每一个接口需要被分配一个唯一的标识符（可以通过uuidgen工具生成）。 这个标识符是唯一的，128-bit的数字。 在接口上下文中进行使用，他通常被叫做IID。

当一个客户想发现对象支持的接口时，客户把接口的 `IID`，传递给对象的 `QueryInterface` 方法。 如果对象支持请求的接口，它会返回指向接口的指针，并递增相应的引用计数。 不支持，则返回错误码。

## XPCOM Identifiers
除了接口的`IID`指向具体的接口，XPCOM 还使用了两个非常重要的标识来区别类和组件。
- `nsIID` 类实际上是nsID类的typedef。 nsID的其他的typedef形式还包括，CID和IID，他们用来分别表示具体类的实现和指定的接口。
- `nsID` 类提供了与Equals相似的方法，用于比较标识符。

 ### CID
 一个CID是一个128位的数字，用于标识一个类或组件。 nsISupports的CID看起来如下面样式：`00000000-0000-0000-c000-000000000046`，一般定义如下：
```cpp
#define SAMPLE_CID \  
{ 0x777f7150, 0x4a2b, 0x4301, \  
{ 0xad, 0x10, 0x5e, 0xab, 0x25, 0xb3, 0x22, 0xaa}}
```
或者使用宏申明了一个CID常量：
`static NS_DEFINE_CID(kWebShellCID, NS_WEB_SHELL_CID);`

### Contract ID
契约标识符是一个人可识别的字符串，用于组件的访问。 可以通过CID或契约ID从组件管理器获取组件。 下面是LDAP操作组件的契约ID：
`"@mozilla.org/network/ldap-operation;1"`
契约ID的格式由组件域，模块，组件名字和版本号组成，各部分之间用"/"分隔。
象 CID 一样，契约ID涉及到接口的实现，就是IID干的事。但是契约ID并不和某一具体的实现绑定，和CID一样。一个契约ID只是表示他想实现的一组接口。 契约ID和CID不同之处在于，它可能重载组件（某一组件的不同实现版本）。

## 工厂
在 XPCOM 里工厂由 `nsIFactory` 接口实现，它使用上面所示的工厂设计模式封装对象的创建和初始化。
当代码分割为一个一个组件，客户端代码通常使用 new操作来实例化一个对象：
`SomeClass* component = new SomeClass();`
这种方式需要客户端知道组件的细节，至少要知道他有多大。 工厂设计模式用于封装对象的构建。 工厂的目标是不需要把对象实现的细节和对象初始化流程暴漏给客户端。 在SomeClass例子中，SomeClass的构造和初始化函数，实现了SomeInterface抽象类，在工厂设计模式中，它被包含在了 New_SomeInterface函数里：
```c++
int New_SomeInterface(SomeInterface **ret)
{
    // create the object
    SomeClass *out = new SomeClass();
    if (!out)
        return -1;

    // init the object
    if (out->Init() == FALSE)
    {
        delete out;
        return -1;
    }

    // cast to the interface
    *ret = static_cast<SomeInterface *>(out);
    return 0;
}
```

## XPIDL 和 Type Libraries
如何定义接口，让其可以跨平台，语言，本地开发环境？ 使用IDL(Interface define language)是一种简单而有效的方式。 XPCOM使用基于CORBA OMG规范的变体接口定义语言，XPIDL，可以通过它指定接口的方法，属性，常量，甚至可以定义接口的继承。
使用 XPIDL 来定义你的接口时，需要注意一些缺陷。 它不支持多重继承。 如果你定义一个新的接口，它不能继承自多个接口，只能是一个。 另一个针对XPIDL定义接口的限制是，他的方法名称必须不一致。你可能有两个方法有相同的函数名，但是参数不一样，在XPIDL里一个变通方案就是使用 不同的函数名。
XPIDL 允许你生成类型库(typelibs)，后缀名为`.xpt`的文件。 类型库文件是接口的二进制表现形式。 它为非C++的其它语言提供了访问接口的能力。 当组件被其它语言访问时，他们可以使用二进制类型库访问接口，知道支持什么方法，和怎么调用。
XPCOM这方面的能力叫做XPConnect。 XPConnect是XPCOM的一层，为其他语言提供了访问XPCOM组件的能力，例如，javascript语言。

## XPCOM Services
当客户端通常在需要使用组件提供的功能时才实例化一个新的对象。 但是有一种类型的对象叫做服务，他们总是只有一个拷贝。 每次客户端需要访问服务提供的功能时，它访问的都是服务的同一个实例。 提供功能的单点访问属于单例设计模式范畴。

在XPCOM，除了组件的支持和管理外，还提供了大量的服务帮助开发者写跨平台的组件。 这些服务包括一个跨平台的文件抽象，它提供了统一和强大的访问文件，目录服务，以维持应用程序的位置和系统特定的位置；一个内存管理，以确保每个使用这都 使用相同的内存分配器；一个事件通知系统，允许进行简单的事件传输。

## XPCOM 宏

### `xpcom\base\nsISupportsImpl.h`

| 宏 | 说明 |
|--------------------|-----------------------------------|
| NS_IMPL_ISUPPORTS | 为给出的类实现nsISupports 接口，并可提供数量为n的接口 |
| NS_DECL_ISUPPORTS  | 申明nsISuppports接口的方法，包含了nRefCnt变量。 |
| NS_INIT_ISUPPORTS  | 初始化nRefCnt为0，必须在类的构造函数里调用。        |
| NS_GET_IID         | 返回给定名称的接口的IID。 接口必须有XPIDL生成。      |
| NS_IMPL_QUERY_INTERFACE |       |

- `mozilla::detail::kImplementedIID`
```c++

namespace mozilla::detail {

// Helper which is roughly equivalent to NS_GET_IID, which also performs static
// assertions that `Class` is allowed to implement the given XPCOM interface.
//
// These assertions are done like this to allow them to be used within the
// `NS_INTERFACE_TABLE_ENTRY` macro, though they are also used in
// `NS_IMPL_QUERY_BODY`.
template <typename Class, typename Interface>
constexpr const nsIID& GetImplementedIID() {
  if constexpr (mozilla::detail::InterfaceNeedsThreadSafeRefCnt<
                    Interface>::value) {
    static_assert(Class::HasThreadSafeRefCnt::value,
                  "Cannot implement a threadsafe interface with "
                  "non-threadsafe refcounting!");
  }
  return NS_GET_TEMPLATE_IID(Interface);
}

template <typename Class, typename Interface>
constexpr const nsIID& kImplementedIID = GetImplementedIID<Class, Interface>();

}
```

每一个XPCOM对象都要实现 nsISupports接口，但是一遍又一遍的写相似的实现代码真的很烦。
```c++
/**
 * Convenience macros for implementing all nsISupports methods for
 * a simple class.
 * @param _class The name of the class implementing the method
 * @param _classiiddef The name of the #define symbol that defines the IID
 * for the class (e.g. NS_ISUPPORTS_IID)
 */

#define NS_IMPL_ISUPPORTS0(_class) \
  NS_IMPL_ADDREF(_class)           \
  NS_IMPL_RELEASE(_class)          \
  NS_IMPL_QUERY_INTERFACE0(_class)

#define NS_IMPL_ISUPPORTS(aClass, ...) \
  NS_IMPL_ADDREF(aClass)               \
  NS_IMPL_RELEASE(aClass)              \
  NS_IMPL_QUERY_INTERFACE(aClass, __VA_ARGS__)
```
它使用的是接口的名称，而不是接口的 IID 。 在宏实现里，使用了 NS_GET_ID() 宏，从名称获取接口的 IID 。
```c++
#define NS_GET_IID(T) (T::COMTypeInfo<T, void>::kIID)
#define NS_GET_TEMPLATE_IID(T) (T::template COMTypeInfo<T, void>::kIID)
```
