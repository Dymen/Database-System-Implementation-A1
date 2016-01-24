//
// Created by Dan Ye on 1/23/16.
//
#ifndef A1_CHECLRU_C
#define A1_CHECLRU_C

#include "../headers/CheckLRU.h"

LRUChecker::LRUChecker(size_t numPages) {
    int i;

    for (i = 0; i < numPages; i ++){
        page2Ptr.insert(make_pair(i, make_shared(new LRULinkedList)));
        if (i > 0) {
            page2Ptr[i]->prev = page2Ptr[i - 1];
            page2Ptr[i - 1]->succ = page2Ptr[i];
        }
        ptr2Page[page2Ptr[i]] = i;
    }
    page2Ptr[0]->prev = nullptr;
    page2Ptr[numPages-1]->succ = nullptr;
    head = page2Ptr[0];
    tail = page2Ptr[numPages-1];
}

LRUChecker::~LRUChecker() {

}

size_t LRUChecker::getLRUPage() {
    return ptr2Page[head];
}

void LRUChecker::updatePage(size_t pageNo) {
    if (page2Ptr[pageNo] == head)
        head = page2Ptr[pageNo]->succ;
    else
        page2Ptr[pageNo]->prev
    //move pageNo to the tail
    tail->succ = page2Ptr[pageNo];
    tail = page2Ptr[pageNo];
}
#endif

