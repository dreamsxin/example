
## `dom\ipc\ContentProcess.cpp`
- `ContentProcess::Init`

## `toolkit\xre\nsAppRunner.cpp`
- `ScopedXPCOMStartup::Initialize`
```c++
nsXREDirProvider::nsXREDirProvider() { gDirServiceProvider = this; }

nsresult ScopedXPCOMStartup::Initialize(bool aInitJSContext) {
  NS_ASSERTION(gDirServiceProvider, "Should not get here!");

  nsresult rv;

  // gDirServiceProvider
  rv = NS_InitXPCOM(&mServiceManager, gDirServiceProvider->GetAppDir(),
                    gDirServiceProvider, aInitJSContext);
  if (NS_FAILED(rv)) {
    NS_ERROR("Couldn't start xpcom!");
    mServiceManager = nullptr;
  } else {
#ifdef DEBUG
    nsCOMPtr<nsIComponentRegistrar> reg = do_QueryInterface(mServiceManager);
    NS_ASSERTION(reg, "Service Manager doesn't QI to Registrar.");
#endif
  }

  return rv;
}
```
## `xpcom\io`

组件定义 `xpcom\io\components.conf`
- `NS_InitXPCOM`
```c++
nsDirectoryService::RealInit();
```

## `xpcom\io\nsDirectoryService.h`

## `layout\build\nsLayoutStatics.cpp`
- nsLayoutStatics::Initialize()
```c++
  if (XRE_IsParentProcess()) {
    InitializeQuotaManager();
    InitializeLocalStorage();
  }
```


### `dom\quota\ActorsParent.cpp`
- InitializeQuotaManager
```c++
void InitializeQuotaManager() {
  MOZ_ASSERT(XRE_IsParentProcess());
  MOZ_ASSERT(NS_IsMainThread());
  MOZ_ASSERT(!gQuotaManagerInitialized);

  if (!QuotaManager::IsRunningGTests()) {
    // These services have to be started on the main thread currently.
    const nsCOMPtr<mozIStorageService> ss =
        do_GetService(MOZ_STORAGE_SERVICE_CONTRACTID);
    QM_WARNONLY_TRY(OkIf(ss));

    RefPtr<net::ExtensionProtocolHandler> extensionProtocolHandler =
        net::ExtensionProtocolHandler::GetSingleton();
    QM_WARNONLY_TRY(MOZ_TO_RESULT(extensionProtocolHandler));

    IndexedDatabaseManager* mgr = IndexedDatabaseManager::GetOrCreate();
    QM_WARNONLY_TRY(MOZ_TO_RESULT(mgr));
  }
  // 
  QM_WARNONLY_TRY(QM_TO_RESULT(QuotaManager::Initialize()));

#ifdef DEBUG
  gQuotaManagerInitialized = true;
#endif
}
```
- QuotaManager::EnsureCreated()
- QuotaManager::InitializeOrigin

## `dom\localstorage\`

**继承关系**
```c++
class PrepareDatastoreOp
    : public LSRequestBase,
      public SupportsCheckedUnsafePtr<CheckIf<DiagnosticAssertEnabled>> {
}
```
- InitializeLocalStorage
只是注册回调
```c++
void InitializeLocalStorage() {
  MOZ_ASSERT(XRE_IsParentProcess());
  MOZ_ASSERT(NS_IsMainThread());
  MOZ_ASSERT(!gLocalStorageInitialized);

  // XXX Isn't this redundant? It's already done in InitializeQuotaManager.
  if (!QuotaManager::IsRunningGTests()) {
    // This service has to be started on the main thread currently.
    const nsCOMPtr<mozIStorageService> ss =
        do_GetService(MOZ_STORAGE_SERVICE_CONTRACTID);

    QM_WARNONLY_TRY(OkIf(ss));
  }

  Preferences::RegisterCallbackAndCall(ShadowWritesPrefChangedCallback,
                                       kShadowWritesPref);

  Preferences::RegisterCallbackAndCall(SnapshotPrefillPrefChangedCallback,
                                       kSnapshotPrefillPref);

  Preferences::RegisterCallbackAndCall(
      SnapshotGradualPrefillPrefChangedCallback, kSnapshotGradualPrefillPref);

  Preferences::RegisterCallbackAndCall(ClientValidationPrefChangedCallback,
                                       kClientValidationPref);

#ifdef DEBUG
  gLocalStorageInitialized = true;
#endif
}
```

