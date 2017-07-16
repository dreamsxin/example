# Gearman

## 任务的优先级

同步阻塞调用，等待返回结果
	
- doLow:最低优先
- doNomal:正常优先级
- doHigh:最优先执行

异步派发任务，不等待返回结果，返回任务句柄，通过该句柄可获取任务运行状态信息

- doLowBackground
- doBackground
- doHighBackground

注意Task和doXXX区别，Task是一组job，派发后会分配到多个worker上并行执行并返回结果给调用方。
而doXXX的每个任务只会在一个worker上执行。

- addTaskLow
- addTask
- addTaskHigh

- addTaskLowBackground
- addTaskBackground
- addTaskHighBackground

cli.php代码如下：
```php
<?php
$client = new GearmanClient();
$client->addServer('127.0.0.1', 4730);
$client->setCompleteCallback(function($task) {
    echo $task->data(), PHP_EOL;
});
//分别添加不同优先级的Task
$client->addTaskLow('sum', json_encode(array(100, 100)));
$client->addTask('sum', json_encode(array(200, 200)));
$client->addTaskHigh('sum', json_encode(array(300, 300)));
 
$client->runTasks();
```

worker.php代码如下：
```php	
<?php
$worker = new GearmanWorker();
$worker->addServer('127.0.0.1', 4730);
$worker->addFunction('sum', function($job) {
    $data = json_decode($job->workload(), true);
    $ret = $data[0] + $data[1];
    echo $ret, PHP_EOL;
    sleep(1);
    return $ret;
});
while($worker->work());
```

## 获取任务的状态

cli.php代码如下：
```php
<?php
$client = new GearmanClient();
$client->addServer('127.0.0.1', 4730);
//任务排队时的回调
$client->setCreatedCallback('reverse_create');
//有任务数据包时的回调
$client->setDataCallback('reverse_data');
//收集任务状态的回调
$client->setStatusCallback('reverse_status');
//任务完成时的回调
$client->setCompleteCallback('reverse_complete');
//任务失败时的回调
$client->setFailCallback('reverse_fail');
//分别添加不同优先级的Task
$client->addTaskLow('reverse', '1234567');
$client->addTask('reverse', 'hello');
$client->addTaskHigh('reverse', 'world');
 
$client->runTasks();
 
function reverse_create($task) {
    echo '任务创建(', $task->jobHandle() , '): ', PHP_EOL;
}
function reverse_data($task) {
    echo '接收数据(', $task->jobHandle() , '): ', $task->data(), PHP_EOL;
}
function reverse_status($task) {
    //taskNumerator()获取任务完成情况的分子
    //taskDenominator()获取任务完成情况的分母
    echo '完成情况(', $task->jobHandle() , '): ', $task->taskNumerator(), '/', $task->taskDenominator(), PHP_EOL;
}
function reverse_complete($task) {
    echo '任务完成(', $task->jobHandle() , '): ', $task->data(), PHP_EOL;
}
function reverse_fail($task) {
    echo '任务失败(', $task->jobHandle() , '): ', PHP_EOL;
}
```

worker.php代码如下：
```php
<?php
$worker = new GearmanWorker();
$worker->addServer('127.0.0.1', 4730);
$worker->addFunction('reverse', function($job) {
    //workload()返回接收的数据
    $data = $job->workload();
    //workloadSize()返回接收数据的字节大小
    $dataSize = $job->workloadSize();
    echo "worker: {$data} ({$dataSize}) \n";
 
    $tmp = 0;
    $mid = $dataSize / 2;
    for($ix = 0; $ix < $mid; ++$ix) {
        $tmp = $data[$ix];
        $data[$ix] = $data[$dataSize - $ix - 1];
        $data[$dataSize - $ix - 1] = $tmp;
 
        //这里发送状态，会被client设置的回调reverse_status()接收到
        //状态完成情况的分子与分母
        $job->sendStatus($ix + 1, $mid + 1);
        //发送数据，被reverse_data()接收到
        $job->sendData($data);
        sleep(1);
    }
    return $data;
});
while($worker->work());
```