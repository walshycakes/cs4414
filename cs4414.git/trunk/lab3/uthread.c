/*Names: Bryan Walsh (bpw7xx), Jared Culp (jjc4fb)
  Class: CS 4414-Operating Systems
  Assignment: Lab #3-Threads pt I
  Date: 3/2/2013
  */

#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"

//forward declarations
void uthread_handle_queue(int is_exit);
int compare_ucontext(const void *key, const void *with);
void cleanup_routine();

//threads head and tail keep our notion of a circular list
uthread_t *head, *tail;
//thread current keeps notion of the current running thread
uthread_t *current;
//garbage is the thread that we delete on a call to exit()
uthread_t *garbage;
//ints to keep track of # of created threads and the size of our "list"
int threads_created, queue_size;
//statically allocated stack for the cleanup context
static char cleanup_stack[STACK_SIZE];

void uthread_init() {

    // make a dummy node for the main thread
    uthread_t *main_thread = (uthread_t*) malloc(sizeof(uthread_t));
    ucontext_t *main_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    getcontext(main_context);

    //initialize the values for the main thread
    main_thread->context = main_context;
    main_thread->next = NULL;
    main_thread->pred = NULL;
    main_thread->t_id = 0;
    main_thread->stack_ptr = NULL;
    queue_size = 1;

    //set up the initial "nodes" of our list
    head = main_thread;
    tail = main_thread;
    current = main_thread;
    head->next = head;
    head->pred = head;
    tail->next = tail;
    tail->pred = tail;

    //set up the context for our cleanup thread
    garbage = (uthread_t*) malloc(sizeof(uthread_t));
    ucontext_t *garbage_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    getcontext(garbage_context);
    garbage->context = garbage_context;
    garbage->context->uc_stack.ss_sp = cleanup_stack;
    garbage->context->uc_stack.ss_size = STACK_SIZE;
    garbage->stack_ptr = cleanup_stack;
    makecontext(garbage_context, (void(*)(void)) cleanup_routine, 0);
}


int uthread_create(void (*func)(int), int val, int pri) {

    //malloc memory for each created thread and corresponding context
    uthread_t *created_thread = (uthread_t*) malloc(sizeof(uthread_t));
    ucontext_t *created_context = (ucontext_t*) malloc(sizeof(ucontext_t));

    //if getcontext() returns -1 then there is an error
    if (getcontext(created_context) == -1)
        printf("error.\n");

    //set up stack pointer and stack size for the created thread's context
    created_context->uc_stack.ss_sp = (void*) malloc(STACK_SIZE);
    created_context->uc_stack.ss_size = STACK_SIZE;

    //create the context for the created_thread
    makecontext(created_context, (void(*)(void)) func, 1, val);

    //initialize context value, thread id, and stack_ptr for the created thread
    created_thread->context = created_context;
    created_thread->t_id = ++threads_created;
    created_thread->stack_ptr = created_context->uc_stack.ss_sp;

    //update the nodes in our list and incrememnt the size of the list
    created_thread->pred = tail;
    tail->next = created_thread;
    tail = created_thread;
    tail->next = head;
    head->pred = tail;
    queue_size++;
    return created_thread->t_id;

}

void uthread_yield() {
    //call handle_queue which will rearrange pointers and swap contexts
    uthread_handle_queue(0);

}

void uthread_exit() {
    //call handle_queue which will decrement the size of the queue and swap contexts
    uthread_handle_queue(1);

}

void uthread_handle_queue(int is_exit) {
    if (is_exit) {
        //decrement the queue size and set the context to garbage's context 
        //so that we remove the unwanted thread calling exit()
        queue_size = queue_size - 1;
        setcontext(garbage->context);
    }
    else {
        //in a call to yield (not exit) we change current to be the next value
        //in the list and swap to it's context
        current = current->next;
        swapcontext(current->pred->context, current->context);
    }
}

void cleanup_routine() {
    //if the list size is greater than 0 we rearrange pointers so that we
    //can remove the current context. We then free the removed node's
    //stack ptr, context, and the thread structure itself
    if (queue_size > 0) {
        uthread_t* old_current = current;
        current->next->pred = current->pred;
        current->pred->next = current->next;
        free(current->stack_ptr);
        free(current->context);
        current = current->next;
        free(old_current);
        setcontext(current->context);
    }
    //if the list size is zero then there is no context to switch to so we
    //free the current context's stack ptr, its context, the thread itself,
    //and also free the garbage context that we malloc'd for and the garbage
    //thread itself
    else {
        free(current->stack_ptr);
        free(current->context);
        free(current);
        free(garbage->context);
        free(garbage);
    }
}
