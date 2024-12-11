## widget\windows\nsWindow.cpp
- nsWindow::Create
## widget\windows\WinEventObserver.cpp
```c++
bool WinEventHub::Initialize() {
  WNDCLASSW wc;
  HMODULE hSelf = ::GetModuleHandle(nullptr);

  if (!GetClassInfoW(hSelf, L"MozillaWinEventHubClass", &wc)) {
    ZeroMemory(&wc, sizeof(WNDCLASSW));
    wc.hInstance = hSelf;
    wc.lpfnWndProc = WinEventProc;
    wc.lpszClassName = L"MozillaWinEventHubClass";
    RegisterClassW(&wc);
  }

  mHWnd = ::CreateWindowW(L"MozillaWinEventHubClass", L"WinEventHub", 0, 0, 0,
                          0, 0, nullptr, nullptr, hSelf, nullptr);
  if (!mHWnd) {
    return false;
  }

  return true;
}
```
