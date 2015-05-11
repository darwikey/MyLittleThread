#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"

static void* f(void* i) {
  unsigned long c[1000];
  c[0]=1;
  f((void*) c[0]);
  return i;
}


int main() {
  unsigned long value = 0;
  void *res = NULL;

  thread_t thread;
  int err;

  err = thread_create(&thread, f, (void*)(value));
  assert(!err);
  
  err = thread_join(thread, &res);
  assert(!err);

  printf("retour main \n");

  return 0;
}
