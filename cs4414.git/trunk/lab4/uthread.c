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
 * uthread.c
 *
 */

#include "uthread.h"

static priority_level_t schedule_queue[PRIORITY_LEVELS];
int priority_now = 0;

static int uthread_id = 0;

ucontext_t cleanup_context;
static uthread_t *retired_thread;                	// the thread to be free'd
static char cleanup_stack[UTHREAD_STACK_SIZE];		// this is allocated on the heap, thus we don't need to free it

struct itimerval timer;
sigset_t set; // process wide signal mask

void timer_interrupt(int signum, siginfo_t *si, void *context) {
	uthread_yield();
}

void setup_signals(void) {
    struct sigaction act;

    act.sa_sigaction = timer_interrupt;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);

    if(sigaction(SIGVTALRM, &act, NULL) != 0) {
        perror("Signal handler");
    }
}

void setup_timers(void) {
	// timer expires after 1 ms, and then every ms
	timer.it_value.tv_sec = 0; 
	timer.it_value.tv_usec = ONE_MS; 
	timer.it_interval.tv_sec = 0; 
	timer.it_interval.tv_usec = ONE_MS; 

	if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
		perror("setitiimer");
	}
}

void update_priority_now() {
    int i;
    for (i = 0; i < PRIORITY_LEVELS; ++i) {
        if (schedule_queue[i].head != NULL) {
            priority_now = i;
            return;
        }
    }
}

void free_stack(void) {
    free(retired_thread->stack);
    free(retired_thread);

    schedule_queue[priority_now].size--;

    if (schedule_queue[priority_now].size == 0) { // this is the last item in the queue
        schedule_queue[priority_now].head = NULL; // set head & tail to NULL
        schedule_queue[priority_now].tail = NULL;
    }

    update_priority_now();
    setcontext(&schedule_queue[priority_now].head->context);
}

void uthread_init(void) {
    uthread_t *main_thread = (uthread_t *) malloc(sizeof(uthread_t));

    if (getcontext(&main_thread->context) < 0) {
        perror("getcontext\n");
        exit(-1);
    }

    main_thread->stack = NULL; // prevent accidental free
    main_thread->priority = MAIN_PRIORITY; // give main lowest priority
    main_thread->t_id = uthread_id;

    schedule_queue[MAIN_PRIORITY].head = main_thread;
    schedule_queue[MAIN_PRIORITY].tail = main_thread;
    schedule_queue[MAIN_PRIORITY].size = 1;

    priority_now = MAIN_PRIORITY;
            
    if (getcontext(&cleanup_context) < 0) {
        perror("getcontext\n");
        exit(-1);
    }

    cleanup_context.uc_stack.ss_sp = cleanup_stack;
    cleanup_context.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    cleanup_context.uc_link = NULL;

    makecontext(&cleanup_context, free_stack, 0);

    setup_signals();
    setup_timers();
}

int uthread_create(uthread_func_t func,int val,int pri) {
    uthread_t *locked_thread = (uthread_t *) malloc(sizeof(uthread_t));

    if (getcontext(&locked_thread->context) < 0) {
        perror("getcontext\n");
        exit(-1);
    }

    locked_thread->context.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    locked_thread->context.uc_stack.ss_sp = locked_thread->stack = malloc(UTHREAD_STACK_SIZE);
    locked_thread->context.uc_link = NULL;
    
    /* priority levels are 0 (highest) to 9 (lowest) */
    if (pri < 0 || pri > 9) {
        perror("Priority must be [0,9]");
        exit(-1);
    }
    locked_thread->priority = pri;
        
    makecontext(&locked_thread->context, (void (*)(void)) func, 1, val);    

    // insert the new thread into the queue, first check if queue is empty        
    if (schedule_queue[pri].head != NULL) {
        locked_thread->next = schedule_queue[pri].head;
        schedule_queue[pri].tail->next = locked_thread;
        schedule_queue[pri].tail = locked_thread;
    }
    else {
        schedule_queue[pri].head = locked_thread;
        schedule_queue[pri].head->next = locked_thread;
        schedule_queue[pri].tail = locked_thread;
        schedule_queue[pri].tail->next = locked_thread;
    }

    schedule_queue[pri].size++;
    locked_thread->t_id = ++uthread_id;
    return uthread_id;
}

void uthread_yield(void) {
    uthread_t *old_thread = schedule_queue[priority_now].head;
    schedule_queue[priority_now].tail->next = schedule_queue[priority_now].head;    // tail->next = head
    schedule_queue[priority_now].tail = schedule_queue[priority_now].head;            // tail = head
    schedule_queue[priority_now].head = schedule_queue[priority_now].head->next;    // head = head->next
    
    update_priority_now();
    swapcontext(&old_thread->context, &schedule_queue[priority_now].head->context);
}

void uthread_exit(void) {
    uthread_t *old_thread = schedule_queue[priority_now].head;
    schedule_queue[priority_now].head = schedule_queue[priority_now].head->next;    // head = head->next
    schedule_queue[priority_now].tail->next = schedule_queue[priority_now].head;    // tail->next = head

    retired_thread = old_thread;
    setcontext(&cleanup_context);
}

void uthread_mutex_init(uthread_mutex_t *lockVar) {
    lockVar->lock_held = 0;
}

