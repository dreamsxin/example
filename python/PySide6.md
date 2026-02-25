## 简单浏览器

```python
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWebEngineCore import QWebEnginePage
from PySide6.QtWidgets import QApplication, QMainWindow

class MyBrowser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.view = QWebEngineView()
        self.setCentralWidget(self.view)

        # Connect the printRequested signal
        self.view.page().printRequested.connect(self.handle_print_request)

        self.view.setUrl("https://www.baidu.com") # Load a web page

    def handle_print_request(self, frame):
        print(frame)
        print(f"Print requested from frame: {frame.url().toString()}")
        # Here, you would typically open a print dialog or trigger your printing logic
        # For instance, you could use QPrinter and QPrintDialog to print the content.

if __name__ == "__main__":
    app = QApplication([])
    browser = MyBrowser()
    browser.show()
    app.exec()
```

### 带超时的浏览器

```python
import sys
from PySide6.QtCore import QUrl, QTimer
from PySide6.QtWidgets import QApplication, QMainWindow, QLineEdit, QToolBar, QMessageBox
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWebEngineCore import QWebEnginePage

class CustomWebPage(QWebEnginePage):
    """自定义页面，用于处理证书错误等"""
    def certificateError(self, error):
        # 忽略证书错误（仅用于演示，实际应用请谨慎处理）
        error.acceptCertificate()
        return True

class BrowserWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("简易浏览器")
        self.resize(1024, 768)

        # 创建浏览器视图并设置自定义页面
        self.browser = QWebEngineView()
        self.custom_page = CustomWebPage(self.browser)
        self.browser.setPage(self.custom_page)
        self.setCentralWidget(self.browser)

        # 导航工具栏
        self.navbar = QToolBar("导航")
        self.addToolBar(self.navbar)

        # 后退
        back_btn = self.navbar.addAction("◀")
        back_btn.triggered.connect(self.browser.back)

        # 前进
        forward_btn = self.navbar.addAction("▶")
        forward_btn.triggered.connect(self.browser.forward)

        # 刷新
        reload_btn = self.navbar.addAction("↻")
        reload_btn.triggered.connect(self.browser.reload)

        # 停止
        stop_btn = self.navbar.addAction("✕")
        stop_btn.triggered.connect(self.browser.stop)

        # 地址栏
        self.url_bar = QLineEdit()
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        self.navbar.addWidget(self.url_bar)

        # 连接浏览器信号
        self.browser.urlChanged.connect(self.update_urlbar)
        self.browser.loadStarted.connect(self.on_load_started)
        self.browser.loadFinished.connect(self.on_load_finished)

        # 超时定时器
        self.timeout_timer = QTimer()
        self.timeout_timer.setSingleShot(True)
        self.timeout_timer.timeout.connect(self.on_timeout)

        # 标记超时是否已处理（避免重复提示）
        self.timeout_occurred = False

        # 设置默认主页
        self.browser.setUrl(QUrl("https://www.bing.com"))

    def navigate_to_url(self):
        """导航到地址栏输入的 URL"""
        url = self.url_bar.text()
        if not url.startswith(("http://", "https://")):
            url = "http://" + url
        self.browser.setUrl(QUrl(url))

    def update_urlbar(self, url):
        """更新地址栏"""
        self.url_bar.setText(url.toString())

    def on_load_started(self):
        """开始加载：重置超时标记并启动定时器"""
        self.timeout_occurred = False
        self.timeout_timer.start(10000)  # 10 秒超时

    def on_load_finished(self, ok):
        """加载完成：停止定时器，并根据成功与否处理错误"""
        self.timeout_timer.stop()
        if not ok and not self.timeout_occurred:
            # 如果不是因为超时而停止的加载失败，则弹出通用错误提示
            QMessageBox.critical(self, "加载错误", "页面加载失败，请检查网址或网络连接。")

    def on_timeout(self):
        """超时处理：停止加载，标记超时，并弹出提示"""
        self.timeout_occurred = True
        self.browser.stop()
        QMessageBox.warning(self, "超时", "页面加载超时，请检查网络连接。")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BrowserWindow()
    window.show()
    sys.exit(app.exec())
```

## 判断页面加载状态

通过 `browser.url` 判断状态，还是网络不同，无法访问时，url为 `about:blank`

