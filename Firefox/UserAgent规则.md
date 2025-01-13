https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Headers/User-Agent/Firefox

`Mozilla/5.0 (platform; rv:geckoversion) Gecko/geckotrail Firefox/firefoxversion`

- Mozilla/5.0 是表示浏览器与 Mozilla 兼容的通用标记，目前几乎所有浏览器都使用该标记。
- platform 描述浏览器运行的本地平台（如 Windows、Mac、Linux 或 Android），以及是否是手机。Firefox OS 手机显示“Mobile”；web 就是它的平台。请注意，platform 可以由多个“;”分隔的标记组成。详情和示例请参阅下文。
- rv:geckoversion 表示 Gecko 的发行版本（如“17.0”）。
- Gecko/geckotrail 表示浏览器基于 Gecko。
- 在桌面上，geckotrail 是固定字符串“20100101”。
- Firefox/firefoxversion 表示浏览器是 Firefox，并提供版本（如“17.0”）。
- 从移动版 Firefox 10 开始，geckotrail 与 firefoxversion 相同。

```c++

  // Gather platform.
  mPlatform.AssignLiteral(
#if defined(ANDROID)
      "Android"
#elif defined(XP_WIN)
      "Windows"
#elif defined(XP_MACOSX)
      "Macintosh"
#elif defined(XP_UNIX)
      // We historically have always had X11 here,
      // and there seems little a webpage can sensibly do
      // based on it being something else, so use X11 for
      // backwards compatibility in all cases.
      "X11"
#endif
  );

#ifdef XP_UNIX
  if (IsRunningUnderUbuntuSnap()) {
    mPlatform.AppendLiteral("; Ubuntu");
  }
#endif

#ifdef ANDROID
  nsCOMPtr<nsIPropertyBag2> infoService;
  infoService = mozilla::components::SystemInfo::Service();
  MOZ_ASSERT(infoService, "Could not find a system info service");
  nsresult rv;

  // Add the Android version number to the Fennec platform identifier.
  nsAutoString androidVersion;
  rv = infoService->GetPropertyAsAString(u"release_version"_ns, androidVersion);
  MOZ_ASSERT_IF(
      NS_SUCCEEDED(rv),
      // Like version "9"
      (androidVersion.Length() == 1 && std::isdigit(androidVersion[0])) ||
          // Or like version "8.1", "10", or "12.1"
          (androidVersion.Length() >= 2 && std::isdigit(androidVersion[0]) &&
           (androidVersion[1] == u'.' || std::isdigit(androidVersion[1]))));

  // Spoof version "Android 10" for Android OS versions < 10 to reduce their
  // fingerprintable user information. For Android OS versions >= 10, report
  // the real OS version because some enterprise websites only want to permit
  // clients with recent OS version (like bug 1876742). Two leading digits
  // in the version string means the version number is >= 10.
  mPlatform += " ";
  if (NS_SUCCEEDED(rv) && androidVersion.Length() >= 2 &&
      std::isdigit(androidVersion[0]) && std::isdigit(androidVersion[1])) {
    mPlatform += NS_LossyConvertUTF16toASCII(androidVersion);
  } else {
    mPlatform.AppendLiteral("10");
  }

  // Add the `Mobile` or `TV` token when running on device.
  bool isTV;
  rv = infoService->GetPropertyAsBool(u"tv"_ns, &isTV);
  if (NS_SUCCEEDED(rv) && isTV) {
    mCompatDevice.AssignLiteral("TV");
  } else {
    mCompatDevice.AssignLiteral("Mobile");
  }

  if (Preferences::GetBool(UA_PREF("use_device"), false)) {
    mDeviceModelId = mozilla::net::GetDeviceModelId();
  }
#endif  // ANDROID

  // Gather OS/CPU.
#if defined(XP_WIN)

#  if defined _M_X64 || defined _M_AMD64
  mOscpu.AssignLiteral(OSCPU_WIN64);
#  elif defined(_ARM64_)
  // Report ARM64 Windows 11+ as x86_64 and Windows 10 as x86. Windows 11+
  // supports x86_64 emulation, but Windows 10 only supports x86 emulation.
  if (IsWin11OrLater()) {
    mOscpu.AssignLiteral(OSCPU_WIN64);
  } else {
    mOscpu.AssignLiteral(OSCPU_WINDOWS);
  }
#  else
  BOOL isWow64 = FALSE;
  if (!IsWow64Process(GetCurrentProcess(), &isWow64)) {
    isWow64 = FALSE;
  }
  if (isWow64) {
    mOscpu.AssignLiteral(OSCPU_WIN64);
  } else {
    mOscpu.AssignLiteral(OSCPU_WINDOWS);
  }
#  endif

#elif defined(XP_MACOSX)
  mOscpu.AssignLiteral("Intel Mac OS X 10.15");
#elif defined(XP_UNIX)
  if (mozilla::StaticPrefs::network_http_useragent_freezeCpu()) {
#  ifdef ANDROID
    mOscpu.AssignLiteral("Linux armv81");
#  else
    mOscpu.AssignLiteral("Linux x86_64");
#  endif
  } else {
    struct utsname name {};
    int ret = uname(&name);
    if (ret >= 0) {
      nsAutoCString buf;
      buf = (char*)name.sysname;
      buf += ' ';

#  ifdef AIX
      buf += (char*)name.version;
      buf += '.';
      buf += (char*)name.release;
#  else
      buf += (char*)name.machine;
#  endif

      mOscpu.Assign(buf);
    }
  }
#endif

void nsHttpHandler::BuildUserAgent() {

  // Application portion
  mUserAgent.Assign("Mozilla");
  mUserAgent += '/';
  mUserAgent += "5.0";
  mUserAgent += ' ';

  // Application comment
      "Android"
#elif defined(XP_WIN)
      "Windows"
#elif defined(XP_MACOSX)
      "Macintosh"
#elif defined(XP_UNIX)
      // We historically have always had X11 here,
      // and there seems little a webpage can sensibly do
      // based on it being something else, so use X11 for
      // backwards compatibility in all cases.
      "X11"
  mUserAgent += '(';
#ifndef UA_SPARE_PLATFORM
  if (!mPlatform.IsEmpty()) {
    mUserAgent += mPlatform;
    mUserAgent.AppendLiteral("; ");
  }
#endif
  if (!mCompatDevice.IsEmpty()) {
    mUserAgent += mCompatDevice;
    mUserAgent.AppendLiteral("; ");
  } else if (!mOscpu.IsEmpty()) {
    mUserAgent += mOscpu;
    mUserAgent.AppendLiteral("; ");
  }
  if (!mDeviceModelId.IsEmpty()) {
    mUserAgent += mDeviceModelId;
    mUserAgent.AppendLiteral("; ");
  }
  mUserAgent += mMisc;
  mUserAgent += ')';

  // Product portion
  mUserAgent += ' ';
  mUserAgent += mProduct;
  mUserAgent += '/';
  mUserAgent += mProductSub;

  bool isFirefox = mAppName.EqualsLiteral("Firefox");
  if (isFirefox || mCompatFirefoxEnabled) {
    // "Firefox/x.y" (compatibility) app token
    mUserAgent += ' ';
    mUserAgent += mCompatFirefox;
  }
  if (!isFirefox) {
    // App portion
    mUserAgent += ' ';
    mUserAgent += mAppName;
    mUserAgent += '/';
    mUserAgent += mAppVersion;
  }
}
```
