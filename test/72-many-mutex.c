#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "thread.h"
#include "mutex.h"


struct mutex_t mutex;
int resource = 0;


static void * thfunc(void *_nbyield)
{
  unsigned long i, j;
  int job = rand();

  for(i=0; i<100; i++){
    mutex_lock(&mutex);
    
    resource = job;

    for(j=0; j<1000; j++){
      thread_yield();
      assert(resource == job);
    }

    mutex_unlock(&mutex);    
  }

  return NULL;
}

int main(int argc, char *argv[])
{
  int nbth, i, err;
  thread_t *ths;
  
  struct timeval tv1, tv2;
  unsigned long us;

  if (argc < 2) {
    printf("arguments manquants: nombre de threads\n");
    return -1;
  }
  
  srand(time(NULL));

  mutex_init(&mutex, MUTEX_NORMAL);

  nbth = atoi(argv[1]);

  ths = malloc(nbth * sizeof(thread_t));
  assert(ths);

  gettimeofday(&tv1, NULL);

  for(i=0; i<nbth; i++) {

    err = thread_create(&ths[i], thfunc, NULL);
    assert(!err);
  }

  for(i=0; i<nbth; i++) {
    void *res;
    err = thread_join(ths[i], &res);
    assert(!err);
    assert(res == NULL);
  }

  gettimeofday(&tv2, NULL);
  us = (tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
  printf("%d threads: %ld us\n",
	  nbth, us);

  free(ths);

  return 0;
}
