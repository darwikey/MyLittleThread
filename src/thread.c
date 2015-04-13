#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> 
#include "link.h"

void thread_stack_overflow();
void thread_stack_overflow_detected();

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
  /*struct listiterator it = listiterator__init_iterator(&thread_list);
  
  if (linkedlist__get_size(&thread_list) <= 1){
    return 0;
  }

  for (; listiterator__is_valide(it); it = listiterator__goto_next(it)){
    if (listiterator__get_data(it) == thread){
      return 1;
    }
    }*/
  struct listiterator it = listiterator__find_data(&thread_list, thread);

  return listiterator__is_valide(it); // non valide
}


// supprime un thread de la liste de thread
void _impl_thread_delete(thread_t thread){

  struct listiterator it = listiterator__find_data(&thread_list, thread);

  if (listiterator__is_valide(it)){
    listiterator__remove_node(it);
  }


  //TODO delete stack
}


// lance la fonction et stocke sa valeur retournée
void _impl_thread_launch_function(thread_t thread, void* (*function)(void*), void* parameter){
  thread->returned_value = function(parameter);

  _impl_thread_delete(thread);
}


void _impl_thread_init_main(void){
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
    linkedlist__push_front(&thread_list, main_thread);
    current_thread = main_thread;
  }
}


thread_t thread_self(void){
  if (current_thread == NULL){
    _impl_thread_init_main();
  }
  return current_thread;
}


int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){

  // si le thread du main n'existe pas, on le crée 
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
    current_thread = main_thread;
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
    _impl_thread_init_main();
  }

  // recupère le thread en queue de file
  thread_t next_thread = NULL;
  if (linkedlist__get_size(&thread_list) > 0){
    next_thread = linkedlist__back(&thread_list);
  }

  if (next_thread == NULL){
    return -1;
  }

  // on passe le thread de la queue à la tete de la liste
  linkedlist__pop_back(&thread_list);
  linkedlist__push_front(&thread_list, next_thread);

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
  while (_impl_thread_is_valid(thread)){
    printf("yield");
    thread_yield();
  }



  //thread_t previous_thread = current_thread;
  //current_thread = thread;

  // passe au thread
  //swapcontext(&previous_thread->context, &current_thread->context);

  if (retval != NULL){
    *retval = thread->returned_value;
  }
  
  return 0;
}


void thread_exit(void *retval){
  
  current_thread->returned_value = retval;
  
  _impl_thread_delete(current_thread);

  assert(current_thread->father_thread);

  // passe au thread du pere
  swapcontext(&current_thread->context, &current_thread->father_thread->context);
 
  assert(0);
}


void thread_stack_overflow() {
  
  // Pile pour gérér les signaux si la pile du thread est pleine
  stack_t ss;
  ss.ss_sp = malloc(SIGSTKSZ);
  ss.ss_size = SIGSTKSZ;
  ss.ss_flags = 0;
  ss.ss_size=SIGSTKSZ;
  sigaltstack(&ss, NULL);
  
  // Déroutement
  struct sigaction sa;
  sa.sa_handler = thread_stack_overflow_detected; // gestionnaire de signal 
  sa.sa_flags = SA_ONSTACK;
  sigaction(SIGSEGV, &sa, NULL);

}

void thread_stack_overflow_detected() {
  
  thread_t current_thread = thread_self();

  //On s'assure de ne pas tuer le thread courant
  thread_t ancient_thread = current_thread;
  current_thread = current_thread->father_thread;
  swapcontext(&ancient_thread->context, &current_thread->context);
  _impl_thread_delete(ancient_thread);

}
