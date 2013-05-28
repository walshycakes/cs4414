threadpkg2: uthread.c
	gcc -c -o threadpkg2.o uthread.c

thread: uthread.c uthread_test.c
	gcc -o thread_test uthread.c uthread_test.c
	./thread_test

mutex: uthread.c mutex_test.c
	gcc -o mutex_test uthread.c mutex_test.c
	./mutex_test

clean:
	-rm -f *.o thread_test mutex_test
