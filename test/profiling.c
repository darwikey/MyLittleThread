#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "thread.h"
#include <sys/time.h>
/* fibonacci.
 *
 * la durée doit être proportionnel à la valeur du résultat.
 * valgrind doit être content.
 * jusqu'à quelle valeur cela fonctionne-t-il ?
 *
 * support nécessaire:
 * - thread_create()
 * - thread_join() avec récupération de la valeur de retour
 * - retour sans thread_exit()
 */

static void * fibo(void *_value)
{
  thread_t th, th2;
  int err;
  void *res = NULL, *res2 = NULL;
  unsigned long value = (unsigned long) _value;

  /* on passe un peu la main aux autres pour eviter de faire uniquement la partie gauche de l'arbre */
  thread_yield();

  if (value < 3)
    return (void*) 1;

  err = thread_create(&th, fibo, (void*)(value-1));
  assert(!err);
  err = thread_create(&th2, fibo, (void*)(value-2));
  assert(!err);

  err = thread_join(th, &res);
  assert(!err);
  err = thread_join(th2, &res2);
  assert(!err);

  return (void*)((unsigned long) res + (unsigned long) res2);
}

int main(int argc, char *argv[])
{
  unsigned long value, res, i;
  struct timeval start, finish ;
  int elapsed;

  if (argc < 2) {
    printf("argument manquant: entier x pour lequel calculer fibonacci(x)\n");
    return -1;
  }

  value = atoi(argv[1]);

  gettimeofday (&start, NULL);

  for (i = 1; i < value; i++){
    res = (unsigned long)fibo((void *)i);

    gettimeofday (&finish, NULL);
    elapsed = finish.tv_sec * 1000000 + finish.tv_usec;
    elapsed -= start.tv_sec * 1000000 + start.tv_usec;

    printf("%ld %d\n", res, elapsed);
  }

  return 0;
}
