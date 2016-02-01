
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

#include <vector>
#include "MyDB_PageHandle.h"
#include "../../Catalog/headers/MyDB_Table.h"
#include "MyDB_Page.h"
#include "CheckLRU.h"

using namespace std;
class MyDB_TableInfo{
public:
    MyDB_TableInfo(string tableName, long i):tableName(tableName), tablePos(i){};
    string tableName;
    long tablePos;
	~MyDB_TableInfo(){
		//cout << "Deconstruct " << tableName << " " << tablePos << endl;
	};
};

struct MyDB_TableInfoComparator{
    bool operator()(shared_ptr<MyDB_TableInfo> info1, shared_ptr<MyDB_TableInfo> info2)const{
        if (info1->tableName.compare(info2->tableName) != 0)
            return info1->tableName.compare(info2->tableName) > 0 ? false : true;
        if (info1->tablePos != info2->tablePos)
            return info1->tablePos > info2->tablePos > 0 ? false : true;
        return false;
    }
};

class MyDB_BufferManager {

public:

	// THESE METHODS MUST APPEAR AND THE PROTOTYPES CANNOT CHANGE!

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

    void decRefMap(string tableName, long pos);

	void incRefMap(string tableName, long pos);

	shared_ptr<MyDB_Page> findPageInBuffer(MyDB_TablePtr table, long pos);

    shared_ptr<MyDB_Page> getPagePtr(MyDB_TablePtr table, long pos, bool anonymous);

    shared_ptr<MyDB_Page> getPagePtr();

    int checkRefMap(string tableName, long pos);

    void cleanRefMap(string tableName, long pos);

    int findTempFilePos();

    void setTempFilePos(int pos, bool flag){
        tempPagePosMap[pos] = flag;
    }

    string getTempFileName(){
        return tempFile;
    };

    void updateLRUPage(int);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (int pageSize, int numPages, string tempFile);
	
	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// FEEL FREE TO ADD ADDITIONAL PUBLIC METHODS
	shared_ptr<MyDB_Table> tempTable;
private:
	typedef map<int, shared_ptr<LRULinkedList>> Page2LRUPtrMap;
	typedef map <shared_ptr<MyDB_TableInfo>, int, MyDB_TableInfoComparator> PageRefMap;
	char* bufferStart;
	int pageSize, numPages;
	string tempFile;
	vector<shared_ptr<MyDB_Page>> buffer;
    shared_ptr<LRULinkedList> head, tail;
    map<int, shared_ptr<LRULinkedList>> page2LRUPtr;
    map<shared_ptr<MyDB_TableInfo>, int, MyDB_TableInfoComparator> pageRefMap;
    map<int, bool> tempPagePosMap;

    //Let LRU return a page number for storing new content
    int getLRUPage();
    //check whether pageNo_th page stores the corresponding content
    bool checkPage(int pageNo, MyDB_TablePtr whichTable, long i);
};

#endif


