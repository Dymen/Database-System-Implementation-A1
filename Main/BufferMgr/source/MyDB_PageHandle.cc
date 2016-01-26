
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "../headers/MyDB_PageHandle.h"
#include "../headers/MyDB_BufferManager.h"

void *MyDB_PageHandleBase :: getBytes () {
    if (pagePtr->checkPage(table, tablePos))
        //The current pagePtr still store the corresponding content
	    return pagePtr->readPageContent();
    else{
        reloadPage();
        return pagePtr->readPageContent();
    }
}

void MyDB_PageHandleBase :: wroteBytes () {
    pagePtr->dirty = true;
}

void MyDB_PageHandleBase :: pin() {
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    pagePtr->pinned = true;
}

void MyDB_PageHandleBase :: unpin() {
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    pagePtr->pinned = false;
}

void MyDB_PageHandleBase ::reloadPage() {
    if (anonymous)
        pagePtr = bufManager->getPagePtr();
    else
        pagePtr = bufManager->getPagePtr(table, tablePos);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	if (pagePtr->checkPage(table, tablePos))
        pagePtr->decRef();
    else{
        if (bufManager->checkRefMap(table->getName(), tablePos) == -1){
            reloadPage();
            pagePtr->decRef();
        }
        else bufManager->decRefMap(table->getName(), tablePos);
    }

}

MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> pagePtr, MyDB_BufferManager *bufManager){
	this->pagePtr = pagePtr;
    this->bufManager = bufManager;
    this->table = pagePtr->table;
    this->tablePos = pagePtr->tablePos;
    this->anonymous = pagePtr->anonymous;
	pagePtr->incRef();
}

#endif

