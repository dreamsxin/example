
```output
Started GET "/test/Test" for 127.0.0.1 at 2016-09-28 17:19:33 +0800
Processing by ProjectsController#show as HTML
  Parameters: {"namespace_id"=>"test", "id"=>"Test"}
Completed 500 Internal Server Error in 50ms (ActiveRecord: 9.1ms)

Rugged::OdbError (Object not found - failed to find pack entry (0232100132054d12018ce4e49675000b50ec1867)):
  app/models/repository.rb:16:in `new'
  app/models/repository.rb:16:in `initialize'
  app/models/project.rb:268:in `new'
  app/models/project.rb:268:in `repository'
  app/controllers/application_controller.rb:136:in `repository'
```

```shell
# 查看 检查仓库的数据完整性
git fsck
cd repositories/test/Test.git
rm –fr ref/head/<branchname>
git fsck
```

如果指定 `--full` 选项，该命令显示所有未被其他对象引用 (指向) 的所有对象：

```shell
git fsck --full
dangling blob d670460b4b4aece5915caf5c68d12f560a9fe3e4
dangling commit ab1afef80fac8e34258ff41fc1b867c702daa24b
dangling tree aea790b9a58f6cf6f2804eeac9f0abbe9631e4c9
dangling blob 7108f7ecb345ee9d0084193f147cdad4d2998293
```

本例中，可以从 dangling commit 找到丢失了的 commit。用相同的方法就可以恢复它，即创建一个指向该 SHA 的分支。