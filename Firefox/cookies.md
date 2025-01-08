## `netwerk/cookie`

## tools

https://github.com/KingOfTheNOPs/cookie-monster

## browser\components\preferences\dialogs\siteDataSettings.js
- init()
```js
    setEventListener("sitesList", "select", this.onSelect);
    setEventListener("hostCol", "click", this.onClickTreeCol);
    setEventListener("usageCol", "click", this.onClickTreeCol);
    setEventListener("lastAccessedCol", "click", this.onClickTreeCol);
    setEventListener("cookiesCol", "click", this.onClickTreeCol);
    setEventListener("searchBox", "command", this.onCommandSearch);
    setEventListener("removeAll", "command", this.onClickRemoveAll);
    setEventListener("removeSelected", "command", this.removeSelected);
```
- onClickRemoveAll()

## browser\modules\SiteDataManager.sys.mjs
- removeAll()
- removeSiteData()
