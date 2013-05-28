/**
 * CS 4414 Spring 2012
 * Lab 3: Creating a User-Level Thread Package Part I
 *
 * Reference Implementation
 *
 * @author Zhengyang Liu (zl4ef)
 *
 *
 * Lab 4: Creating a User-Level Thread Package Part II
 *
 * @author Jared Culp (jjc4fb)
 * @author Bryan Walsh (bpw7xx)
 *
 * uthread.h
 *
 */

#ifndef __UTHREAD_H
#define __UTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ucontext.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define UTHREAD_STACK_SIZE (1 << 20)
#define PRIORITY_LEVELS 10
#define MAIN_PRIORITY 9
#define ONE_MS 1000

typedef void (*uthread_func_t)(int val);

typedef struct uthread {
    ucontext_t context;
    void *stack;
    struct uthread *next;
    int priority;
    int t_id;
    int holds_lock; // prevent others from unlocking
} uthread_t;

typedef struct priority_level {
    uthread_t *head;
    uthread_t *tail;
    unsigned int size;
} priority_level_t;

typedef struct uthread_mutex {
    int lock_held;
    struct priority_level queue[PRIORITY_LEVELS];
} uthread_mutex_t;

void uthread_init(void);
int uthread_create(uthread_func_t func,int val,int pri);
void uthread_yield(void);
void uthread_exit(void);
void uthread_mutex_init(uthread_mutex_t *lockVar);
void uthread_mutex_lock(uthread_mutex_t *lockVar);
void uthread_mutex_unlock(uthread_mutex_t *lockVar);
void uthread_mutex_destroy(uthread_mutex_t *lockVar);

#endif //__UTHREAD_H
