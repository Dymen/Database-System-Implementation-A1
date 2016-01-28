//
// Created by Dan Ye on 1/23/16.
//

#ifndef A1_CHECLRU_H
#define A1_CHECLRU_H

#include <memory>
#include <map>
#include <iostream>

using namespace std;

class LRULinkedList{
public:
    shared_ptr<LRULinkedList> prev, succ;
    size_t pageNo;
    LRULinkedList(){
        prev = nullptr;
        succ = nullptr;
    }
    ~LRULinkedList(){
        //cout << "Deconstruct linked list" << endl;
        prev = nullptr;
        succ = nullptr;
    }
};
#endif //A1_CHECLRU_H
