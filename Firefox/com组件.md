
## toolkit\xre\moz.build

```build
XPCOM_MANIFESTS += [
    "components.conf",
]
```

## toolkit\xre\components.conf
```conf
Classes = [
    {
        'name': 'XULRuntime',
        'js_name': 'appinfo',
        'cid': '{95d89e3e-a169-41a3-8e56-719978e15b12}',
        'contract_ids': [
            '@mozilla.org/xre/app-info;1',
            '@mozilla.org/xre/runtime;1',
        ] + crash_reporter,
        'interfaces': ['nsIXULRuntime', 'nsIXULAppInfo', 'nsICrashReporter'],
        'legacy_constructor': 'mozilla::AppInfoConstructor',
        'headers': ['nsAppRunner.h'],
        'processes': ProcessSelector.ALLOW_IN_SOCKET_PROCESS,
        'overridable': True,
    },
    {
        'cid': '{471f4944-1dd2-11b2-87ac-90be0a51d609}',
        'contract_ids': ['@mozilla.org/embedcomp/rangefind;1'],
        'type': 'nsFind',
        'headers': ['/toolkit/components/find/nsFind.h'],
    },
    {
        'cid': '{7e677795-c582-4cd1-9e8d-8271b3474d2a}',
        'contract_ids': ['@mozilla.org/embedding/browser/nsWebBrowserPersist;1'],
        'type': 'nsWebBrowserPersist',
        'headers': ['/dom/webbrowserpersist/nsWebBrowserPersist.h'],
    },
    {
        'name': 'WindowWatcher',
        'js_name': 'ww',
        'cid': '{a21bfa01-f349-4394-a84c-8de5cf0737d0}',
        'contract_ids': ['@mozilla.org/embedcomp/window-watcher;1'],
        'interfaces': ['nsIWindowWatcher'],
        'type': 'nsWindowWatcher',
        'headers': ['nsWindowWatcher.h'],
        'init_method': 'Init',
        'overridable': True,
    },
    {
        'cid': '{5573967d-f6cf-4c63-8e0e-9ac06e04d62b}',
        'contract_ids': ['@mozilla.org/xre/directory-provider;1'],
        'singleton': True,
        'type': 'nsXREDirProvider',
        'constructor': 'nsXREDirProvider::GetSingleton',
        'headers': ['/toolkit/xre/nsXREDirProvider.h'],
    },
    {
        'cid': '{4e4aae11-8901-46cc-8217-dad7c5415873}',
        'contract_ids': ['@mozilla.org/embedcomp/dialogparam;1'],
        'type': 'nsDialogParamBlock',
        'headers': ['/toolkit/components/windowwatcher/nsDialogParamBlock.h'],
    },
    {
        'cid': '{5f5e59ce-27bc-47eb-9d1f-b09ca9049836}',
        'contract_ids': ['@mozilla.org/toolkit/profile-service;1'],
        'singleton': True,
        'constructor': 'NS_GetToolkitProfileService',
        'type': 'nsToolkitProfileService',
        'headers': ['/toolkit/profile/nsToolkitProfileService.h'],
    },
]
```
