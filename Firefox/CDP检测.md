
## `remote.active-protocols`
定义远程协议。
- WebDriver BiDi（1）
- CDP（`2`）。
同时支持两个协议设置为`3`。自 Firefox 129 以来默认为 `1`（WebDriver BiDi）。

```js
  // Defines the protocols that will be active for the Remote Agent.
  // 1: WebDriver BiDi
  // 2: CDP (Chrome DevTools Protocol)
  // 3: WebDriver BiDi + CDP
  pref("remote.active-protocols", 1);
```
## 检测
```html
<!DOCTYPE html>
<html>

<head>
	<title>Detect Chrome DevTools Protocol</title>
	<script>
		function genNum(e) {
			return 1000 * e.Math.random() | 0;
		}
		function catchCDP(e) {
			if (e.chrome) {
				var rng1 = 0;
				var rng2 = 1;
				var acc = rng1;
				var result = false;
				try {
					var errObj = new e.Error();
					var propertyDesc = {
						configurable: false,
						enumerable: false,
						get: function () {
							acc += rng2;
							return '';
						}
					};
					Object.defineProperty(errObj, "stack", propertyDesc);
					console.debug(errObj);
					errObj.stack;
					if (rng1 + rng2 != acc) {
						result = true;
					}
				} catch {

				}
				return result;
			}
		}
		function isCDPOn() {
			if(!window)
				return;
			const el = document.querySelector('span#status');
			if(!el)
				return;
			el.innerText = catchCDP(window) ? "yes":"no";
		}
		function init() {
			isCDPOn();
			setInterval(isCDPOn, 100);
		}
		document.addEventListener("DOMContentLoaded", init);
	</script>
</head>

<body>
	<p>CDP Detected: <span id="status">-</span></p>
</body>

</html>
```
