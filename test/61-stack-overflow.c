#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"

static void* f(void* i) {
  printf("encore de la place\n");
  unsigned long c[100];
  c[0]=1;
  f((void*) c[0]);
  return i;
}


static void* g(void *i) {
  thread_t thread;
  int err;
  void * res = NULL;

  err = thread_create(&thread, f, (void*)(i));
  assert(!err);

  printf("retour fonction g \n");

  err = thread_join(thread, &res);
  assert(!err);
  
  printf("fin fct g\n");

  return res;
  
  }

int main() {
  unsigned long value = 0;
  void *res = NULL;

  thread_t thread;
  int err;

  err = thread_create(&thread, g, (void*)(value));
  assert(!err);
  
  printf("retour main\n");
  
  //err = thread_join(thread, &res);
  //assert(!err);

  printf("fin main \n");

  return 0;
}
