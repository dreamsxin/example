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
