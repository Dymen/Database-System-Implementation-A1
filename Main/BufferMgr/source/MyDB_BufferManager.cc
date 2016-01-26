
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "../headers/MyDB_PageHandle.h"
#include "../../Catalog/headers/MyDB_Table.h"
#include "../headers/MyDB_BufferManager.h"
#include <string>
#include <stdlib.h>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long pos) {
    MyDB_PageHandle newHandle;
    newHandle = make_shared<MyDB_PageHandleBase>(getPagePtr(table, pos), this);
    return newHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    MyDB_PageHandle newHandle;

    newHandle = make_shared<MyDB_PageHandleBase>(getPagePtr(), this);
    return newHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table, long pos) {
    shared_ptr<MyDB_Page> pagePtr = getPagePtr(table, pos);
    pagePtr->pinned = true;
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(pagePtr, this);
    MyDB_PageHandle newHandle(newHandleBase);

    return newHandle;
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    shared_ptr<MyDB_Page> pagePtr = getPagePtr();
    pagePtr->pinned = true;
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(pagePtr, this);
    MyDB_PageHandle newHandle(newHandleBase);

    return newHandle;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->unpin();
}


void MyDB_BufferManager::decRefMap(string tableName, long pos) {
    MyDB_TableInfo tableInfo(tableName, pos);
    pageRefMap[tableInfo] --;
}

size_t MyDB_BufferManager::checkRefMap(string tableName, long pos){
    MyDB_TableInfo tableInfo(tableName, pos);
    if (pageRefMap.find(tableInfo) == pageRefMap.end())
        return -1;
    return pageRefMap[tableInfo];
}

void MyDB_BufferManager::cleanRefMap(string tableName, long pos) {
    MyDB_TableInfo tableInfo(tableName, pos);
    pageRefMap[tableInfo] = -1;
}

//Return a page pointer(already loaded the corresponding content)
shared_ptr<MyDB_Page> MyDB_BufferManager::getPagePtr(MyDB_TablePtr table, long pos) {
    int i, newPage;

    for (i = 0; i < numPages; i ++) {
        if (buffer[i]->unUsed)
            continue;
        if (buffer[i]->checkPage(table, pos))
            return buffer[i];
    }
    newPage = getLRUPage();
    buffer[newPage]->reloadData(table, pos);
    buffer[newPage]->nRef = checkRefMap(table->getName(), pos);
    if (buffer[newPage]->nRef == -1)
        buffer[newPage]->nRef = 0;
    else
        cleanRefMap(table->getName(), pos);
    return buffer[newPage];
}

//Return a page pointer for anonymous page
shared_ptr<MyDB_Page> MyDB_BufferManager::getPagePtr() {
    int newPage;

    newPage = getLRUPage();
    buffer[newPage]->reloadData();
    return buffer[newPage];
}

size_t MyDB_BufferManager::getLRUPage() {
    shared_ptr<LRULinkedList> curNode = head;

    while (buffer[curNode->pageNo]->pinned){
        curNode = curNode->succ;
        if (curNode == nullptr)
            throw new exception();
    }
    if (! buffer[curNode->pageNo]->unUsed) {
        if (! buffer[curNode->pageNo]->anonymous) {
            MyDB_TableInfo tableInfo(buffer[curNode->pageNo]->table->getName(), buffer[curNode->pageNo]->tablePos);
            pageRefMap[tableInfo] = buffer[curNode->pageNo]->nRef;
        }
        buffer[curNode->pageNo]->cleanPage(tempFile);
    }
	return curNode->pageNo;
}

//Move page to the tail of LRU list
void MyDB_BufferManager::updatePage(size_t pageNo) {
	if (page2LRUPtr[pageNo] != head)
		page2LRUPtr[pageNo]->prev->succ = page2LRUPtr[pageNo]->succ;
    else
        head = page2LRUPtr[pageNo]->succ;
	if (page2LRUPtr[pageNo] != tail)
		page2LRUPtr[pageNo]->succ->prev = page2LRUPtr[pageNo]->prev;
    else
        tail = page2LRUPtr[pageNo]->prev;
	//If the page is not empty ,move pageNo to the tail
    if (! buffer[pageNo]->unUsed) {
        tail->succ = page2LRUPtr[pageNo];
        page2LRUPtr[pageNo]->prev = tail;
        page2LRUPtr[pageNo]->succ = nullptr;
        tail = page2LRUPtr[pageNo];
    }
    else{
        //If not, move page to the head
        head->prev = page2LRUPtr[pageNo];
        page2LRUPtr[pageNo]->prev = nullptr;
        page2LRUPtr[pageNo]->succ = head;
        head = page2LRUPtr[pageNo];
    }
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
	int i;

	//Initialize the LRU relating variables
	for (i = 0; i < numPages; i ++){
        LRULinkedList *newListNode = new LRULinkedList();
        shared_ptr<LRULinkedList> newListNodePtr(newListNode);
		page2LRUPtr.insert(make_pair(i, newListNodePtr));
		if (i > 0) {
			page2LRUPtr[i]->prev = page2LRUPtr[i - 1];
			page2LRUPtr[i - 1]->succ = page2LRUPtr[i];
		}
		page2LRUPtr[i]->pageNo = i;
	}
	page2LRUPtr[0]->prev = nullptr;
	page2LRUPtr[numPages-1]->succ = nullptr;
	head = page2LRUPtr[0];
	tail = page2LRUPtr[numPages-1];

    //Initialize buffer
    char* bufferStart = (char*)malloc(pageSize * numPages);
    for (i = 0; i < numPages; i ++){
        MyDB_Page *newPage = new MyDB_Page(i, pageSize, bufferStart+i*pageSize);
        shared_ptr<MyDB_Page> newPagePtr(newPage);
        buffer.push_back(newPagePtr);
    }

    //Initialize other parameters
    this->tempFile = tempFile;
    this->pageSize = pageSize;
    this->numPages = numPages;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