**常量**
```c++
constexpr auto kDataFileName = u"data.sqlite"_ns;

constexpr auto kStorageName = u"storage"_ns;

#define INDEXEDDB_DIRECTORY_NAME u"indexedDB"
#define ARCHIVES_DIRECTORY_NAME u"archives"
#define PERSISTENT_DIRECTORY_NAME u"persistent"
#define PERMANENT_DIRECTORY_NAME u"permanent"
#define TEMPORARY_DIRECTORY_NAME u"temporary"
#define DEFAULT_DIRECTORY_NAME u"default"
#define PRIVATE_DIRECTORY_NAME u"private"
#define TOBEREMOVED_DIRECTORY_NAME u"to-be-removed"

#define WEB_APPS_STORE_FILE_NAME u"webappsstore.sqlite"
#define LS_ARCHIVE_FILE_NAME u"ls-archive.sqlite"
#define LS_ARCHIVE_TMP_FILE_NAME u"ls-archive-tmp.sqlite"
```
- LSRequestBase::Finish()
- LSRequestBase::FinishInternal()
- LSRequestBase::Run()
- LSRequestBase::StartRequest()
- ConnectionDatastoreOperationBase::Run()
- ConnectionDatastoreOperationBase::RunOnConnectionThread()
- PrepareDatastoreOp::DatabaseWork()
- PrepareDatastoreOp::NestedRun()
- PrepareDatastoreOp::BeginLoadData()
- PrepareDatastoreOp::LoadDataOp::DoDatastoreWork()
- PrepareDatastoreOp::Start()
- PrepareDatastoreOp::GetResponse
- TransactionDatabaseOperationBase::RunOnConnectionThread()
- Database::EnsureConnection()
- ConnectionPool::GetOrCreateConnection
- new DatabaseConnection
- DatabaseConnection::Init
- Connection::Connection
- Connection::SetItem
- Connection::RemoveItem
- ConnectionWriteOptimizer::PerformInsertOrUpdate
- ConnectionWriteOptimizer::PerformDelete
- ConnectionWriteOptimizer::PerformTruncate
- Connection::InitTemporaryOriginHelper::Run()
- Connection::InitTemporaryOriginHelper::RunOnIOThread()
- QuotaManager::Get()
- QuotaManager::EnsureTemporaryOriginIsInitializedInternal
- QuotaManager::Init()
- QuotaManager::Observer::Observe
设置 `gBasePath`
- QuotaManager::Initialize()
- QuotaManager::GetOrCreate()
```cpp
auto instance = MakeRefPtr<QuotaManager>(*gBasePath, *gStorageName);
```
- QuotaManager::QuotaManager()
初始化目录
```c++
  QM_TRY_UNWRAP(
      do_Init(mIndexedDBPath),
      GetPathForStorage(*baseDir, nsLiteralString(INDEXEDDB_DIRECTORY_NAME)));

  QM_TRY(MOZ_TO_RESULT(baseDir->Append(mStorageName)));

  QM_TRY_UNWRAP(do_Init(mStoragePath),
                MOZ_TO_RESULT_INVOKE_MEMBER_TYPED(nsString, baseDir, GetPath));

  QM_TRY_UNWRAP(
      do_Init(mStorageArchivesPath),
      GetPathForStorage(*baseDir, nsLiteralString(ARCHIVES_DIRECTORY_NAME)));

  QM_TRY_UNWRAP(
      do_Init(mPermanentStoragePath),
      GetPathForStorage(*baseDir, nsLiteralString(PERMANENT_DIRECTORY_NAME)));

  QM_TRY_UNWRAP(
      do_Init(mTemporaryStoragePath),
      GetPathForStorage(*baseDir, nsLiteralString(TEMPORARY_DIRECTORY_NAME)));

  QM_TRY_UNWRAP(
      do_Init(mDefaultStoragePath),
      GetPathForStorage(*baseDir, nsLiteralString(DEFAULT_DIRECTORY_NAME)));

  QM_TRY_UNWRAP(
      do_Init(mPrivateStoragePath),
      GetPathForStorage(*baseDir, nsLiteralString(PRIVATE_DIRECTORY_NAME)));

  QM_TRY_UNWRAP(
      do_Init(mToBeRemovedStoragePath),
      GetPathForStorage(*baseDir, nsLiteralString(TOBEREMOVED_DIRECTORY_NAME)));
```
- QuotaClient::InitOrigin
- QuotaManager::EnsureTemporaryStorageIsInitializedInternal
- QuotaManager::LoadQuota()
- QuotaManager::GetOriginDirectory

```c++
Result<nsCOMPtr<nsIFile>, nsresult> QuotaManager::GetOriginDirectory(
    const OriginMetadata& aOriginMetadata) const {
  QM_TRY_UNWRAP(
      auto directory,
      QM_NewLocalFile(GetStoragePath(aOriginMetadata.mPersistenceType)));

  // 根据 origin 设置子目录 
  QM_TRY(MOZ_TO_RESULT(directory->Append(
      MakeSanitizedOriginString(aOriginMetadata.mStorageOrigin))));

  return directory;
}
const nsString& GetStoragePath() const { return *mStoragePath; }
  const nsString& GetStoragePath(PersistenceType aPersistenceType) const {
    if (aPersistenceType == PERSISTENCE_TYPE_PERSISTENT) {
      return *mPermanentStoragePath;
    }

    if (aPersistenceType == PERSISTENCE_TYPE_TEMPORARY) {
      return *mTemporaryStoragePath;
    }

    if (aPersistenceType == PERSISTENCE_TYPE_DEFAULT) {
      return *mDefaultStoragePath;
    }

    MOZ_ASSERT(aPersistenceType == PERSISTENCE_TYPE_PRIVATE);

    return *mPrivateStoragePath;
  }
```
`dom\quota\SanitizationUtils.cpp`
```c++
const char QuotaManager::kReplaceChars[] = CONTROL_CHARACTERS "/:*?\"<>|\\";
const char16_t QuotaManager::kReplaceChars16[] =
    u"" CONTROL_CHARACTERS "/:*?\"<>|\\";

nsAutoString MakeSanitizedOriginString(const nsACString& aOrigin) {
  // An origin string is ASCII-only, since it is obtained via
  // nsIPrincipal::GetOrigin, which returns an ACString.
  return NS_ConvertASCIItoUTF16(MakeSanitizedOriginCString(aOrigin));
}
// 替换成 +++
nsAutoCString MakeSanitizedOriginCString(const nsACString& aOrigin) {
#ifdef XP_WIN
  NS_ASSERTION(!strcmp(QuotaManager::kReplaceChars, FILE_ILLEGAL_CHARACTERS FILE_PATH_SEPARATOR), "Illegal file characters have changed!");
#endif

  nsAutoCString res{aOrigin};

  res.ReplaceChar(QuotaManager::kReplaceChars, '+');

  return res;
}
```
