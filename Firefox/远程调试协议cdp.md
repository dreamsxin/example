#

## 调用流程

- new ContentProcessSession
- new lazy.DomainCache
- DomainCache.execute
- DomainCache.domainSupportsMethod

## 协议实现

- remote\cdp\Protocol.sys.mjs
- remote\cdp\domains\parent\Browser.sys.mjs
- remote\cdp\domains\parent\Target.sys.mjs
