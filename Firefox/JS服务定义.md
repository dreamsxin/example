## 
- dom\system\IOUtils.cpp
## dom\chrome-webidl
- ChromeUtils.webidl
- IOUtils.webidl
- PathUtils.webidl

## tools\ts\build_services.js
生成 `lib.gecko.services.d.ts`
```js
function main(lib_dts, services_json) {
  let interfaces = JSON.parse(fs.readFileSync(services_json, "utf8"));

  for (let [iface, svc] of Object.entries(interfaces)) {
    SERVICES[svc] = SERVICES[svc] ?? [];
    SERVICES[svc].push(iface);
  }

  let lines = [HEADER];
  lines.push("interface JSServices {");
  for (let [svc, ifaces] of Object.entries(SERVICES).sort()) {
    lines.push(`  ${svc}: ${ifaces.join(" & ")};`);
  }
  lines.push("}\n");

  let dts = lines.join("\n");
  console.log(`[INFO] ${lib_dts} (${dts.length.toLocaleString()} bytes)`);
  fs.writeFileSync(lib_dts, dts);
}
```

## tools\@types\lib.gecko.services.d.ts
```ts
interface JSServices {
  DAPTelemetry: nsIDAPTelemetry;
  appShell: nsIAppShellService;
  appinfo: nsICrashReporter & nsIXULAppInfo & nsIXULRuntime;
  blocklist: nsIBlocklistService;
  cache2: nsICacheStorageService;
  catMan: nsICategoryManager;
  clearData: nsIClearDataService;
  clipboard: nsIClipboard;
  console: nsIConsoleService;
  cookieBanners: nsICookieBannerService;
  cookies: nsICookieManager & nsICookieService;
  cpmm: ContentProcessMessageManager;
  dirsvc: nsIDirectoryService & nsIProperties;
  dns: nsIDNSService;
  domStorageManager: nsIDOMStorageManager & nsILocalStorageManager;
  droppedLinkHandler: nsIDroppedLinkHandler;
  eTLD: nsIEffectiveTLDService;
  els: nsIEventListenerService;
  env: nsIEnvironment;
  focus: nsIFocusManager;
  fog: nsIFOG;
  intl: mozIMozIntl;
  io: nsIIOService & nsINetUtil & nsISpeculativeConnect;
  loadContextInfo: nsILoadContextInfoFactory;
  locale: mozILocaleService;
  logins: nsILoginManager;
  mm: ChromeMessageBroadcaster;
  obs: nsIObserverService;
  perms: nsIPermissionManager;
  policies: nsIEnterprisePolicies;
  ppmm: ParentProcessMessageManager;
  prefs: nsIPrefBranch & nsIPrefService;
  profiler: nsIProfiler;
  prompt: nsIPromptService;
  qms: nsIQuotaManagerService;
  rfp: nsIRFPService;
  scriptSecurityManager: nsIScriptSecurityManager;
  scriptloader: mozIJSSubScriptLoader;
  search: nsISearchService;
  sessionStorage: nsISessionStorageService;
  startup: nsIAppStartup;
  storage: mozIStorageService;
  strings: nsIStringBundleService;
  sysinfo: nsIPropertyBag2 & nsISystemInfo;
  telemetry: nsITelemetry;
  textToSubURI: nsITextToSubURI;
  tm: nsIThreadManager;
  uriFixup: nsIURIFixup;
  urlFormatter: nsIURLFormatter;
  uuid: nsIUUIDGenerator;
  vc: nsIVersionComparator;
  wm: nsIWindowMediator;
  ww: nsIWindowWatcher;
  xulStore: nsIXULStore;
}
```
