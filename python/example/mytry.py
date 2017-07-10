from MyException import *

try:
    open('loss.json', 'r')
except IOError as e:
    print e
finally:
    print "always excute"

try:
    open('loss.json', 'r')
except Exception as e:
    print e



try:
    raise MyException2("my excepition is raised ")
except MyException2 as e:
    print e
