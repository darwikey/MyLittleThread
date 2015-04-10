#include "mutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// pour opérations atomiques (nécessaire si on gère plusieurs coeurs) -> C11 (prof) ou openPA (perso).


static struct mutex_struct mutex_struct = {.mutex=NULL, .state=0, .attr=MUTEX_NORMAL};


int mutex_init(mutex_t * mutex, const enum attr * attr){
  
  if (attr != NULL){
    if (*attr !=  MUTEX_NORMAL && *attr != MUTEX_RECURSIVE && *attr != MUTEX_ERRORCHECK) // attr n'est pas l'une des 3 valeurs supportées, ni NULL
      return -1;
    
    else // attr != NULL et est l'une des 3 valeurs supportées
      mutex_struct.attr = *attr;
  }    

  else  // attr == NULL    
    mutex_struct.attr = MUTEX_NORMAL;

  mutex_struct.mutex = mutex; 
  mutex_struct.state = 2; // déverrouillé
  //printf("mutex_init\n");
  return 0;
}

int mutex_destroy(mutex_t * mutex){
  // && mutex_struct.attr == MUTEX_ERRORCHECK à laisser ? ou tout le tps check ?
  if(mutex_struct.state == 1 && mutex_struct.attr == MUTEX_ERRORCHECK){ // essai de détruire un mutex verrouillé, comportement indéfini
    printf("Attention, vous avez essayé de détruire un mutex verrouillé\n");
    return -1;
  }
 
  mutex_struct.state = 0; // détruire = désinitialiser
  return 0;
}

int mutex_lock(mutex_t * mutex){
  if(mutex_struct.attr ==  MUTEX_ERRORCHECK && mutex_struct.state == 1){ // déjà verrouillé
    printf("Tentative de verrouillage de mutex déjà verrouillé.\n");
    while(mutex_struct.state == 1);
    return -2;
  }

  else if(mutex_struct.attr ==  MUTEX_ERRORCHECK && mutex_struct.state == 0){ // non initialisé
    printf("Tentative de verrouillage de mutex non initialisé.\n");
    return -1;
  }
  //printf("mutex_lock\n");
  
  while(mutex_struct.state == 1); // on bloque la ressource (pb : attente active -> 100% de proc)
  mutex_struct.state = 1; // on verrouille
  return 0;
}

int mutex_trylock(mutex_t * mutex){
  if(mutex_struct.state == 1) // si verrouillé
    return 0;

  else return mutex_lock(mutex);   

}

int mutex_unlock(mutex_t * mutex){
  
  if(mutex_struct.attr == MUTEX_ERRORCHECK && mutex_struct.state == 2){ // déjà déverrouillé
    printf("Tentative de déverrouillage de mutex déjà déverrouillé.\n");
    return -2;
  }

  else if(mutex_struct.attr == MUTEX_ERRORCHECK && mutex_struct.state == 0){ // non initialisé
    printf("Tentative de déverouillage de mutex non initialisé.\n");
    return -1;
  }

  else{
    mutex_struct.state = 2;
    return 0;
  }
}
