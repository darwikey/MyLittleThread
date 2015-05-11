#ifndef __MUTEX_H__
#define __MUTEX_H__

#ifndef USE_PTHREAD
struct linkedlist;

enum attr {MUTEX_NORMAL, MUTEX_RECURSIVE, MUTEX_ERRORCHECK};

/* structure de mutex
 */
struct mutex_t{
  int state; // 0 = non initialisé, 1 = verrouillé, 2 = déverrouillé
  enum attr attr; //attribut
  struct linkedlist* waiting_thread;
};


/* Créer un nouveau mutex
   Renvoie 0 en cas de succès, -1 en cas d'erreur
*/
int mutex_init(struct mutex_t * mutex, enum attr attr);


/* Détruit le mutex référencé par mutex
   Retourne 0 si tout s'est bien passé, -1 en cas d'erreur
*/
int mutex_destroy(struct mutex_t * mutex);


/* Verrouille le mutex
   Retourne 0 si tout se passe bien
   -1 si le mutex n'était pas initialisé
   -2 si le mutex était déjà verrouillé
*/
int mutex_lock(struct mutex_t * mutex);


/* Verrouille le mutex.
   Retourne immédiatement si le mutex est déjà verrouillé
*/
int mutex_trylock(struct mutex_t * mutex);


/* Déverrouille le mutex  
   Retourne 0 si tout se passe bien
   -1 si le mutex n'était pas initialisé
   -2 si le mutex était déjà déverrouillé
*/
int mutex_unlock(struct mutex_t * mutex);

#else /* USE_PTHREAD */

/* Si on compile avec -DUSE_PTHREAD, ce sont les pthreads qui sont utilisés */
#include <sched.h>
#include <pthread.h>
#define mutex_t pthread_mutex_t
#define MUTEX_NORMAL PTHREAD_MUTEX_NORMAL
#define MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE
#define MUTEX_ERRORCHECK PTHREAD_MUTEX_ERRORCHECK
#define mutex_init pthread_mutex_init
#define mutex_destroy pthread_mutex_destroy
#define mutex_lock pthread_mutex_lock
#define mutex_trylock pthread_mutex_trylock
#define mutex_unlock pthread_mutex_unlock
#define thread_self pthread_self


#endif /* USE_PTHREAD */

#endif /* __MUTEX_H__ */
