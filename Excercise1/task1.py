# Python 3.3.3 and 2.7.6
# python helloworld_python.py

from threading import Thread

i = 0

def thread1_func():
	global i
	print("Hello from a thread1!")
	for j in range (0,1000000):
   		i +=1
   	print("i =+ %d" % i)

def thread2_func():
	global i
	print("Hello from a thread2!")
	for k in range (0,1000000):
		i -=1
	print("i -= %d" % i) 


def main():
    thread1 = Thread(target = thread1_func, args = (),)
    thread2 = Thread(target = thread2_func, args = (),)
    thread1.start()
    thread2.start()
    thread1.join()
    thread2.join()
    print("Hello from main!")
    print("Hello from testeditor2!")
    print("i = %d" % i) 
   
main()