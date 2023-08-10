
## rar2john

```shell
john-1.9.0-jumbo-1-win64\run\rar2john.exe xxx.rar
```

## hashcat

对于 RAR，我们选择 `-m` ：

RAR3-hp - 12500
RAR5 - 13000
可以由 rar2john 提取出的 hash 值分辨出，如 `$rar5$16$2f7307ef2ad72d1b269f27584b75c5a6$15$2f1a08b1b3db654bb18857b39cdd47ba$8$ce36bd864f5d7dc8` 属于 RAR5。

字符集

?l: abcdefghijklmnopqrstuvwxyz
?u: ABCDEFGHIJKLMNOPQRSTUVWXYZ
?d: 0123456789
?h: 0123456789abcdef
?H: 0123456789ABCDEF
?s: «space»!"#$%&'()*+,-./:;<=>?@[\]^_{|}~
?a: ?l?u?d?s
?b: 0x00 - 0xff
除此之外，还可以自定义字符集，参见 Mask Attack

例：?a?l?l?l?d?d?d?d 代表第一位为任意字符，第二到第四位为小写字母，后四位为数字

```shell
.\hashcat.exe -m 13000 -a 3 '$rar5$16$2f7307ef2ad72d1b269f27584b75c5a6$15$2f1a08b1b3db654bb18857b39cdd47ba$8$ce36bd864f5d7dc8' ?a?a?a?a
```
