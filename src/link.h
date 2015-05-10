//////////////////////////////////////////////////////////////////////////////
// Berenger Bramas
// An open source (under LGPL licence) double linked list in C
//
// Since C does not have standard containers, it is sometime useful to have
// access to a list (which should be simple and easy to understand and
// without bug).
//
// There is two part in this file, one for the declaration and another for the
// implementation (it has been put in one file to let you import it easily
// in your project.
// You can find unit test on the blog.
//
// There a list node, a list and a list iterator.
//////////////////////////////////////////////////////////////////////////////
#ifndef LINK_H
#define LINK_H

//////////////////////////////////////////////////////////////////////////////

/** A simple node double linkely */
struct listnode {
    //< Any type of data
    void* data;
    //< Address of previous (or null)
    struct listnode *previous;
    //< Address of next (or null)
    struct listnode *next;
};

/** Alloc and init a list */
struct listnode* listnode__alloc_empty(void);
/** Alloc and init a list with the given data */
struct listnode* listnode__alloc_init_withData(void* data);

/** Return the next node from the current node */
struct listnode* listnode__get_next(struct listnode* node);
/** Return the previous node from the current node */
struct listnode* listnode__get_previous(struct listnode* node);
/** Return the data from the current node */
void* listnode__get_data(struct listnode* node);

/** Set the next node to the current node */
void listnode__set_next(struct listnode* node, struct listnode* next);
/** Set the previous node to the current node */
void listnode__set_previous(struct listnode* node, struct listnode* previous);
/** Set the data to the current node */
void listnode__set_data(struct listnode* node, void* data);

//////////////////////////////////////////////////////////////////////////////

/** A linked list with pointer to the head and the back */
struct linkedlist {
    int nbElementsInList;
    struct listnode *headNode;
    struct listnode *backNode;
};

/** Inline declaration as : struct linkedlist aList = EmptyList; */
#define EmptyList {             \
    .nbElementsInList = 0,      \
    .headNode = 0,              \
    .backNode = 0               \
    }

/** Init a list that has already been allocate */
void linkedlist__init(struct linkedlist* list);
/** Allocate and init a list */
struct linkedlist* linkedlist__alloc_empty(void);
/** Free all nodes and reset an list */
void linkedlist__free_nodes(struct linkedlist * list);
/** Free the list and all nodes */
void linkedlist__free_list_and_nodes(struct linkedlist * list);

/** Push a value in the front of the list */
void linkedlist__push_front(struct linkedlist* list, void* data);
/** Push a value in the back of the list */
void linkedlist__push_back(struct linkedlist* list, void* data);

/** Remove the first value */
void linkedlist__pop_front(struct linkedlist* list);
/** Remove the last value */
void linkedlist__pop_back(struct linkedlist* list);

/** Get the first value */
void* linkedlist__back(struct linkedlist* list);
/** Get the last value */
void* linkedlist__front(struct linkedlist* list);

/** Get the number of elements in the list */
int linkedlist__get_size(struct linkedlist* list);

//////////////////////////////////////////////////////////////////////////////

/** A iterator on the list */
struct listiterator {
    //< Current pointed item
    struct listnode* workingNode;
    //< Target list
    struct linkedlist* workingList;
};

/** Build an iterator from a list */
struct listiterator listiterator__init_iterator(struct linkedlist* list);
/** Goto the next element and return the updated iterator (there must be a next) */
struct listiterator listiterator__goto_next(struct listiterator iterator);
/** Goto the previous element and return the updated iterator (there must be a previous) */
struct listiterator listiterator__goto_previous(struct listiterator iterator);

/** Return 1 if the iterator is valide , else 0*/
int listiterator__is_valide(struct listiterator iterator);
/** Return 1 if the iterator has a next , else 0 */
int listiterator__has_next(struct listiterator iterator);
/** Return 1 if the iterator has a previous , else 0 */
int listiterator__has_previous(struct listiterator iterator);
/** Return the data of the element pointed by the iterator, iterator must be valide */
void* listiterator__get_data(struct listiterator iterator);
/** Set the data of the element pointed by the iterator, iterator must be valide */
void listiterator__set_data(struct listiterator iterator, void*data);

/** Delete a node from the list and return the iterator of the current valide element (iterator becomes invalide if list is empty)*/
struct listiterator listiterator__remove_node(struct listiterator iterator);

/** Insert a value after the current iterator, iterator stills valide, an iterator on the new element is returned */
struct listiterator listiterator__insert_after(struct listiterator iterator, void* data);
/** Insert a value before the current iterator, iterator stills valide, an iterator on the new element is returned  */
struct listiterator listiterator__insert_before(struct listiterator iterator, void* data);

//////////////////////////////////////////////////////////////////////////////

/** Find a data equal to the given paramter and return an iterator (else invalide iterator is returned) */
struct listiterator listiterator__find_data(struct linkedlist* list, void* data);
/** Find a data equal to the given paramter using the equal function and return an iterator (else invalide iterator is returned) */
struct listiterator listiterator__find(struct linkedlist* list, int (*equal)(void*, void*), void* secondParameter);
/** Find a data that the test function retrun true and return an iterator (else invalide iterator is returned) */
struct listiterator listiterator__test(struct linkedlist* list, int (*test)(void*));
/** Apply a function to all the elements of the list */
void listiterator__proceed(struct linkedlist* list, void (*process)(void*));



#endif
