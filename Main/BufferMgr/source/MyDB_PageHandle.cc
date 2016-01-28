
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "../headers/MyDB_PageHandle.h"
#include "../headers/MyDB_BufferManager.h"

void *MyDB_PageHandleBase :: getBytes () {
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    bufManager->updateLRUPage(pagePtr->pageNo);
    return pagePtr->readPageContent();
}

void MyDB_PageHandleBase :: wroteBytes () {
    pagePtr->dirty = true;
    bufManager->updateLRUPage(pagePtr->pageNo);
}

void MyDB_PageHandleBase :: pin() {
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    bufManager->updateLRUPage(pagePtr->pageNo);
    pagePtr->pinned = true;
}

void MyDB_PageHandleBase :: unpin() {
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    bufManager->updateLRUPage(pagePtr->pageNo);
    pagePtr->pinned = false;
}

void MyDB_PageHandleBase ::reloadPage() {
    pagePtr = bufManager->getPagePtr(table, tablePos, anonymous);
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
    if (pagePtr->anonymous){
        //release disk space
        bufManager->setTempFilePos(tablePos, false);
    }

}

MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> pagePtr, MyDB_BufferManager *bufManager){
	this->pagePtr = pagePtr;
    this->bufManager = bufManager;
    this->table = pagePtr->table;
    this->tablePos = pagePtr->tablePos;
    this->anonymous = pagePtr->anonymous;
	pagePtr->incRef();
    bufManager->updateLRUPage(pagePtr->pageNo);
}

#endif

