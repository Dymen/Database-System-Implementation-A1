
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "../headers/MyDB_PageHandle.h"
#include "../../Catalog/headers/MyDB_Table.h"
#include "../headers/MyDB_BufferManager.h"
#include <string>
#include <stdlib.h>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long pos) {
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(this, table, pos, false);
    MyDB_PageHandle newHandle(newHandleBase);
    return newHandle;
}

//Return a handle which points to an anonymous page, but before read/write op the handle won't be allocated a real page in buffer
MyDB_PageHandle MyDB_BufferManager :: getPage () {
    //First assign an address in disk for use
    int pos = findTempFilePos();
    setTempFilePos(pos, true);
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(this, tempTable, pos, true);
    MyDB_PageHandle newHandle(newHandleBase);
    return newHandle;
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table, long pos) {
    shared_ptr<MyDB_Page> pagePtr = getPagePtr(table, pos, false);
    pagePtr->pinned = true;
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(pagePtr, this);
    MyDB_PageHandle newHandle(newHandleBase);
    return newHandle;
}

//Will allcate a page from buffer directly(always a new page for this function)
MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    shared_ptr<MyDB_Page> pagePtr = getPagePtr();
    pagePtr->pinned = true;
    pagePtr->anonymous = true;
    MyDB_PageHandleBase *newHandleBase = new MyDB_PageHandleBase(pagePtr, this);
    MyDB_PageHandle newHandle(newHandleBase);
    return newHandle;
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->unpin();
}

//Check whether the page is already in the buffer pool
shared_ptr<MyDB_Page> MyDB_BufferManager::findPageInBuffer(MyDB_TablePtr table, long pos){
    int i;
    for (i = 0; i < numPages; i ++) {
        if (buffer[i]->checkPage(table, pos)) {
            //cout << table->getName() << " " << pos << " in page " << i << endl;
            return buffer[i];
        }
    }
    return nullptr;
}

void MyDB_BufferManager::decRefMap(string tableName, long pos) {
    MyDB_TableInfo *tableInfo = new MyDB_TableInfo(tableName, pos);
    shared_ptr<MyDB_TableInfo> tableInfoPtr(tableInfo);
    pageRefMap[tableInfoPtr] --;
    if (pageRefMap[tableInfoPtr] == 0)
        pageRefMap[tableInfoPtr] = -1;
}

void MyDB_BufferManager::incRefMap(string tableName, long pos) {
    MyDB_TableInfo *tableInfo = new MyDB_TableInfo(tableName, pos);
    shared_ptr<MyDB_TableInfo> tableInfoPtr(tableInfo);
    map<shared_ptr<MyDB_TableInfo>, int>::iterator iter = pageRefMap.find(tableInfoPtr);
    if (iter == pageRefMap.end())
        pageRefMap.insert(make_pair(tableInfoPtr, 1));
    else {
        if (pageRefMap[tableInfoPtr] >= 0)
            pageRefMap[tableInfoPtr] ++;
        else
            pageRefMap[tableInfoPtr] = 1;
    }
}

//Check whether the corresponding page is saved in disk and return the number of references
int MyDB_BufferManager::checkRefMap(string tableName, long pos){
    MyDB_TableInfo *tableInfo = new MyDB_TableInfo(tableName, pos);
    shared_ptr<MyDB_TableInfo> tableInfoPtr(tableInfo);
    map<shared_ptr<MyDB_TableInfo>, int>::iterator iter = pageRefMap.find(tableInfoPtr);
    if (iter == pageRefMap.end())
        return -1;
    return pageRefMap[tableInfoPtr];
}

//Mark that there is no handle pointing to the page
//By default, when calling this function there exists a key of tableName
void MyDB_BufferManager::cleanRefMap(string tableName, long pos) {
    MyDB_TableInfo *tableInfo = new MyDB_TableInfo(tableName, pos);
    shared_ptr<MyDB_TableInfo> tableInfoPtr(tableInfo);
    pageRefMap[tableInfoPtr] = -1;
}

