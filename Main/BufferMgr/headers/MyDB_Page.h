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
    MyDB_Page();
    ~MyDB_Page();
    size_t nRef, pageNo;
    MyDB_TablePtr table;
    size_t tablePos; //tablePos_th page in the table
    bool pinned, unUsed;

    void decRef();
    void writeToDisk();
    void readFromDisk(MyDB_TablePtr whichTable, long i);

};
#endif //A1_MYDB_PAGE_H
