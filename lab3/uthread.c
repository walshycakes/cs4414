#include <ucontext.h>
#include "uthread.h"

/*
	int getcontext(ucontext_t *ucp);
	int setcontext(const ucontext_t *ucp);
	
	sys/ucontext.h
	typedef struct ucontext
	{
		unsigned long int uc_flags;
		struct ucontext *uc_link;
		stack_t uc_stack;
		mcontext_t uc_mcontext;
		__sigset_t uc_sigmask;
		struct _libc_fpstate __fpregs_mem;
	} ucontext_t; 

	uc_link points to context that will be resumed
	when the current context terminates

	uc_stack is the stack used by this context

	uc_mcontext is machine-specific (registers)

	uc_sigmask is set of signals blocked in this context
*/

ucontext_t ucontext;

void uthread_init() {
}

int uthread_create(void (*func)(int), int val, int pri) {
	return 0;
}

void uthread_yield() {
}

void uthread_exit() {
}
