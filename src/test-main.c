// My includes
#include "link.h"

// System includes
#include <stdio.h>
#include <assert.h>
#include <string.h>

int areEqual(void* str1, void* str2){
    return strcmp((char*)str1, (char*)str2) == 0;
}

void print(void* str1){
    printf("%s\n", (char*)str1);
}

void printInt(void* hex){
    printf("%d\n", ((int*)hex)[0]);
}

void basicTest(){
    struct linkedlist* lnk = linkedlist__alloc_empty();

    assert(linkedlist__get_size(lnk) == 0);

    linkedlist__push_front(lnk, "First");
    assert(linkedlist__get_size(lnk) == 1);

    linkedlist__push_front(lnk, "Second");
    assert(linkedlist__get_size(lnk) == 2);

    linkedlist__push_back(lnk, "Third");
    assert(linkedlist__get_size(lnk) == 3);

    linkedlist__free_list_and_nodes(lnk);
}

void findTest(){
    struct listiterator iter;
    struct linkedlist* lnk = linkedlist__alloc_empty();

    assert(linkedlist__get_size(lnk) == 0);

    linkedlist__push_front(lnk, "First");
    assert(linkedlist__get_size(lnk) == 1);

    linkedlist__push_front(lnk, "Second");
    assert(linkedlist__get_size(lnk) == 2);

    linkedlist__push_back(lnk, "Third");
    assert(linkedlist__get_size(lnk) == 3);

    iter = listiterator__find(lnk, areEqual, "First");
    assert(listiterator__is_valide(iter));

    iter = listiterator__find(lnk, areEqual, "Second");
    assert(listiterator__is_valide(iter));

    iter = listiterator__find(lnk, areEqual, "Third");
    assert(listiterator__is_valide(iter));

    iter = listiterator__find(lnk, areEqual, "NONE");
    assert(listiterator__is_valide(iter) == 0);

    listiterator__proceed(lnk, print);

    iter = listiterator__find(lnk, areEqual, "Second");
    assert(listiterator__is_valide(iter));
    iter = listiterator__remove_node(iter);
    assert(listiterator__is_valide(iter));
    iter = listiterator__find(lnk, areEqual, "Second");
    assert(listiterator__is_valide(iter) == 0);
    assert(linkedlist__get_size(lnk) == 2);

    iter = listiterator__find(lnk, areEqual, "First");
    assert(listiterator__is_valide(iter));
    iter = listiterator__remove_node(iter);
    assert(listiterator__is_valide(iter));
    iter = listiterator__find(lnk, areEqual, "First");
    assert(listiterator__is_valide(iter) == 0);
    assert(linkedlist__get_size(lnk) == 1);

    iter = listiterator__find(lnk, areEqual, "Third");
    assert(listiterator__is_valide(iter));
    iter = listiterator__remove_node(iter);
    assert(listiterator__is_valide(iter) == 0);
    iter = listiterator__find(lnk, areEqual, "Third");
    assert(listiterator__is_valide(iter) == 0);
    assert(linkedlist__get_size(lnk) == 0);

    iter = listiterator__find(lnk, areEqual, "NONE");
    assert(listiterator__is_valide(iter) == 0);

    linkedlist__free_list_and_nodes(lnk);
}


void frontBackTest(){
    const int TestSize = 30;
    int testArray[TestSize];
    struct listiterator iter;
    struct linkedlist* lnk = linkedlist__alloc_empty();
    int idx;

    for(idx = 0 ; idx < TestSize ; idx += 1){
        testArray[idx] = idx;
    }

    for(idx = 0 ; idx < TestSize/2 ; idx += 1){
        assert(linkedlist__get_size(lnk) == idx*2);
        linkedlist__push_front(lnk, &testArray[TestSize/2-idx-1]);
        linkedlist__push_back(lnk, &testArray[idx+TestSize/2]);
        assert(linkedlist__front(lnk) == &testArray[TestSize/2-idx-1]);
        assert(linkedlist__back(lnk) == &testArray[idx+TestSize/2]);
    }

    listiterator__proceed(lnk, printInt);

    iter = listiterator__init_iterator(lnk);
    for(idx = 0 ; idx < TestSize ; idx += 1){
        assert(listiterator__is_valide(iter) != 0);
        assert(listiterator__get_data(iter) == &testArray[idx]);
        iter = listiterator__goto_next(iter);
    }
    assert(listiterator__is_valide(iter) == 0);

    iter = listiterator__init_iterator(lnk);
    while(listiterator__has_next(iter) != 0){
        iter = listiterator__goto_next(iter);
        assert(listiterator__is_valide(iter) != 0);
    }
    for(idx = TestSize-1 ; idx >= 0 ; idx -= 1){
        assert(listiterator__is_valide(iter) != 0);
        assert(listiterator__get_data(iter) == &testArray[idx]);
        iter = listiterator__goto_previous(iter);
    }
    assert(listiterator__is_valide(iter) == 0);

    iter = listiterator__init_iterator(lnk);
    for(idx = 0 ; idx < TestSize ; idx += 1){
        assert(linkedlist__get_size(lnk) == TestSize-idx);
        assert(listiterator__is_valide(iter) != 0);
        assert(listiterator__get_data(iter) == &testArray[idx]);
        iter = listiterator__remove_node(iter);
    }
    assert(listiterator__is_valide(iter) == 0);
    assert(linkedlist__get_size(lnk) == 0);

    linkedlist__free_list_and_nodes(lnk);
}

void insertTest(){
    const int TestSize = 30;
    int testArray[TestSize];
    struct listiterator iter;
    struct linkedlist* lnk = linkedlist__alloc_empty();
    int idx;

    for(idx = 0 ; idx < TestSize ; idx += 1){
        testArray[idx] = idx;
    }

    linkedlist__push_front(lnk, &testArray[TestSize-1]);
    assert(linkedlist__get_size(lnk) == 1);

    iter = listiterator__init_iterator(lnk);
    for(idx = TestSize-3 ; idx >= 0 ; idx -= 2){
        assert(listiterator__is_valide(iter) != 0);
        assert(listiterator__has_previous(iter) == 0);
        iter = listiterator__insert_before(iter, &testArray[idx]);
        assert(listiterator__has_next(iter) != 0);
        assert(listiterator__get_data(iter) == &testArray[idx]);
        assert(listiterator__get_data(listiterator__goto_next(iter)) == &testArray[idx+2]);
    }

    listiterator__proceed(lnk, printInt);

    iter = listiterator__insert_before(iter, &testArray[0]);
    iter = listiterator__goto_next(iter);

    for(idx = 2 ; idx < TestSize ; idx += 2){
        assert(listiterator__is_valide(iter) != 0);
        iter = listiterator__insert_after(iter, &testArray[idx]);
        assert(listiterator__has_next(iter) != 0);
        assert(listiterator__get_data(iter) == &testArray[idx]);
        assert(listiterator__get_data(listiterator__goto_previous(iter)) == &testArray[idx-1]);
        iter = listiterator__goto_next(iter);
    }

    listiterator__proceed(lnk, printInt);
    assert(linkedlist__get_size(lnk) == TestSize);

    linkedlist__free_list_and_nodes(lnk);
}

int main(int argc, char** argv){
    basicTest();
    findTest();
    frontBackTest();
    insertTest();
    return 0;
}
