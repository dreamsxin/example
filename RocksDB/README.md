# RocksDB

RocksDB is an embeddable persistent key-value store for fast storage.

https://github.com/facebook/rocksdb

## GET STARTED

**Opening A Database**
```c++
#include <assert>
#include "rocksdb/db.h"

rocksdb::DB* db;
rocksdb::Options options;
options.create_if_missing = true;
//options.error_if_exists = true;
rocksdb::Status status =
  rocksdb::DB::Open(options, "/tmp/testdb", &db);
assert(status.ok());
...
```
**Status**
```c++
rocksdb::Status s = ...;
if (!s.ok()) cerr << s.ToString() << endl;
```

**Close**
```c++
/* open the db as described above */
/* do something with db */
delete db;
```

**Read And Write**
```c++
std::string value;
rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key1, &value);
if (s.ok()) s = db->Put(rocksdb::WriteOptions(), key2, value);
if (s.ok()) s = db->Delete(rocksdb::WriteOptions(), key1);
```

