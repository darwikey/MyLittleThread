#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "thread.h"
#include "mutex.h"

thread_t tid[2];
int counter = 0;
struct mutex_t lock;
int choix = 0;

void* doSomeThing(void *arg)
{
  int i;
  int job;
  job = counter ++;

  if(choix == 1)
    mutex_lock(&lock);

  printf("\n Job %d started\n", job);

  for(i = 0 ; i < 10000000 ; i++){}


  printf("\n Job %d finished\n", job);

  if(choix == 1)
    mutex_unlock(&lock);

  return NULL;
}

int main(void)
{
  int i = 0;
  int err;
  enum attr my_attr = MUTEX_ERRORCHECK;
  printf("Avec (1) ou sans (0) mutex ?");
  scanf("%d", &choix);

  if(choix == 1){
    printf("type de mutex? (0 = normal, 1 = recursive, 2 = errorcheck)");
    scanf("%d", (int*)&my_attr);
  }

  if(choix == 1){
    if (mutex_init(&lock, my_attr) != 0){
      printf("\n mutex init failed\n");
      return 1;
    }
  }
  
  for(i = 0; i < 2; i++){
    
    err = thread_create(&(tid[i]), &doSomeThing, NULL);
    assert(!err);

  }
  
  thread_join(tid[0], NULL);
  thread_join(tid[1], NULL);
  if(choix == 1)
    mutex_destroy(&lock);
  
  return 0;
}
