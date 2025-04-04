## accessible\base\nsAccessibilityService.cpp
- nsAccessibilityService::Init()
- if (XRE_IsParentProcess()) PlatformInit();
## accessible\windows\msaa\Platform.cpp
- a11y::PlatformInit()
## accessible\windows\msaa\nsWinUtils.cpp
- nsWinUtils::MaybeStartWindowEmulation()
- nsWinUtils::RegisterNativeWindow
## widget\windows\nsWindow.cpp
- nsWindow::Create
- nsWindow::WindowProc
- nsWindow::WindowProcInternal
- nsWindow::ProcessMessage
- nsWindow::ProcessMessageInternal
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

## 窗口消息

- MOZ_HAS_REMOTE
- -no-remote

### nsRemoteService
- StartupServer()
### nsWinRemoteServer
### nsWinRemoteClient
- SendCommandLine
### ipc\glue\WindowsMessageLoop.cpp
- NeuteredWindowRegion
- StartNeutering
```c++
::SetWindowsHookEx(WH_CALLWNDPROC, CallWindowProcedureHook, nullptr, gUIThreadId);
```
- CallWindowProcedureHook
- NeuteredWindowProc
```c++
SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)NeuteredWindowProc);
SetWindowLongPtr(hWnd, GWLP_WNDPROC, currentWndProc);
SetProp(hWnd, kOldWndProcProp, (HANDLE)currentWndProc)
```
- ProcessOrDeferMessage