```python
import sys
import re
from PySide6.QtCore import QUrl, QTimer
from PySide6.QtWidgets import QApplication, QMainWindow, QLineEdit, QToolBar, QMessageBox, QStatusBar
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWebEngineCore import QWebEngineSettings,QWebEnginePage

class CustomWebPage(QWebEnginePage):
    """自定义页面，用于处理证书错误等"""
    def certificateError(self, error):
        # 忽略证书错误（仅用于演示，实际应用请谨慎处理）
        error.acceptCertificate()
        return True

class BrowserWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("简易浏览器")
        self.resize(1024, 768)

        # 创建浏览器视图并设置自定义页面
        self.browser = QWebEngineView()
        
        settings = self.browser.settings()
        settings.setAttribute(QWebEngineSettings.ErrorPageEnabled, False)

        self.custom_page = CustomWebPage(self.browser)
        self.browser.setPage(self.custom_page)
        self.setCentralWidget(self.browser)

        # 导航工具栏
        self.navbar = QToolBar("导航")
        self.addToolBar(self.navbar)

        # 后退
        back_btn = self.navbar.addAction("◀")
        back_btn.triggered.connect(self.browser.back)

        # 前进
        forward_btn = self.navbar.addAction("▶")
        forward_btn.triggered.connect(self.browser.forward)

        # 刷新
        reload_btn = self.navbar.addAction("↻")
        reload_btn.triggered.connect(self.browser.reload)

        # 停止
        stop_btn = self.navbar.addAction("✕")
        stop_btn.triggered.connect(self.browser.stop)

        # 地址栏
        self.url_bar = QLineEdit()
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        self.navbar.addWidget(self.url_bar)

        # 连接浏览器信号
        self.browser.urlChanged.connect(self.update_urlbar)
        self.browser.loadStarted.connect(self.on_load_started)
        self.browser.loadFinished.connect(self.on_load_finished)

        # 超时定时器
        self.timeout_timer = QTimer()
        self.timeout_timer.setSingleShot(True)
        self.timeout_timer.timeout.connect(self.on_timeout)

        # 标记超时是否已处理（避免重复提示）
        self.timeout_occurred = False

        # 设置默认主页
        self.browser.setUrl(QUrl("https://www.bing.com"))

        # 创建状态栏并显示初始消息
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("就绪")

    def navigate_to_url(self):
        """导航到地址栏输入的 URL"""
        url = self.url_bar.text()
        if not url.startswith(("http://", "https://")):
            url = "http://" + url
        self.browser.setUrl(QUrl(url))

    def update_urlbar(self, url):
        """更新地址栏"""
        self.url_bar.setText(url.toString())

    def show_current_url(self):
        url = self.browser.url()
        if url.isEmpty():
            QMessageBox.information(self, "当前 URL", "当前没有加载任何页面")
        else:
            QMessageBox.information(self, "当前 URL", f"当前页面地址：\n{url.toString()}")

    def on_load_started(self):
        """开始加载：重置超时标记、启动定时器、更新状态栏"""
        self.timeout_occurred = False
        self.timeout_timer.start(10000)  # 10 秒超时
        self.status_bar.showMessage("正在加载...")

    def on_load_finished(self, ok):
        """加载完成：停止定时器，根据成功与否更新状态栏和处理错误"""
        self.timeout_timer.stop()
        self.show_current_url()
        if ok:
            self.status_bar.showMessage("完成")
        else:
            if self.timeout_occurred:
                # 超时已经处理，状态栏已在 on_timeout 中更新
                return
            # 加载失败，尝试从页面内容中提取 HTTP 状态码
            self.browser.page().toHtml(self.handle_load_error_html)

    def handle_load_error_html(self, html):
        """分析错误页面的 HTML，尝试提取 HTTP 状态码"""
        # 常见 HTTP 错误模式
        patterns = [
            (r'(\d{3})\s+(?:Not Found|Forbidden|Internal Server Error|Service Unavailable|Bad Request|Unauthorized)', 'HTTP 错误'),
            (r'<title>.*?(\d{3}).*?</title>', '标题中包含状态码'),
            (r'HTTP[^\d]*(\d{3})', '直接匹配 HTTP 状态码')
        ]
        status_code = None
        for pattern, desc in patterns:
            match = re.search(pattern, html, re.IGNORECASE)
            if match:
                status_code = match.group(1)
                break

        if status_code:
            msg = f"加载失败 (HTTP {status_code})"
            self.status_bar.showMessage(msg)
            QMessageBox.critical(self, "加载错误", f"页面返回 HTTP 状态码 {status_code}")
        else:
            self.status_bar.showMessage("加载失败")
            QMessageBox.critical(self, "加载错误", "页面加载失败，请检查网址或网络连接。")

    def on_timeout(self):
        """超时处理：停止加载，标记超时，更新状态栏并弹出提示"""
        self.timeout_occurred = True
        self.browser.stop()
        self.status_bar.showMessage("超时")
        QMessageBox.warning(self, "超时", "页面加载超时，请检查网络连接。")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BrowserWindow()
    window.show()
    sys.exit(app.exec())
```

## 加载状态

