#ifndef UTHREAD_H
#define UTHREAD_H

/* initialize the thread library */
void uthread_init();

/* create new thread with priority pri that executes
   function func whose argument is val */
int uthread_create(void (*func)(int), int val, int pri);

/* make currently running thread relinquish cpu and start
   any waiting threads using round-robin scheduling */
void uthread_yield();

/* terminate currently running thread and run next thread
   if no other threads, process should exit */
void uthread_exit();

#endif /* uthread.h */