
#ifndef CATALOG_UNIT_H
#define CATALOG_UNIT_H

#include "../../Qunit/headers/QUnit.h"
#include "../../BufferMgr/headers/MyDB_BufferManager.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

void writeNums(char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '0' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeLetters (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = 'i' + (i % 10);
	}
	bytes[len - 1] = 0;
}

void writeSymbols (char *bytes, size_t len, int i) {
	for (size_t j = 0; j < len - 1; j++) {
		bytes[j] = '!' + (i % 10);
	}
	bytes[len - 1] = 0;
}

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);
	//string path = "/Users/danye/Documents/Courses/COMP530/assignment1/";
	string path = "/storage-home/d/dy13/comp530/A1";

	// UNIT TEST 1: A BIG ONE!!
	{

		// create a buffer manager 
		//MyDB_BufferManager myMgr (64, 16, "tempDSFSD");
		MyDB_BufferManager myMgr (64, 16, "temp");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("table1", path+"table1");

		// allocate a pinned page
		cout << "allocating pinned page\n";
		MyDB_PageHandle pinnedPage = myMgr.getPinnedPage (table1, 0);
		//MyDB_PageHandle pinnedPage = myMgr.getPage ();
		char *bytes = (char *) pinnedPage->getBytes ();
		writeNums (bytes, 64, 0);
		pinnedPage->wroteBytes ();

		
		// create a bunch of pinned pages and remember them
		vector <MyDB_PageHandle> myHandles;
		for (int i = 1; i < 10; i++) {
			cout << "allocating pinned page\n";
			MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
			//MyDB_PageHandle temp = myMgr.getPage ();
			char *bytes = (char *) temp->getBytes ();
			writeNums (bytes, 64, i);
			temp->wroteBytes ();
			myHandles.push_back (temp);
		}

		// now forget the pages we created
		vector <MyDB_PageHandle> temp;
		myHandles = temp;

		// now remember 8 more pages
		for (int i = 0; i < 8; i++) {
			cout << "allocating pinned page\n";
			MyDB_PageHandle temp = myMgr.getPinnedPage (table1, i);
			char *bytes = (char *) temp->getBytes ();

			// write numbers at the 0th position
			if (i == 0)
				writeNums (bytes, 64, i);
			else
				writeSymbols (bytes, 64, i);
			temp->wroteBytes ();
			myHandles.push_back (temp);
		}

		// now correctly write nums at the 0th position
		cout << "allocating unpinned page\n";
		MyDB_PageHandle anotherDude = myMgr.getPage (table1, 0);
		bytes = (char *) anotherDude->getBytes ();
		writeSymbols (bytes, 64, 0);
		anotherDude->wroteBytes ();
		
		// now do 90 more pages, for which we forget the handle immediately
		for (int i = 10; i < 100; i++) {
			cout << "allocating unpinned page\n";
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char *bytes = (char *) temp->getBytes ();
			writeNums (bytes, 64, i);
			temp->wroteBytes ();
		}

		// now forget all of the pinned pages we were remembering
		vector <MyDB_PageHandle> temp2;
		myHandles = temp2;

		// now get a pair of pages and write them
		for (int i = 0; i < 100; i++) {
			cout << "allocating pinned page\n";
			MyDB_PageHandle oneHandle = myMgr.getPinnedPage ();
			char *bytes = (char *) oneHandle->getBytes ();
			writeNums (bytes, 64, i);
			oneHandle->wroteBytes ();
			cout << "allocating pinned page\n";
			MyDB_PageHandle twoHandle = myMgr.getPinnedPage ();
			writeNums (bytes, 64, i);
			twoHandle->wroteBytes ();
		}

		// make a second table
		MyDB_TablePtr table2 = make_shared <MyDB_Table> ("table2", path+"table2");
		for (int i = 0; i < 100; i++) {
			cout << "allocating unpinned page\n";
			MyDB_PageHandle temp = myMgr.getPage (table2, i);
			char *bytes = (char *) temp->getBytes ();
			writeLetters (bytes, 64, i);
			temp->wroteBytes ();
		}
		
	}

	// UNIT TEST 2
	{
		MyDB_BufferManager myMgr (64, 16, path+"A1/temp");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("table1", path+"table1");

		// look up all of the pages, and make sure they have the correct numbers
		for (int i = 0; i < 100; i++) {
			MyDB_PageHandle temp = myMgr.getPage (table1, i);
			char answer[64];
			if (i < 8)
				writeSymbols (answer, 64, i);
			else
				writeNums (answer, 64, i);
			char *bytes = (char *) temp->getBytes ();
			QUNIT_IS_EQUAL (string (answer), string (bytes));
		}
	}

	//UNIT TEST 3
	{
		MyDB_BufferManager myMgr (4, 4, path+"A1/temp2");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("table3", path+"table3");
		int i;
		MyDB_PageHandle handle1[6];
		for (i = 0; i < 2; i ++){
			//cout << "====" << endl;
			handle1[i] = myMgr.getPinnedPage(table1, i%2);
			char *bytes = (char *)handle1[i]->getBytes();
			writeLetters(bytes, 4, i);
			handle1[i]->wroteBytes();
		}
		for (i = 2; i < 4; i ++){
			//cout << "====" << endl;
			handle1[i] = myMgr.getPage(table1, i%2);
			char *bytes = (char *)handle1[i]->getBytes();
			writeLetters(bytes, 4, i);
			handle1[i]->wroteBytes();
		}
		for (i = 4; i < 6; i ++){
			//cout << "====" << endl;
			handle1[i] = myMgr.getPage(table1, i);
			char *bytes = (char *)handle1[i]->getBytes();
			writeLetters(bytes, 4, i);
			handle1[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			//cout << "====" << endl;
			MyDB_PageHandle handle = myMgr.getPage(table1, i);
			char *bytes = (char *)handle->getBytes();
			writeLetters(bytes, 4, i);
			handle->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			//cout << "====" << endl;
			handle1[i] = myMgr.getPage();
			char *bytes = (char *)handle1[i]->getBytes();
			writeLetters(bytes, 4, i+3);
			handle1[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			//cout << "====" << endl;
			MyDB_PageHandle handle = myMgr.getPage(table1, i);
			char *bytes = (char *)handle->getBytes();
			writeLetters(bytes, 4, i+2);
			handle->wroteBytes();
		}
		for (i = 0; i < 6; i ++) {
			handle1[i] = myMgr.getPage(table1, i);
			char *bytes = (char *)handle1[i]->getBytes();
			char answer[4];
			writeLetters(answer, 4, i+2);
			QUNIT_IS_EQUAL((string)answer, (string)bytes);
		}
	}

	//UNIT TEST 4
	{
		MyDB_BufferManager myMgr (4, 6, path+"A1/temp3");
		MyDB_TablePtr table1 = make_shared <MyDB_Table> ("table4", path+"table4");
		int i;
		MyDB_PageHandle handle1[6];
		MyDB_PageHandle handle2[6];
		MyDB_PageHandle handle;
		for (i = 0; i < 6; i ++){
			handle1[i] = myMgr.getPage(table1, i);
			handle2[i] = myMgr.getPage(table1, i);
			char *bytes = (char *)handle1[i]->getBytes();
			writeLetters(bytes, 4, i);
			handle1[i]->wroteBytes();

			bytes = (char *)handle2[i]->getBytes();
			writeLetters(bytes, 4, i+4);
			handle2[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			handle1[i] = myMgr.getPage();
			char *bytes = (char *)handle1[i]->getBytes();
			writeNums(bytes, 4, i);
			handle1[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			handle1[i] = myMgr.getPage(table1, i);
			char *bytes = (char *)handle1[i]->getBytes();
			writeNums(bytes, 4, i+2);
			handle1[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			char *bytes = (char *)handle2[i]->getBytes();
			writeNums(bytes, 4, i+10);
			handle2[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			handle2[i] = myMgr.getPage();
			char *bytes = (char *)handle2[i]->getBytes();
			writeNums(bytes, 4, i+22);
			handle2[i]->wroteBytes();
		}
		for (i = 0; i < 6; i ++){
			handle = myMgr.getPage(table1, i);
			char *bytes = (char*)handle->getBytes();
			char answer[4];
			writeNums(answer, 4, i+10);
			QUNIT_IS_EQUAL((string)answer, (string)bytes);
		}
	}
}

#endif
