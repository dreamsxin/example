- https://github.com/Kaliiiiiiiiii-Vinyzu/patchright/blob/main/patchright.patch
- https://github.com/abrahamjuliot/creepjs
- https://github.com/rebrowser/rebrowser-bot-detector
- https://github.com/TheGP/untidetect-tools
- https://github.com/Granitosaurus/playwright-stealth/blob/main/playwright_stealth/stealth.py

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta
      name="viewport"
      content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0"
    />
    <meta http-equiv="x-ua-compatible" content="ie=edge" />
    <meta name="keywords" content="https://github.com/dreamsxin" />
    <meta name="description" content="https://github.com/dreamsxin" />
    <title>test测试</title>
  </head>
  <body>
    <script>
      // 通过大数据量测试控制台性能
      console.table(new Array(1000).fill({data: "test"}));

      //'clear','8620759QzHgOr','outerWidth','max','removeChild','1817050qRqjmz','concat','indexOf','addEventListener','pointerType','contextmenu','valueOf','call','704787zYDoIZ','push','keyCode','console','outerHeight','fromCharCode','cut','parent','异常2','table','apply','ctrlKey','1364xzCCFd','innerHTML','preventDefault','86280sJjBAe','now','pop','returnValue','createElement','undefined','macintosh','disableSelect','935832oxBPWm','6cbaMHL','document','6208749pXYgho','native\x20code','return\x20this','userAgent','innerWidth','return\x20new\x20F(','keydown','disableCut','异常3','innerHeight','selectstart','839750npLXMw','body','8YdLqaT','shiftKey','F12','bind','toLowerCase','disableMenu','join','which','disablePaste','copy','altKey','constructor','length','F,a','prototype','paste','metaKey','appendChild','debugger','6uIIIZJ','log','toString','event','slice'

      document.write("navigator.webdriver:" + navigator.webdriver + "<br>\n");
      console.log(navigator.webdriver);
      Object.defineProperty(navigator, "webdriver", {
        get: () => undefined,
      });
      document.write("navigator.webdriver:" + navigator.webdriver + "<br>\n");

      document.write(
        "navigator.webdriver:" +
          Object.getOwnPropertyDescriptor(
            Navigator.prototype,
            "webdriver"
          ).get.apply(navigator) +
          "<br>\n"
      );

      /* alert(Function.prototype.bind.toString());
alert(Function.prototype.toString.call(setTimeout));
alert(navigator.userAgent.toLowerCase());
alert(document.createElement('script')) */

navigator.webdriver; // true if webdriver controlled, false otherwise
// this lazy patch is commonly found on the internet, it does not even set the right value
Object.defineProperty(navigator, 'webdriver', {
  get: () => undefined
});
navigator.webdriver; // undefined
Object.getOwnPropertyDescriptor(Navigator.prototype, 'webdriver').get.apply(navigator);
// true

      if (window.top !== window.self) {
        alert("运行在iframe中");
      }

      // 检测setInterval是否原生
      if (
        Function.prototype.toString.call(setInterval).indexOf("native code") ===
        -1
      ) {
        alert("函数被重写!");
      }

      // 检测控制台是否被重写
      if (
        Function.prototype.toString.call(console.log).indexOf("native code") ===
        -1
      ) {
        alert("控制台方法被篡改!");
      }

      // 创建并移除script元素检测DOM操作
      const script = document.createElement("script");
      script.innerHTML = "debugger;";
      document.body.appendChild(script);
      script.remove();

      // 检测窗口尺寸异常（开发者工具通常会影响尺寸）
      if (
        window.outerWidth - window.innerWidth > 200 ||
        window.outerHeight - window.innerHeight > 200
      ) {
        alert("窗口尺寸异常!");
      }

      // 开发者工具检测
      const detectDevTools = () => {
        const threshold = 100;
        const start = performance.now();
        debugger;
        const diff = performance.now() - start;
        if (diff > threshold) {
          alert("调试模式检测!");
        }
      };

      detectDevTools();

      var detected = false;
      var e = new Error();
      Object.defineProperty(e, "stack", {
        get() {
          detected = true;
          alert(1);
          document.write("detected true<br>\n");
        },
      });

      document.write("start detected clear<br>\n");
      console.clear(e);
      document.write("start detected log<br>\n");
      console.log(e);
      document.write("start detected table<br>\n");
      console.table(e);

      alert("正常");
    </script>
  </body>
</html>
```


```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta
      name="viewport"
      content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0"
    />
    <meta http-equiv="x-ua-compatible" content="ie=edge" />
    <title>test测试</title>
    <meta name="keywords" content="https://github.com/dreamsxin" />
    <meta name="description" content="https://github.com/dreamsxin" />
  </head>
  <body>
    <script>
      // console.debug = console.log = ()=>{}
      // test:
      console.info(
        !!("" + window.console.debug).match(
          /[\)\( ]{3}[>= ]{3}\{\n[ r]{9}etu[n r]{3}n[lu]{3}/
        )
      );

      var counter = 0;
      var e = new Error();
      Object.defineProperty(e, "stack", {
        get() {
          counter++;
        },
      });
      console.log(e);
      console.info("counter: ", counter);

      var detected = false;
      var e = new Error();
      Object.defineProperty(e, "stack", {
        get() {
          detected = true;
          document.write("detected true<br>\n");
        },
      });

      document.write("start detected log<br>\n");
      console.log(e);
      document.write("start detected table<br>\n");
      console.table(e);
      //document.write("start detected clear<br>\n");
      //console.clear(e);
    </script>
  </body>
</html>
```
