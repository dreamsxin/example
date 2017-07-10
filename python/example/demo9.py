import time as t
from time import time

import module1

print(t.localtime())
print(time())
print t.strftime("%Y-%m-%d %H:%M:%S", t.localtime())

module1.printdata(1)
