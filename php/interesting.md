# 趣味数学题

## 每瓶啤酒2元，2个空酒瓶或4个瓶盖可换1瓶啤酒。10元最多可喝多少瓶啤酒？

```php

function getMillisecond() {
    list($t1, $t2) = explode(' ', microtime());
    return (float)sprintf('%.0f',(floatval($t1)+floatval($t2))*1000);
}

// Manaphp 的遍历方法
function calc($money) {
    $bottles = (int)($money/2);
    $capsules = 0;
    $empty = 0;

    for ($i = 0; $i < $bottles; $i++) {
        $capsules++;
        $empty++;

        if ($capsules > 0 && $capsules % 4 === 0) {
            $bottles++;
            $capsules -= 4;
        }

        if ($empty > 0 && $empty % 2 === 0) {
            $bottles ++;
            $empty -= 2;
        }
    }
    return [$bottles, $capsules, $empty];
}

$start = getMillisecond();
var_dump(calc(10000));
echo '耗时：'.(getMillisecond() - $start).PHP_EOL;

// Dreamsxin 的递归方法
function calc2($money, $capsules, $empty) {
    $bottles = (int)($money/2) + (int)($capsules/4) + (int)($empty/2);

    if ($bottles <= 0) {
        return 0;
    }
    $money = $money%2;
    $capsules = ($capsules%4) + $bottles;
    $empty = ($empty%2) + $bottles;
    return $bottles + calc2($money, $capsules, $empty);
}

$money = 10000;
$capsules = 0;
$empty = 0;
$start = getMillisecond();
var_dump(calc2($money, $capsules, $empty));
echo '耗时：'.(getMillisecond() - $start).PHP_EOL;
```
