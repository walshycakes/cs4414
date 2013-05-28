/*Names: Bryan Walsh (bpw7xx), Jared Culp (jjc4fb)
  Class: CS 4414-Operating Systems
  Assignment: Lab #3-Threads pt I
  Date: 3/2/2013
  */

#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>
#define STACK_SIZE 1000000

//uthread struct to hold a thread's context, pointers to the next and 
//previous threads (in order to maintain a notion of a circular queue),
//a thread id, and a stack pointer so we always have a pointer to the base of
//the context's stack.
typedef struct uthread {
    ucontext_t *context;
    struct uthread *next, *pred;
    int t_id;
    void *stack_ptr;
} uthread_t;

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
