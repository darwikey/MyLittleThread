#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> 

ucontext_t main_context;
ucontext_t thread_context;



int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){

  getcontext(&thread_context);

  thread_context.uc_stack.ss_size = 64*1024;
  thread_context.uc_stack.ss_sp = malloc(thread_context.uc_stack.ss_size);
  thread_context.uc_link = &main_context;
  makecontext(&thread_context, (void (*)(void)) func, 0);

  swapcontext(&main_context, &thread_context);
  
  return 0;
}


int thread_yield(void){

  swapcontext(&thread_context, &main_context);
 
  return 0;
}
