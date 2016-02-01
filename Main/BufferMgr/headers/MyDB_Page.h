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
    MyDB_Page(int pageNo, int pageSize, char* pageAdd);
    ~MyDB_Page();
    int pageNo, nRef;
    bool pinned, unUsed, anonymous, dirty;
    MyDB_TablePtr table;
    int tablePos; //tablePos_th page in the table

    void decRef();
    void incRef();
    void writeToDisk();
    void readFromDisk();
    void* readPageContent();
    void cleanPage(string tempFile);
    void reloadData(MyDB_TablePtr tab, int pos);
    bool checkPage(MyDB_TablePtr tab, int pos);

private:
    int pageSize;
    char* pageAdd;
};
#endif //A1_MYDB_PAGE_H
