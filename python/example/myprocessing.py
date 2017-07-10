import threading
import multiprocessing
import time

def worker_1(interval):
    print "worker_1"
    time.sleep(interval)
    print "end worker_1"

def worker_2(interval):
    print "worker_2"
    time.sleep(interval)
    print "end worker_2"

def worker_3(interval):
    print "worker_3"
    time.sleep(interval)
    print "end worker_3"


p1 = multiprocessing.Process(target = worker_1, args = (1,))
p2 = multiprocessing.Process(target = worker_2, args = (2,))
p3 = multiprocessing.Process(target = worker_3, args = (3,))

p1.daemon = True
p2.daemon = True
p3.daemon = True
p1.start()
p2.start()
p3.start()
#p1.join()
#p2.join()
#p3.join()

print("The number of CPU is:" + str(multiprocessing.cpu_count()))
for p in multiprocessing.active_children():
    print("child   p.name:" + p.name + "\tp.id" + str(p.pid))
print "END!!!!!!!!!!!!!!!!!"
