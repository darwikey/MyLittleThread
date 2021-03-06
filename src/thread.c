#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h> 
#include <valgrind/valgrind.h>
#include <time.h>
#include <signal.h>
#include "link.h"
#include "link.inl"
#include <sys/mman.h>
#include <malloc.h>

#ifndef STACK_SIZE
#define STACK_SIZE (5 * 1024)
#endif

void thread_stack_overflow();
void thread_stack_overflow_detected();

struct thread_struct{
  ucontext_t context;
  void* returned_value;
  struct thread_struct* father_thread;
  int valgrind_stackid;
  int is_valid;
  int is_waiting;
};

static thread_t main_thread = NULL;
// liste de thread_t
static struct linkedlist thread_list = EmptyList;
//thread courrant
static thread_t current_thread = NULL;
// verrou pour la préemption
static int lock_preempt = 0;


void _impl_thread_lock(){
  lock_preempt = 1;
}


void _impl_thread_unlock(){
  lock_preempt = 0;
} 


// crée un thread
thread_t _impl_thread_create(){
  thread_t t = malloc(sizeof(struct thread_struct));

  if (t == NULL){
    perror("malloc");
  }

  t->returned_value = NULL;
  t->father_thread = NULL;
  t->valgrind_stackid = 0;
  t->is_valid = 1;
  t->is_waiting = 0;

  return t;
}


void _impl_thread_delete(thread_t thread){
  if (thread->context.uc_stack.ss_sp != NULL){

    VALGRIND_STACK_DEREGISTER(thread->valgrind_stackid);

    free(thread->context.uc_stack.ss_sp);
  }

  free(thread);
}


// retire le thread courant de la liste de threads actifs  
void _impl_thread_disable_current(void){
  assert(linkedlist__back(&thread_list) == current_thread);
  linkedlist__pop_back(&thread_list);
}


// ajoute un thread a la liste des threads
void _impl_thread_add(thread_t thread){
  linkedlist__push_front(&thread_list, thread);
}


// retourne si le thread est présent dans la liste de thread
int _impl_thread_is_valid(thread_t thread){
  //struct listiterator it = listiterator__find_data(&thread_list, thread);

  return thread->is_valid;//listiterator__is_valide(it); // non valide
}


// lance la fonction et stocke sa valeur retournée
void _impl_thread_launch_function(thread_t thread, void* (*function)(void*), void* parameter){
  _impl_thread_unlock();

  thread->returned_value = function(parameter);

  thread_exit(thread->returned_value);
}


// fonction appelée à intervalle régulier
void _impl_thread_preempt_handler(int signal){
  if (signal == SIGALRM && !lock_preempt){
    thread_yield();
  }
}

//init le premier thread
void _impl_thread_init_main(void){
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
    linkedlist__push_front(&thread_list, main_thread);
    current_thread = main_thread;
  }

#ifdef THREAD_PREEMPT
  // enregistre la fonction de préemption
  struct sigaction act;
  act.sa_handler = _impl_thread_preempt_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if(sigaction(SIGALRM, &act, 0) == -1){
    perror("sigaction");
  }

  ualarm(THREAD_PREEMPT_INTERVAL, THREAD_PREEMPT_INTERVAL);
#endif
}


thread_t thread_self(void){
  _impl_thread_lock();

  if (current_thread == NULL){
    _impl_thread_init_main();
  }

  _impl_thread_unlock();
  return current_thread;
}


int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){
  _impl_thread_lock();
  thread_t previous_thread = current_thread;

  // si le thread du main n'existe pas, on le crée 
  if (main_thread == NULL){
    _impl_thread_init_main();
    
    previous_thread = main_thread;
  }  

  // alloue le thread
  *new_thread = _impl_thread_create();
  
  // ajoute à la liste (au debut)
  linkedlist__push_back(&thread_list, *new_thread);

  // enregistrement des threads
  (*new_thread)->father_thread = current_thread;
  current_thread = *new_thread;

  // recupère le context actuelle
  getcontext(&(*new_thread)->context);

  // alloue la stack
  const size_t stack_size = STACK_SIZE;
  (*new_thread)->context.uc_stack.ss_size = stack_size;
  (*new_thread)->context.uc_stack.ss_sp = malloc(stack_size);
  if ((*new_thread)->context.uc_stack.ss_sp == NULL){
    perror("malloc");
  }
  (*new_thread)->valgrind_stackid = VALGRIND_STACK_REGISTER((*new_thread)->context.uc_stack.ss_sp,
							    (*new_thread)->context.uc_stack.ss_sp + stack_size);
 
  //Cas débordement de pile
  thread_stack_overflow();


  // lance la fonction
  makecontext(&(*new_thread)->context, (void (*)(void)) _impl_thread_launch_function, 3, (*new_thread), func, funcarg);

  // sauvegarde le context du main thread et passe dans le context du nouveau thread
  swapcontext(&previous_thread->context, &(*new_thread)->context);

  _impl_thread_unlock();

  
  return 0;
}


