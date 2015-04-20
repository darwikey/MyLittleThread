#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"

static void* f(void* i) {
  char c[1000];
  f(i);
}

int main() {
  thread_t thread;
  int err;
  unsigned long value = 0;
  void *res = NULL;

  err = thread_create(&thread, f, (void*)(value));
  assert(!err);

  err = thread_join(thread, &res);
  assert(!err);

  return 0;
}
