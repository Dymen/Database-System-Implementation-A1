//
// Created by Dan Ye on 1/23/16.
//

#ifndef A1_CHECLRU_H
#define A1_CHECLRU_H

#include <memory>
#include <map>

using namespace std;

class LRULinkedList{
public:
    shared_ptr<LRULinkedList> prev, succ;
};

class LRUChecker{
public:
    LRUChecker(size_t numPages);
    ~LRUChecker();

    shared_ptr<LRULinkedList> head, tail;
    map<size_t, shared_ptr<LRULinkedList>> page2Ptr;
    map<shared_ptr<LRULinkedList>, size_t> ptr2Page;

    size_t getLRUPage();
    void updatePage(size_t);
};

#endif //A1_CHECLRU_H