//Return a page pointer(already loaded the corresponding content)
shared_ptr<MyDB_Page> MyDB_BufferManager::getPagePtr(MyDB_TablePtr table, long pos, bool anonymous) {
    int i, newPage;

    for (i = 0; i < numPages; i ++) {
        if (buffer[i]->unUsed)
            continue;
        if (buffer[i]->checkPage(table, pos)) {
            //cout << table->getName() << " " << pos << " in page " << i << endl;
            return buffer[i];
        }
    }
    newPage = getLRUPage();
    buffer[newPage]->reloadData(table, pos);
    buffer[newPage]->nRef = checkRefMap(table->getName(), pos);
    if (buffer[newPage]->nRef == -1)
        buffer[newPage]->nRef = 0;
    else
        cleanRefMap(table->getName(), pos);
    buffer[newPage]->unUsed = false;
    buffer[newPage]->tablePos = pos;
    buffer[newPage]->table = table;
    buffer[newPage]->anonymous = anonymous;
    buffer[newPage]->dirty = false;
    return buffer[newPage];
}

//Return a page pointer from buffer pool for anonymous page
shared_ptr<MyDB_Page> MyDB_BufferManager::getPagePtr() {
    int newPage;

    newPage = getLRUPage();
    buffer[newPage]->nRef = 0;
    buffer[newPage]->unUsed = false;
    buffer[newPage]->pinned = false;
    buffer[newPage]->anonymous = true;
    buffer[newPage]->dirty = false;
    buffer[newPage]->table = tempTable;
    buffer[newPage]->tablePos = findTempFilePos();
    setTempFilePos(buffer[newPage]->tablePos, true);
    return buffer[newPage];
}

int MyDB_BufferManager::getLRUPage() {
    shared_ptr<LRULinkedList> curNode = head;

    while (buffer[curNode->pageNo]->pinned){
        curNode = curNode->succ;
        if (curNode == nullptr)
            throw new exception();
    }
    if (! buffer[curNode->pageNo]->unUsed) {
        //cout << "evict page " << curNode->pageNo << endl;
        //Save the number of references to a map in the manager, and then clean the page for later use
        MyDB_TableInfo *tableInfo = new MyDB_TableInfo(buffer[curNode->pageNo]->table->getName(), buffer[curNode->pageNo]->tablePos);
        shared_ptr<MyDB_TableInfo> tableInfoPtr(tableInfo);
        if (buffer[curNode->pageNo]->nRef > 0) {
            map<shared_ptr<MyDB_TableInfo>, int>::iterator iter = pageRefMap.find(tableInfoPtr);
            if (iter != pageRefMap.end())
                pageRefMap[tableInfoPtr] = buffer[curNode->pageNo]->nRef;
            else
                pageRefMap.insert(make_pair(tableInfoPtr, buffer[curNode->pageNo]->nRef));
        }
        buffer[curNode->pageNo]->cleanPage(tempFile);
    }
    /*
    else
        cout << "allocate page " << curNode->pageNo << endl;
    */
	return curNode->pageNo;
}

//Move page to the tail of LRU list
void MyDB_BufferManager::updateLRUPage(int pageNo) {
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

int MyDB_BufferManager::findTempFilePos(){
    int i = 0;

    while (true){
        if (tempPagePosMap.find(i) == tempPagePosMap.end())
            return i;
        if (tempPagePosMap[i] == false)
            return i;
        i ++;
    }
}

MyDB_BufferManager :: MyDB_BufferManager (int pageSize, int numPages, string tempFile) {
	int i;

    MyDB_Table *temp = new MyDB_Table(tempFile, tempFile);
    shared_ptr<MyDB_Table>tempPtr(temp);
    tempTable = tempPtr;

	//Initialize the LRU relating variables
	for (i = 0; i < numPages; i ++){
        shared_ptr<LRULinkedList> newListNodePtr = make_shared<LRULinkedList>();
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
    bufferStart = (char*)malloc(pageSize * numPages);
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
    int i;
    for (i = 0; i < numPages; i ++)
        if (buffer[i]->dirty)
            buffer[i]->writeToDisk();
    for (Page2LRUPtrMap::const_iterator it = page2LRUPtr.begin(); it != page2LRUPtr.end();) {
        //cout << "Erase " << it->second->pageNo << endl;
        if (it->second != nullptr)
            it->second->~LRULinkedList();
        it = page2LRUPtr.erase(it);
    }

    for (PageRefMap::const_iterator it = pageRefMap.begin(); it != pageRefMap.end();){
        //cout << "Erase " << it->first->tableName << " " << it->first->tablePos << endl;
        it = pageRefMap.erase(it);
    }
    free(bufferStart);
}
	
#endif


