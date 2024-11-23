## `browser\app\profile\firefox.js`

## `modules\libpref\Preferences.cpp`
如果满足以下两个条件之一，则首选项将被“净化”（即不发送到 webcontent 进程）：
1.首选项名称与以下列表中的前缀之一匹配
2.pref 是动态命名的
```c++
static bool ShouldSanitizePreference(const Pref* const aPref) {
  // In the parent process, we use a heuristic to decide if a pref
  // value should be sanitized before sending to subprocesses.
  MOZ_DIAGNOSTIC_ASSERT(XRE_IsParentProcess());

  const char* prefName = aPref->Name();

  // If a pref starts with this magic string, it is a Once-Initialized pref
  // from Static Prefs. It should* not be in the above list and while it looks
  // like a dnyamically named pref, it is not.
  // * nothing enforces this
  if (strncmp(prefName, "$$$", 3) == 0) {
    return false;
  }

  // First check against the denylist.
  // The services pref is an annoying one - it's much easier to blocklist
  // the whole branch and then add this one check to let this one annoying
  // pref through.
  for (const auto& entry : sRestrictFromWebContentProcesses) {
    if (strncmp(entry.mPrefBranch, prefName, entry.mLen) == 0) {
      for (const auto& pasEnt : sOverrideRestrictionsList) {
        if (strncmp(pasEnt.mPrefBranch, prefName, pasEnt.mLen) == 0) {
          return false;
        }
      }
      return true;
    }
  }

  // Then check if it's a dynamically named string preference and not
  // in the override list
  if (aPref->Type() == PrefType::String && !aPref->HasDefaultValue()) {
    for (const auto& entry : sDynamicPrefOverrideList) {
      if (strncmp(entry.mPrefBranch, prefName, entry.mLen) == 0) {
        return false;
      }
    }
    return true;
  }

  return false;
}

void Preferences::SerializePreferences(nsCString& aStr,
                                       bool aIsDestinationWebContentProcess) {
  MOZ_RELEASE_ASSERT(InitStaticMembers());

  aStr.Truncate();

  for (auto iter = HashTable()->iter(); !iter.done(); iter.next()) {
    Pref* pref = iter.get().get();
    if (!pref->IsTypeNone() && pref->HasAdvisablySizedValues()) {
      pref->SerializeAndAppend(aStr, aIsDestinationWebContentProcess && ShouldSanitizePreference(pref));
    }
  }

  aStr.Append('\0');
}
```
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
