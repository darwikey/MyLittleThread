#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> 
#include "link.h"


struct thread_struct{
  ucontext_t context;
  void* returned_value;
  struct thread_struct* father_thread;
};

static thread_t main_thread = NULL;
// liste de thread_t
static struct linkedlist thread_list = EmptyList;
//thread courrant
static thread_t current_thread = NULL;


// crée un thread
thread_t _impl_thread_create(){
  thread_t t = malloc(sizeof(struct thread_struct));
  t->returned_value = NULL;
  t->father_thread = NULL;

  return t;
}


// retourne si le thread est présent dans la liste de thread
int _impl_thread_is_valid(thread_t thread){
  struct listiterator it = listiterator__init_iterator(&thread_list);
  
  for (; listiterator__has_next(it); it = listiterator__goto_next(it)){
    if (listiterator__get_data(it) == thread){
      return 1;
    }
  }

  return 0; // non valide
}


// lance la fonction et stocke sa valeur retournée
void _impl_thread_launch_function(thread_t thread, void* (*function)(void*), void* parameter){
  thread->returned_value = function(parameter);  
}


thread_t thread_self(){
  return current_thread;
}


int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){

  // si le thread du main n'existe pas, on le crée 
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
    linkedlist__push_front(&thread_list, main_thread);
  }  

  // alloue le thread
  *new_thread = _impl_thread_create();
  
  // ajoute à la liste (au debut)
  linkedlist__push_front(&thread_list, *new_thread);

  // enregistrement des threads
  (*new_thread)->father_thread = current_thread;
  current_thread = *new_thread;

  // recupère le context actuelle
  getcontext(&(*new_thread)->context);

  // alloue la stack
  const size_t stack_size = 64*1024;
  (*new_thread)->context.uc_stack.ss_size = stack_size;
  (*new_thread)->context.uc_stack.ss_sp = malloc(stack_size);
  (*new_thread)->context.uc_link = &main_thread->context;

  // lance la fonction
  makecontext(&(*new_thread)->context, (void (*)(void)) _impl_thread_launch_function, 3, (*new_thread), func, funcarg);

  // sauvegarde le context du main thread et passe dans le context du nouveau thread
  swapcontext(&main_thread->context, &(*new_thread)->context);
  
  return 0;
}


int thread_yield(void){

  if (current_thread == NULL){
    return -1;
  }

  // recupère le thread en queue de file
  thread_t next_thread = NULL;
  if (linkedlist__get_size(&thread_list) > 0){
    next_thread = linkedlist__back(&thread_list);
  }

  if (next_thread == NULL){
    return -1;
  }

  thread_t previous_thread = current_thread;
  current_thread = next_thread;

  // sauvegarde le contexte du thread et revient dans le main thread
  swapcontext(&previous_thread->context, &next_thread->context);
  
  return 0;
}


int thread_join(thread_t thread, void **retval){
 
  // si le thread que l'on attend est notre propre thread
  if (current_thread == thread){
    return -1;
  }

  // si le thread n'existe plus on retourne une erreur
  if (!_impl_thread_is_valid(thread)){
    return -1;
  }

  thread_t previous_thread = current_thread;
  current_thread = thread;

  // passe au thread
  swapcontext(&previous_thread->context, &current_thread->context);

  if (retval != NULL){
    *retval = current_thread->returned_value;
  }
  
  return 0;
}


/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */
void thread_exit(void *retval){
  //signal_off();
  
  current_thread->returned_value = retval;
  //current_thread->  = 1;
  
  //passer au thread suivant
  /*if(current_thread -> next != NULL){
    thread_t temp = current_thread ; 
    current_thread = current_thread->next ;
    
    
    }*/
  
  //on enleve le dernier thread
  linkedlist__pop_back(current_thread);
  
  
  
}











