#include "mutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "link.h"

// pour opérations atomiques (nécessaire si on gère plusieurs coeurs) -> C11 (prof) ou openPA (perso).

void _impl_thread_disable_current(void);
void _impl_thread_add(thread_t);


int mutex_init(struct mutex_t * mutex, enum attr attr){
  
  if (attr !=  MUTEX_NORMAL && attr != MUTEX_RECURSIVE && attr != MUTEX_ERRORCHECK) // attr n'est pas l'une des 3 valeurs supportées
    return -1;
    
  else // attr != NULL et est l'une des 3 valeurs supportées
    mutex->attr = attr;  
  
  mutex->state = 2; // déverrouillé
  mutex->waiting_thread = NULL;

  return 0;
}

int mutex_destroy(struct mutex_t * mutex){
  // && mutex_t.attr == MUTEX_ERRORCHECK à laisser ? ou tout le tps check ?
  if(mutex->state == 1 && mutex->attr == MUTEX_ERRORCHECK){ // essai de détruire un mutex verrouillé, comportement indéfini
    fprintf(stderr, "Attention, vous avez essayé de détruire un mutex verrouillé\n");
    return -1;
  }
 
  mutex->state = 0; // détruire = désinitialiser
  return 0;
}

int mutex_lock(struct mutex_t * mutex){
  if(mutex->attr ==  MUTEX_ERRORCHECK){

    if (mutex->state == 1){ // déjà verrouillé
      fprintf(stderr, "Tentative de verrouillage de mutex déjà verrouillé.\n");
      return -2;
    }

    if(mutex->state == 0){ // non initialisé
      fprintf(stderr, "Tentative de verrouillage de mutex non initialisé.\n");
      return -1;
    }
  }
  
  
  // on attend la ressource 
  while(mutex->state == 1){
    
    // si la liste n'existe pas
    if (mutex->waiting_thread == NULL){
      mutex->waiting_thread = linkedlist__alloc_empty();

      if (mutex->waiting_thread == NULL)
	perror("malloc list");
    }

    linkedlist__push_front(mutex->waiting_thread, thread_self());
    _impl_thread_disable_current();
  
    thread_yield();
  }

  // on verrouille
  mutex->state = 1; 

  return 0;
}


int mutex_trylock(struct mutex_t * mutex){
  if(mutex->state == 1) // si verrouillé
    return 0;

  else 
    return mutex_lock(mutex);   

}

int mutex_unlock(struct mutex_t * mutex){
  
  if(mutex->attr == MUTEX_ERRORCHECK && mutex->state == 2){ // déjà déverrouillé
    fprintf(stderr, "Tentative de déverrouillage de mutex déjà déverrouillé.\n");
    return -2;
  }

  else if(mutex->attr == MUTEX_ERRORCHECK && mutex->state == 0){ // non initialisé
    fprintf(stderr, "Tentative de déverrouillage de mutex non initialisé.\n");
    return -1;
  }

  
  mutex->state = 2; //déverrouillé
  
  if (mutex->waiting_thread != NULL){
    if (linkedlist__get_size(mutex->waiting_thread) > 0){
      thread_t t = (thread_t)linkedlist__back(mutex->waiting_thread);
      linkedlist__pop_back(mutex->waiting_thread);
      _impl_thread_add(t);
    }
    else{
      //détruit la liste si elle est vide
      free(mutex->waiting_thread);
      mutex->waiting_thread = NULL;
    }
  }


  return 0;
}
