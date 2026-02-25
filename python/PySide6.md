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
