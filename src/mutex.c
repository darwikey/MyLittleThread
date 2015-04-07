#include "mutex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>


struct mutex_struct mutex_struct;


ucontext_t main_context;
ucontext_t mutex_context;


mutex_unitialized(mutex_struct * mutex_struct){
  mutex_struct.mutex = NULL; // pas forcément nécessaire
  mutex_struct.state = 0; // unitialized
  mutex_struct.attr = NULL; // pas forcément nécessaire
}


int mutex_init(mutex_t * mutex, const mutex_attr_t attr){
  
  if (attr != NULL){
    if (strcmp(attr, "MUTEX_NORMAL") != 0 && strcmp(attr, "MUTEX_RECURSIVE") != 0 && strcmp(attr, "MUTEX_ERRORCHECK") != 0) // attr n'est pas l'une des 3 valeurs supportées, ni NULL
      return -1;
    
    else // attr != NULL et est l'une des 3 valeurs supportées
      mutex_struct.attr = attr;
  }    

  else  // attr == NULL    
    mutex_struct.attr = "MUTEX_NORMAL";

  mutex_struct.mutex = mutex; 
  mutex_struct.state = 2; // unlocked
  return 0;
}

int mutex_destroy(mutex_t * mutex){
  // && mutex_struct.attr == "MUTEX_ERRORCHECK" à laisser ? ou tout le tps check ?
  if(mutex_struct.state == 1 && mutex_struct.attr == "MUTEX_ERRORCHECK"){ // essai de détruire un mutex verrouillé, comportement indéfini
    printf("Attention, vous avez essayé de détruire un mutex verrouillé\n");
    return -1;
  }
 
  mutex_struct.state = 0; // unitialized
  return 0;
}

int mutex_lock(mutex_t * mutex){
  if(strcmp(mutex_struct.attr, "MUTEX_ERRORCHECK") == 0){

    if(mutex_struct.state == 1) // already locked
      return -2;

    else if(mutex_struct.state == 0) // unitialized
      return -1;
  }
  
  mutex_struct.state = 1; // locked
  return 0;
}

int mutex_trylock(mutex_t * mutex){
  if(mutex_struct.state == 1) // if locked
    return 0;

  else mutex_lock(mutex);   

}

int mutex_unlock(mutex_t * mutex){

  if(strcmp(mutex_struct.attr, "MUTEX_ERRORCHECK") == 0){

    if(mutex_struct.state == 2) // already unlocked
      return -2;

    else if(mutex_struct.state == 0) // unitialized
      return -1;
  }

  



}
