#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> 
#include "link.h"

static pthread_t FATHER_THREAD = 0;


struct thread_struct{
  ucontext_t context;
};

static thread_t main_thread = NULL;
// liste de thread_t
static struct linkedlist thread_list = EmptyList;
//thread courrant
static thread_t current_thread = NULL;


thread_t _impl_thread_create(){
  thread_t t = malloc(sizeof(struct thread_struct));

  return t;
}


thread_t thread_self(){
  /*if (linkedlist__get_size(&thread_list) < 1){
    return NULL;
  }

  // recupère le thread courrant, celui qui est en haut de la liste
  return linkedlist__back(&thread_list);*/

  return current_thread;
}


int thread_create(thread_t* new_thread,  void *(*func)(void *), void *funcarg){

  // si le thread du main n'existe pas, on le crée 
  if (main_thread == NULL){
    main_thread = _impl_thread_create();
    linkedlist__push_front(&thread_list, main_thread);
  }

	//enregistrement du thread pere
	FATHER_THREAD = thread_self();

  // alloue le thread
  *new_thread = _impl_thread_create();
  
  // ajoute à la liste (au debut)
  linkedlist__push_front(&thread_list, *new_thread);

  current_thread = *new_thread;

  // recupère le context actuelle
  getcontext(&(*new_thread)->context);

  // alloue la stack
  const size_t stack_size = 64*1024;
  (*new_thread)->context.uc_stack.ss_size = stack_size;
  (*new_thread)->context.uc_stack.ss_sp = malloc(stack_size);
  (*new_thread)->context.uc_link = &main_thread->context;
  makecontext(&(*new_thread)->context, (void (*)(void)) func, 0);

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

  // sauvegarde le contexte du thread et revient dans le main thread
  swapcontext(&current_thread->context, &next_thread->context);
  
  return 0;
}


int thread_join(thread_t thread, void **retval){
   // recupère le thread courrant
  /*  thread_t current_thread = thread_self();

  if (current_thread == NULL){
    return -1;
    }*/

  current_thread = thread;

  // passe au thread
  swapcontext(&main_thread->context, &current_thread->context);
  
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
extern void thread_exit(void *retval){
	signal_off();
	thread_t current_thread = thread_self();
	current_thread-> retval = retval;
	//current_thread->  = 1;
	
	//passer au thread suivant
	if(current_thread -> next != NULL){
	thread_t temp = current_thread ; 
	current_thread = current_thread->next ;
	
	
	}

	//on enleve le dernier thread
	linkedlist__pop_back(current_thread);
	
	

}











