#include "mutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// pour opérations atomiques (nécessaire si on gère plusieurs coeurs) -> C11 (prof) ou openPA (perso).


static struct mutex_t mutex = {.state=0, .attr=MUTEX_NORMAL};


int mutex_init(struct mutex_t * mutex, const enum attr * attr){
  
  if (attr != NULL){
    if (*attr !=  MUTEX_NORMAL && *attr != MUTEX_RECURSIVE && *attr != MUTEX_ERRORCHECK) // attr n'est pas l'une des 3 valeurs supportées, ni NULL
      return -1;
    
    else // attr != NULL et est l'une des 3 valeurs supportées
      mutex->attr = *attr;
  }    

  else  // attr == NULL    
    mutex->attr = MUTEX_NORMAL;

  mutex->state = 2; // déverrouillé
  //printf("mutex_init\n");
  return 0;
}

int mutex_destroy(struct mutex_t * mutex){
  // && mutex_t.attr == MUTEX_ERRORCHECK à laisser ? ou tout le tps check ?
  if(mutex->state == 1 && mutex->attr == MUTEX_ERRORCHECK){ // essai de détruire un mutex verrouillé, comportement indéfini
    printf("Attention, vous avez essayé de détruire un mutex verrouillé\n");
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
    fprintf(stderr, "Tentative de déverouillage de mutex non initialisé.\n");
    return -1;
  }

  else{
    mutex->state = 2;
    return 0;
  }
}
