
https://firefox-source-docs.mozilla.org/ipc/ipdl.html

## IPDL

进程间通信协议定义语言（IPDL）是 Mozilla 特有的语言，用于定义C++代码中进程间的信息传递机制。
在Firefox中管理父进程和子进程间通信的主要协议是Content协议。正如你在其名为 `PContent.ipdl` 的源码中所看到的，该定义相当大，包括了很多其他的协议—因为在Content中有很多子协议，所以整个协议定义是分层的。
在每个协议里面，都有启动每个子协议的方法，并提供对新启动的子协议里面的方法的访问方式。对于自动化测试，这种结构也意味着没有一个包含所有有趣方法的平坦攻击面。相反，它可能需要多次消息往返才能到达某个子协议，然后可能会暴露一个漏洞。
除了 `PContent.ipdl` 之外，Firefox 还使用 `PBackground.ipdl`，它不包括在Content中，但允许后台通信，因此提供了另一种在C++进程之间传递信息的机制。

##　JS　Actors

在　JavaScript　中执行IPC的首选方式是基于 `JS Actors`，其中 JSProcessActor 提供了一个子进程和其父进程之间的通信通道，JSWindowActor提供了一个框架和其父框架之间的通信通道。在注册了一个新的JS行为体后，它提供了两种方法来发送消息。

- sendAsyncMessage()
- sendQuery()，以及一个接收消息的方法：receiveMessage()。

## 实例

`MyTypes.ipdlh`
```ipdlh
include protocol PMyManaged;

struct MyActorPair {
    PMyManaged actor1;
    nullable PMyManaged actor2;
};
```

`PMyManager.ipdl`
```ipdl
include protocol PMyManaged;
include MyTypes;                          // for MyActorPair

using MyActorEnum from "mozilla/myns/MyActorUtils.h";
using mozilla::myns::MyData from "mozilla/MyDataTypes.h";
[MoveOnly] using class mozilla::myns::MyOtherData from "mozilla/MyDataTypes.h";
[RefCounted] using class mozilla::myns::MyThirdData from "mozilla/MyDataTypes.h";

namespace mozilla {
namespace myns {

[Comparable] union MyUnion {
    float; 
    MyOtherData;
};

[ChildProc=any]
sync protocol PMyManager {
  manages PMyManaged;
  parent:
    async __delete__(nsString aNote);
    sync SomeMsg(MyActorPair? aActors, MyData[] aMyData)
        returns (int32_t x, int32_t y, MyUnion aUnion);
    async PMyManaged();
  both:
    [Tainted] async AnotherMsg(MyActorEnum aEnum, int32_t aNumber)
        returns (MyOtherData aOtherData);
};

}    // namespace myns
}    // namespace mozilla
```
`PMyManaged.ipdl`
```ipdl
include protocol PMyManager;

namespace mozilla {
namespace myns {

protocol PMyManaged {
  manager PMyManager;
  child:
    async __delete__(Shmem aShmem);
};

}    // namespace myns
}    // namespace mozilla
```

`moz.build`
```build
IPDL_SOURCES += [
    "MyTypes.ipdlh",
    "PMyManaged.ipdl",
    "PMyManager.ipdl",
]

UNIFIED_SOURCES += [
    "MyManagedChild.cpp",
    "MyManagedParent.cpp",
    "MyManagerChild.cpp",
    "MyManagerParent.cpp",
]

include("/ipc/chromium/chromium-config.mozbuild")
```
