
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "../headers/MyDB_PageHandle.h"

void *MyDB_PageHandleBase :: getBytes () {
	return pagePtr->readPageContent();
}

void MyDB_PageHandleBase :: wroteBytes () {
    pagePtr->dirty = true;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	pagePtr->decRef();
}

MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> pagePtr){
	this->pagePtr = pagePtr;
	pagePtr->incRef();
}

#endif

