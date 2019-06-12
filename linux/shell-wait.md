# Wait Command in Linux

## Example-1: Using wait command for multiple processes

```shell
#!/bin/bash
echo "testing wait command1" &
process_id=$!
echo "testing wait command2" &
wait $process_id
echo Job 1 exited with status $?
wait $!
echo Job 2 exited with status $?
```

## Example-2: Test wait command after using kill command

```shell
#!/bin/bash
echo "Testing wait command"
sleep 20 &
pid=$!
kill $pid
wait $pid
echo $pid was terminated.
```

## Example-3: Check the exit status value

```shell
#!/bin/bash
function check()
{
echo "Sleep for $1 seconds"
sleep $1
exit $2
}
check $1 $2 &
b=$!
echo "Checking the status"
wait $b && echo OK || echo NOT OK
```

```shell
bash wait3.sh 3 0
bash wait3.sh 3 5
```