```python
import sys
import re
from PySide6.QtCore import QUrl, QTimer
from PySide6.QtWidgets import QApplication, QMainWindow, QLineEdit, QToolBar, QMessageBox, QStatusBar
from PySide6.QtWebEngineWidgets import QWebEngineView
from PySide6.QtWebEngineCore import QWebEngineSettings,QWebEnginePage

class CustomWebPage(QWebEnginePage):
    """自定义页面，用于处理证书错误等"""
    def certificateError(self, error):
        # 忽略证书错误（仅用于演示，实际应用请谨慎处理）
        error.acceptCertificate()
        return True

class BrowserWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("简易浏览器")
        self.resize(1024, 768)

        # 创建浏览器视图并设置自定义页面
        self.browser = QWebEngineView()
        
        settings = self.browser.settings()
        settings.setAttribute(QWebEngineSettings.ErrorPageEnabled, False)

        self.custom_page = CustomWebPage(self.browser)
        self.browser.setPage(self.custom_page)
        self.setCentralWidget(self.browser)

        # 导航工具栏
        self.navbar = QToolBar("导航")
        self.addToolBar(self.navbar)

        # 后退
        back_btn = self.navbar.addAction("◀")
        back_btn.triggered.connect(self.browser.back)

        # 前进
        forward_btn = self.navbar.addAction("▶")
        forward_btn.triggered.connect(self.browser.forward)

        # 刷新
        reload_btn = self.navbar.addAction("↻")
        reload_btn.triggered.connect(self.browser.reload)

        # 停止
        stop_btn = self.navbar.addAction("✕")
        stop_btn.triggered.connect(self.browser.stop)

        # 地址栏
        self.url_bar = QLineEdit()
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        self.navbar.addWidget(self.url_bar)

        # 连接浏览器信号
        self.browser.urlChanged.connect(self.update_urlbar)
        self.browser.loadStarted.connect(self.on_load_started)
        self.browser.loadFinished.connect(self.on_load_finished)
        self.browser.page().loadingChanged.connect(self.on_loading_changed)

        # 超时定时器
        self.timeout_timer = QTimer()
        self.timeout_timer.setSingleShot(True)
        self.timeout_timer.timeout.connect(self.on_timeout)

        # 标记超时是否已处理（避免重复提示）
        self.timeout_occurred = False

        # 设置默认主页
        self.browser.setUrl(QUrl("https://www.bing.com"))

        # 创建状态栏并显示初始消息
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.status_bar.showMessage("就绪")

    def navigate_to_url(self):
        """导航到地址栏输入的 URL"""
        url = self.url_bar.text()
        if not url.startswith(("http://", "https://")):
            url = "http://" + url
        self.browser.setUrl(QUrl(url))

    def update_urlbar(self, url):
        """更新地址栏"""
        self.url_bar.setText(url.toString())

    def show_current_url(self):
        url = self.browser.url()
        if url.isEmpty():
            QMessageBox.information(self, "当前 URL", "当前没有加载任何页面")
        else:
            QMessageBox.information(self, "当前 URL", f"当前页面地址：\n{url.toString()}")

    def on_load_started(self):
        """开始加载：重置超时标记、启动定时器、更新状态栏"""
        self.timeout_occurred = False
        self.timeout_timer.start(10000)  # 10 秒超时
        self.status_bar.showMessage("正在加载...")

    def on_loading_changed(self, loading_info):
        # Retrieve status code from the loading info
        # Note: Available if ErrorDomain is HttpStatusCodeDomain
        print(loading_info)
        status_code = loading_info.errorDomain() 
        print(f"Loaded {loading_info.url().toString()} with status: {status_code} {loading_info.status()}")

    def on_load_finished(self, ok):
        """加载完成：停止定时器，根据成功与否更新状态栏和处理错误"""
        self.timeout_timer.stop()
        self.show_current_url()
        if ok:
            self.status_bar.showMessage("完成")
        else:
            if self.timeout_occurred:
                # 超时已经处理，状态栏已在 on_timeout 中更新
                return
            # 加载失败，尝试从页面内容中提取 HTTP 状态码
            self.browser.page().toHtml(self.handle_load_error_html)

    def handle_load_error_html(self, html):
        """分析错误页面的 HTML，尝试提取 HTTP 状态码"""
        # 常见 HTTP 错误模式
        patterns = [
            (r'(\d{3})\s+(?:Not Found|Forbidden|Internal Server Error|Service Unavailable|Bad Request|Unauthorized)', 'HTTP 错误'),
            (r'<title>.*?(\d{3}).*?</title>', '标题中包含状态码'),
            (r'HTTP[^\d]*(\d{3})', '直接匹配 HTTP 状态码')
        ]
        status_code = None
        for pattern, desc in patterns:
            match = re.search(pattern, html, re.IGNORECASE)
            if match:
                status_code = match.group(1)
                break

        if status_code:
            msg = f"加载失败 (HTTP {status_code})"
            self.status_bar.showMessage(msg)
            QMessageBox.critical(self, "加载错误", f"页面返回 HTTP 状态码 {status_code}")
        else:
            self.status_bar.showMessage("加载失败")
            QMessageBox.critical(self, "加载错误", "页面加载失败，请检查网址或网络连接。")

    def on_timeout(self):
        """超时处理：停止加载，标记超时，更新状态栏并弹出提示"""
        self.timeout_occurred = True
        self.browser.stop()
        self.status_bar.showMessage("超时")
        QMessageBox.warning(self, "超时", "页面加载超时，请检查网络连接。")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BrowserWindow()
    window.show()
    sys.exit(app.exec())
```
