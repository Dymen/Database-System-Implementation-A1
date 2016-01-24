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
    size_t pageNo;
};
#endif //A1_CHECLRU_H
