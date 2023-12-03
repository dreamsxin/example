# chromium

浏览器引擎

https://github.com/chromium/chromium

## 源码目录结构

https://www.chromium.org/developers/how-tos/getting-around-the-chrome-source-code/

## 修改默认搜索引擎及标签页

### 默认新标签页修改方法：

1. chrome/browser/ui/http://browser_tabstrip.cc
2. chrome/browser/ui/browser.h
3. chrome/browser/ui/http://browser.cc
4. chrome/common/webui_url_constants.h
5. chrome/common/http://webui_url_constants.cc

### 默认搜索引擎修改方法

1. components/search_engines/templates_url_prepopulate_data_cc
2. components/search_engines/prepopulated_engines.json
