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

void MyDB_Page::readFromDisk() {
    ifstream fin(table->getStorageLoc());
    fin.seekg(tablePos, ios::beg);
    fin.read(pageAdd, pageSize);
}

void MyDB_Page::writeToDisk() {
    ofstream fout(table->getStorageLoc());
    fout.seekp(tablePos, ios::beg);
    fout.write(pageAdd, pageSize);
}

//Read anonymous page
void MyDB_Page::readFromDisk(string tempFile) {
    ifstream fin(tempFile);
    fin.seekg(tablePos, ios::beg);
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
}

#endif //A1_MYDB_PAGE_H
