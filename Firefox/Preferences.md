## `modules\libpref\Preferences.cpp`
不是父对象，将移除
```c++

  if (!XRE_IsParentProcess()) {
    MOZ_ASSERT(gChangedDomPrefs);
    for (unsigned int i = 0; i < gChangedDomPrefs->Length(); i++) {
      nsCString nameStr = gChangedDomPrefs->ElementAt(i).name();
      if (nameStr.EqualsASCII("extensions.webextensions.uuids")) {
        continue;
      }
      Preferences::SetPreference(gChangedDomPrefs->ElementAt(i));
    }
    gChangedDomPrefs = nullptr;
}
```