int thread_yield(void){

  _impl_thread_lock();

  if (current_thread == NULL){
    _impl_thread_init_main();
  }


  // on passe le thread de la queue à la tete de la liste
  if (linkedlist__get_size(&thread_list) > 1){  
    if (current_thread == linkedlist__back(&thread_list)){
      linkedlist__pop_back(&thread_list);
      linkedlist__push_front(&thread_list, current_thread);
    }
  }

  // recupère le thread en queue de file
  thread_t next_thread = linkedlist__back(&thread_list);

  thread_t previous_thread = current_thread;
  current_thread = next_thread;

  // sauvegarde le contexte du thread et revient dans le main thread
  
  swapcontext(&previous_thread->context, &next_thread->context);
  
  _impl_thread_unlock();

  return 0;
}


int thread_join(thread_t thread, void **retval){
 
  _impl_thread_lock();

  // si le thread que l'on attend est notre propre thread
  /*if (current_thread == thread){
    return -1;
    }*/

  
 
  while(_impl_thread_is_valid(thread)){
    thread->father_thread = current_thread;
    current_thread->is_waiting = 1;

    _impl_thread_disable_current();

    thread_yield();
    _impl_thread_lock();

    current_thread->is_waiting = 0;
  }

  
  if (retval != NULL){
    *retval = thread->returned_value;
  }

  //libère memoire  
  _impl_thread_delete(thread);
  _impl_thread_unlock();
  return 0;
}


void thread_exit(void *retval){
  
  _impl_thread_lock();

  current_thread->is_valid = 0;
  
  // stocke la valeur retournée
  current_thread->returned_value = retval;
  

  assert(linkedlist__back(&thread_list) == current_thread);
  linkedlist__pop_back(&thread_list);


  //Ajout du pere en fin de queue
  if (current_thread->father_thread != NULL){
    // si le pere attend, on doit le rajouter dans la liste de threads
    if (current_thread->father_thread->is_waiting){
      //assert(!listiterator__is_valide(listiterator__find_data(&thread_list, current_thread->father_thread)));
      linkedlist__push_back(&thread_list, current_thread->father_thread);
      //current_thread = current_thread->father_thread;
    }
  }		


  // s'il n'y a plus de thread disponible
  if (linkedlist__get_size(&thread_list) < 1){
    exit(EXIT_SUCCESS);
  }

  // passe au thread du pere
  thread_yield();

  assert(0);
}

stack_t ss = {0};
struct sigaction sa;
static char * buffer;
int bool = 1;

void thread_stack_overflow() {

  // Pile pour gérér les signaux si la pile du thread est pleine
  if(ss.ss_sp == NULL)
    ss.ss_sp = malloc(8000);
  ss.ss_size = 8000;
  ss.ss_flags = 0;
  //ss.ss_size=SIGSTKSZ;
  sigaltstack(&ss, NULL);

  // Déroutement
  sa.sa_handler = thread_stack_overflow_detected; // gestionnaire de signal 
  sa.sa_flags = SA_ONSTACK;
  sigaction(SIGSEGV, &sa, NULL);

  /* Protection mémoire thread */

  int pagesize = sysconf(_SC_PAGE_SIZE);
  buffer = memalign(pagesize, 4*pagesize);
  mprotect(buffer + pagesize*2, pagesize, PROT_NONE);


}

void thread_stack_overflow_detected() {
  if(bool) {
    /* Appeler cette fonction une seule fois */
    bool=0;
    printf("overflow  detected \n");
    //Note: l'attribut father_thread n'est pas initialisé
    
    //int i;
    
    struct listnode * thread=thread_list.backNode;
    
    current_thread -> is_valid=0;

    thread_t ancient_thread = thread_list.backNode->data;
    ancient_thread = (thread_t) listnode__get_previous(thread)->data;
  
    /* Debuggage */
  
    /*
      printf("current thread: %p \n", current_thread);
      printf("ancient_thread: %p \n", ancient_thread);
      printf("main_thread: %p \n", main_thread);
    */

    /*
      for(i=0; i<thread_list.nbElementsInList; i++) {
      printf("thread: %p \n", thread->data);
      thread = listnode__get_previous(thread);
      }
    */


    /* Quitte tout */
      //exit(EXIT_FAILURE);

    printf("changement de contexte \n");
    swapcontext(&current_thread->context, &ancient_thread->context);

    printf ("retour dans le main \n"); /* Ne sera jamais executé */

  }
}
