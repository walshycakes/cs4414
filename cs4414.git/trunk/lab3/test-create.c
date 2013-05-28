/* Simple test of thread create
 * and switching back to main
 */

#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"

void thread_start(int val);

int main(int argc, char **argv) {
  printf("Testing uthread_create\n");
	    
  uthread_init();
    
  uthread_create(thread_start, 1, 0); 
      
      printf("yielding to created\n");
  uthread_yield();
  printf("back in main\n");
  uthread_exit();
  printf("WE'RE DONE");
  return 0;
}

void thread_start(int val) {
  printf("In thread_start %d , control transfered\n", val);
      printf("yielding back to main\n");
  uthread_yield();
      printf("ever get here?\n");
}
