//
// Created by Dan Ye on 1/23/16.
//

#ifndef A1_MYDB_PAGE_H
#define A1_MYDB_PAGE_H

#include <string>
#include "../../Catalog/headers/MyDB_Table.h"

using namespace std;

class MyDB_Page{
public:
    MyDB_Page(int pageNo, size_t pageSize, char* pageAdd);
    ~MyDB_Page();
    size_t pageNo;
    bool pinned, unUsed, anonymous, dirty;
    MyDB_TablePtr table;
    size_t tablePos; //tablePos_th page in the table

    void decRef();
    void incRef();
    void writeToDisk();
    void writeToDisk(string tempFile);
    void readFromDisk();
    void readFromDisk(string tempFile);
    void* readPageContent();
    void cleanPage(string tempFile);

private:
    size_t nRef, pageSize;
    char* pageAdd;
};
#endif //A1_MYDB_PAGE_H
