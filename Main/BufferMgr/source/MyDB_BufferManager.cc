
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "../headers/MyDB_PageHandle.h"
#include "../../Catalog/headers/MyDB_Table.h"
#include "../headers/MyDB_BufferManager.h"
#include <string>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

size_t MyDB_BufferManager::getLRUPage() {
    shared_ptr<LRULinkedList> curNode = head;

    while (buffer[curNode->pageNo]->pinned){
        curNode = curNode->succ;
        if (curNode == nullptr)
            throw new exception();
    }
    if (! buffer[curNode->pageNo]->unUsed) {
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
	//If the page is not empty ,move pageNo to the tail
    if (! buffer[pageNo]->unUsed) {
        if (tail->pageNo == pageNo)
            return;
        tail->succ = page2LRUPtr[pageNo];
        page2LRUPtr[pageNo]->prev = tail;
        page2LRUPtr[pageNo]->succ = nullptr;
        tail = page2LRUPtr[pageNo];
    }
    else{
        //If not, move page to the head
        if (head->pageNo == pageNo)
            return;
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
		page2LRUPtr.insert(make_pair(i, make_shared(new LRULinkedList)));
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
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


