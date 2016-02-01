
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "../headers/MyDB_PageHandle.h"
#include "../headers/MyDB_BufferManager.h"

void *MyDB_PageHandleBase :: getBytes () {
    if (pagePtr == nullptr)
        reloadPage();
    else if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    bufManager->updateLRUPage(pagePtr->pageNo);
    return pagePtr->readPageContent();
}

void MyDB_PageHandleBase :: wroteBytes () {
    if (pagePtr == nullptr)
        reloadPage();
    else if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    pagePtr->dirty = true;
    bufManager->updateLRUPage(pagePtr->pageNo);
}

void MyDB_PageHandleBase :: pin() {
    if (pagePtr == nullptr)
        reloadPage();
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    bufManager->updateLRUPage(pagePtr->pageNo);
    pagePtr->pinned = true;
}

//When unpin a page, the page should be already in the buffer pool
void MyDB_PageHandleBase :: unpin() {
    if (pagePtr == nullptr)
        reloadPage();
    if (! pagePtr->checkPage(table, tablePos))
        reloadPage();
    pagePtr->pinned = false;
}

void MyDB_PageHandleBase ::reloadPage() {
    pagePtr = bufManager->getPagePtr(table, tablePos, anonymous);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
    //Decrease the number of references pointing to the page
    //If the page is currently in the buffer, find the page and decrease the count by 1
    //  (1)The page ptr saved int the handle is exactly the corresponding page, decrease directly
    //  (2)The page ptr as expired, reload the page ptr and decrease the count
    //Else decrease the count in the map which saves number of references of pages not in the buffer
	if (pagePtr->checkPage(table, tablePos))
        pagePtr->decRef();
    else{
        if (bufManager->checkRefMap(table->getName(), tablePos) == -1){
            reloadPage();
            pagePtr->decRef();
        }
        else bufManager->decRefMap(table->getName(), tablePos);
    }
    //Release disk space if it's an anonymous page
    //Note that the anonymous page can have at most one handle pointing to it
    if (pagePtr->anonymous){
        bufManager->setTempFilePos(tablePos, false);
    }

}

//By default, this function assumes pagePtr != nullptr. If to assign a nullptr, call the funciton right below.
MyDB_PageHandleBase :: MyDB_PageHandleBase(shared_ptr<MyDB_Page> pagePtr, MyDB_BufferManager *bufManager){
	this->pagePtr = pagePtr;
    this->bufManager = bufManager;
    this->table = pagePtr->table;
    this->tablePos = pagePtr->tablePos;
    this->anonymous = pagePtr->anonymous;
	pagePtr->incRef();
    //bufManager->updateLRUPage(pagePtr->pageNo);
}

//This function assign a nullptr to the handle but will save page info
MyDB_PageHandleBase :: MyDB_PageHandleBase(MyDB_BufferManager *bufManager, MyDB_TablePtr table , int tablePos, bool anonymous){
    shared_ptr<MyDB_Page> pagePtrInBuffer = bufManager->findPageInBuffer(table, tablePos);
    this->pagePtr = pagePtrInBuffer;
    this->bufManager = bufManager;
    this->table = table;
    this->tablePos = tablePos;
    this->anonymous = anonymous;
    if (pagePtrInBuffer == nullptr)
        bufManager->incRefMap(table->getName(), tablePos);
    else
        this->pagePtr->incRef();
    //bufManager->updateLRUPage(pagePtr->pageNo);
}

#endif

