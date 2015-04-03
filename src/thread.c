#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> 
#include "link.h"


struct thread_struct{
  ucontext_t context;
};

static thread_t main_thread = NULL;
// liste de thread_t
static struct linkedlist thread_list = EmptyList;

thread_t _impl_thread_create(){
  thread_t t = malloc(sizeof(struct thread_struct));

  return t;
}


int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){

  // si le thread du main n'existe pas, on le crée 
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
  }
  /*if (thread_list.nbElementsInList == 0){
    }*/

  // alloue le thread
  *new_thread = _impl_thread_create();
  
  // ajoute à la liste
  linkedlist__push_front(&thread_list, *new_thread);

  // recupère le context actuelle
  getcontext(&(*new_thread)->context);

  // alloue la stack
  const size_t stack_size = 64*1024;
  (*new_thread)->context.uc_stack.ss_size = stack_size;
  (*new_thread)->context.uc_stack.ss_sp = malloc(stack_size);
  (*new_thread)->context.uc_link = &main_thread->context;
  makecontext(&(*new_thread)->context, (void (*)(void)) func, 0);

  swapcontext(&main_thread->context, &(*new_thread)->context);
  
  return 0;
}


int thread_yield(void){
  if (linkedlist__get_size(&thread_list) < 1){
    return -1;
  }

  thread_t current_thread = linkedlist__back(&thread_list);

  if (current_thread == NULL){
    return -1;
  }

  swapcontext(&current_thread->context, &main_thread->context);
  
  return 0;
}


int thead_join(thread_t thread, void **retval){

  
  return 0;
}
