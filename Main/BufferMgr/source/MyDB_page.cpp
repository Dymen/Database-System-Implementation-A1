//
// Created by Dan Ye on 1/23/16.
//

#ifndef A1_MYDB_PAGE_C
#define A1_MYDB_PAGE_C

#include <string>
#include <fstream>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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
    int fin;
    fin = open(table->getStorageLoc().data(), O_RDWR | O_CREAT, 0644);
    lseek(fin, tablePos*pageSize, SEEK_SET);
    read(fin, pageAdd, pageSize);
    close(fin);
}

void MyDB_Page::writeToDisk() {
    int fout;
    fout = open(table->getStorageLoc().data(), O_SYNC | O_RDWR | O_CREAT, 0644);
    lseek(fout, tablePos*pageSize, SEEK_SET);
    write(fout, pageAdd, pageSize);
    close(fout);
}

//Read anonymous page
void MyDB_Page::readFromDisk(string tempFile) {
    int fin;
    fin = open(tempFile.data(), O_RDWR | O_CREAT, 0644);
    lseek(fin, tablePos*pageSize, SEEK_SET);
    read(fin, pageAdd, pageSize);
    close(fin);
}

//Write anonymous page
void MyDB_Page::writeToDisk(string tempFile) {
    int fout;
    fout = open(tempFile.data(), O_SYNC | O_RDWR | O_CREAT, 0644);
    lseek(fout, tablePos*pageSize, SEEK_SET);
    write(fout, pageAdd, pageSize);
    close(fout);
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
    return ((!unUsed)&&(table->getName().compare(tab->getName())==0)&&(tablePos==pos));
}

#endif //A1_MYDB_PAGE_H
