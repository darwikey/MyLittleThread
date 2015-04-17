#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include "mutex.h"

pthread_t tid[2];
int counter;
struct mutex_t lock;
int choix;

void* doSomeThing(void *arg)
{
  int i;
  if(choix == 1)
    mutex_lock(&lock);

  counter += 1;
  printf("\n Job %d started\n", counter);

  for(i = 0 ; i < 10000000 ; i++){}

  printf("\n Job %d finished\n", counter);

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
      if (mutex_init(&lock, &my_attr) != 0){
	  printf("\n mutex init failed\n");
	  return 1;
	}
    }

    while(i < 2)
      {
	err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
	if (err != 0)
	  printf("\ncan't create thread :[%s]", strerror(err));
	i++;
      }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    if(choix == 1)
      mutex_destroy(&lock);

    return 0;
  }
