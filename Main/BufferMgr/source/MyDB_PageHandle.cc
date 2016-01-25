
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "../headers/MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
    if (pagePtr->checkPage(table, tablePos))
        //The current pagePtr still store the corresponding content
	    return pagePtr->readPageContent();
    else{
        if (anonymous){
            pagePtr = bufManager->getPagePtr();
            pagePtr->incRef();
        }
        else{
            pagePtr = bufManager->getPagePtr(table, tablePos);
            pagePtr->incRef();
        }
        return pagePtr->readPageContent();
    }
}

void MyDB_PageHandleBase :: wroteBytes () {
    pagePtr->dirty = true;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	pagePtr->decRef();
}

MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> pagePtr, shared_ptr<MyDB_BufferManager> bufManagers){
	this->pagePtr = pagePtr;
    this->bufManager = bufManager;
    this->table = pagePtr->table;
    this->tablePos = pagePtr->tablePos;
    this->anonymous = pagePtr->anonymous;
	pagePtr->incRef();
}

#endif

