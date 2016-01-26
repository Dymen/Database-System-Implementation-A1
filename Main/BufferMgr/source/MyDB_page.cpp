//
// Created by Dan Ye on 1/23/16.
//

#ifndef A1_MYDB_PAGE_C
#define A1_MYDB_PAGE_C

#include <string>
#include <fstream>
#include "../headers/MyDB_Page.h"

using namespace std;

MyDB_Page::MyDB_Page(int pageNo, size_t pageSize, char* pageAdd){
    unUsed = true;
    pinned = false;
    anonymous = false;
    dirty = false;
    this->pageNo = pageNo;
    this->pageSize = pageSize;
    this->pageAdd = pageAdd;
    nRef = 0;
}

MyDB_Page::~MyDB_Page(){

}

void MyDB_Page::decRef(){
    nRef --;
    if (nRef == 0){
        pinned = false;
        if (anonymous)
            unUsed = true;
    }
}

void MyDB_Page::incRef() {
    nRef ++;
    unUsed = false;
}

//Should be called each time page assigned to a new data entry
void MyDB_Page::readFromDisk() {
    ifstream fin(table->getStorageLoc());
    fin.seekg(tablePos*pageSize, ios::beg);
    fin.read(pageAdd, pageSize);
}

void MyDB_Page::writeToDisk() {
    ofstream fout(table->getStorageLoc());
    fout.seekp(tablePos*pageSize, ios::beg);
    fout.write(pageAdd, pageSize);
}

//Read anonymous page
void MyDB_Page::readFromDisk(string tempFile) {
    ifstream fin(tempFile);
    fin.seekg(tablePos*pageSize, ios::beg);
    fin.read(pageAdd, pageSize);
}

//Write anonymous page
void MyDB_Page::writeToDisk(string tempFile) {
    ofstream fout(tempFile);
    fout.seekp(ios::beg);
    fout.write(pageAdd, pageSize);
}

void* MyDB_Page::readPageContent(){
    return pageAdd;
}

void MyDB_Page::cleanPage(string tempFile) {
    if (dirty) {
        if (anonymous)
            writeToDisk(tempFile);
        else
            writeToDisk();
    }
    pinned = false;
    unUsed = true;
    anonymous = false;
    dirty = false;
    nRef = 0;
}

void MyDB_Page::reloadData(MyDB_TablePtr tab, size_t pos){
    table = tab;
    tablePos = pos;
    pinned = false;
    unUsed = false;
    anonymous = false;
    readFromDisk();
}

void MyDB_Page::reloadData(){
    pinned = false;
    unUsed = false;
    anonymous = true;
}

bool MyDB_Page::checkPage(MyDB_TablePtr tab, size_t pos) {
    return ((!unUsed)&&(!anonymous)&&(table->getName().compare(tab->getName())==0)&&(tablePos==pos));
}

#endif //A1_MYDB_PAGE_H