void print_lock_queue(uthread_mutex_t *lockVar) {
    int i, j;
    uthread_t *itr;
    int count = 0;
    int size;
    for(i = 0; i < PRIORITY_LEVELS; ++i){
        size = 0;
        if(lockVar->queue[i].head != NULL){
            itr = lockVar->queue[i].head;
            for(j = 0; j < lockVar->queue[i].size; ++j){
                ++count;
                ++size;
                printf("#%d\tid = %d\t pri = %d\t\n",count, itr->t_id, itr->priority);
                itr = itr->next;
                
            }
            printf("SIZE OF LEVEL %d : %d\n", i, size);
        }
    }
    printf("\n");
}

void print_schedule_queue() {
    int i, j;
    uthread_t *itr;
    int count = 0;
    for(i = 0; i < PRIORITY_LEVELS; ++i){
        if(schedule_queue[i].head != NULL){
            itr = schedule_queue[i].head;
            for(j=0; j<schedule_queue[i].size; ++j){
                ++count;
                printf("#%d\tid = %d\t pri = %d\t\n", count, itr->t_id, itr->priority);
                itr = itr->next;
            }
        }
    }
    printf("\n");
}

int get_lock_queue_size(uthread_mutex_t *lockVar) {
    int i;
    int size;
    for (i = 0; i < PRIORITY_LEVELS; ++i) {
        size += lockVar->queue[i].size;
    }
    return size;
}

void uthread_mutex_lock(uthread_mutex_t *lockVar) {
    sigprocmask(SIG_BLOCK, &set, NULL);

    if (lockVar->lock_held || (get_lock_queue_size(lockVar) > 0)) { // put a thread to sleep
        schedule_queue[priority_now].head->holds_lock = 0;
        uthread_t *old_thread = schedule_queue[priority_now].head;

        if (schedule_queue[priority_now].size == 1) {
            schedule_queue[priority_now].head = NULL;
            schedule_queue[priority_now].tail = NULL;
        }
        else { // if schedule queue is not empty, move pointers ala 'exit'
            schedule_queue[priority_now].head = schedule_queue[priority_now].head->next;
            schedule_queue[priority_now].tail->next = schedule_queue[priority_now].head;
        }

        schedule_queue[priority_now].size--;

        // append head of schedule_queue for current priority to mutex queue for same priority
        if (lockVar->queue[priority_now].size > 0) { // mutex queue for this priority is empty
            old_thread->next = lockVar->queue[priority_now].head;
            lockVar->queue[priority_now].tail->next = old_thread;
            lockVar->queue[priority_now].tail = old_thread;
            
        }
        else {
            lockVar->queue[priority_now].tail = old_thread;
            lockVar->queue[priority_now].head = old_thread;
            lockVar->queue[priority_now].head->next = old_thread;
            lockVar->queue[priority_now].tail->next = old_thread;
        }

        lockVar->queue[priority_now].size++;
    
        update_priority_now();

        sigprocmask(SIG_UNBLOCK, &set, NULL); //unblock signals
        swapcontext(&old_thread->context, &schedule_queue[priority_now].head->context);
    }

    else {
        lockVar->lock_held = 1;
        schedule_queue[priority_now].head->holds_lock = 1;
        sigprocmask(SIG_UNBLOCK, &set, NULL); //unblock signals 
    }
}

void uthread_mutex_unlock(uthread_mutex_t *lockVar) {
    sigprocmask(SIG_BLOCK, &set, NULL);

    if (schedule_queue[priority_now].head->holds_lock == 0) {
        return;
    }

    lockVar->lock_held = 0;
    schedule_queue[priority_now].head->holds_lock = 0;
    

    //check to see if there is anything waiting on the lock... return pri, or return -1
    //if there is nothing else waiting on the lock

    int next_priority;
    int i;
    for(i = 0; i < PRIORITY_LEVELS; ++i){
        if(lockVar->queue[i].size > 0){
            next_priority = i;
            break;
        }
        else{
            next_priority = -1;
        }
    }
    
    if(next_priority >= 0){
        uthread_t *thread_to_move;
        thread_to_move = lockVar->queue[next_priority].head;
        thread_to_move->holds_lock = 1;
        lockVar->lock_held = 1;
        --lockVar->queue[next_priority].size;

        if(lockVar->queue[next_priority].size == 0){
            
            lockVar->queue[next_priority].head = NULL;
            lockVar->queue[next_priority].tail = NULL;
        }
        else{
            
            lockVar->queue[next_priority].head = lockVar->queue[next_priority].head->next;
        }

        if(schedule_queue[next_priority].size == 0){
            
            schedule_queue[next_priority].head = thread_to_move;
            schedule_queue[next_priority].tail = thread_to_move;
            schedule_queue[next_priority].head->next = thread_to_move;
            schedule_queue[next_priority].tail->next = thread_to_move;
        }
        else{
            
            schedule_queue[next_priority].tail->next = thread_to_move;
            schedule_queue[next_priority].tail = thread_to_move;
            thread_to_move->next = schedule_queue[next_priority].head;
        }
        ++schedule_queue[next_priority].size;
        
    }

    sigprocmask(SIG_UNBLOCK, &set, NULL); //unblock signals
}

void uthread_mutex_destroy(uthread_mutex_t *lockVar) {
}
