/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestFFJSON.cpp
 * Author: gowtham
 *
 * Created on 26 August, 2016, 10:49 PM
 */

#include <stdlib.h>
#include <iostream>
#include <logger.h>
#include <ferrybase/FerryTimeStamp.h>
#include <ferrybase/mystdlib.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <stdio.h>  
#include <string.h>
#include "FFJSON.h"

/*
 * Simple C++ Test Suite
 */
using namespace std;
int child_exit_status = 0;
FF_LOG_TYPE fflAllowedType = (FF_LOG_TYPE)(FFL_DEBUG | FFL_INFO);
unsigned int fflAllowedLevel = 9;

void test1() {
	std::cout << "TestFFJSON test 1" << std::endl;
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof (cCurrentPath))) {
		return;
	}

	cCurrentPath[sizeof (cCurrentPath) - 1] = '\0'; /* not really required */
	char c = '\r';
	ffl_info(1, "\\r=%d", (int) c);
	ffl_info(1, "The current working directory is %s", cCurrentPath);
	fflush(stdout);
	string fn = "sample.ffjson";
	//string fn = "/home/gowtham/Projects/ferrymediaserver/output.ffjson";
	ifstream ifs(fn.c_str(), ios::in | ios::ate);
	string ffjsonStr;
	ifs.seekg(0, std::ios::end);
	ffjsonStr.reserve(ifs.tellg());
	ifs.seekg(0, std::ios::beg);
	ffjsonStr.assign((std::istreambuf_iterator<char>(ifs)),
			std::istreambuf_iterator<char>());
	FFJSON ffo(ffjsonStr);
	cout << "amphibians: " << endl;
	FFJSON::Iterator i = ffo["amphibians"].begin(); //["amphibians"]
	while ((i != ffo["amphibians"].end())) {
		cout << string(i) << ":" << i->stringify() << endl;
		++i;
	}
	cout << endl;
	std::string ps = ffo.prettyString(false, true);
	cout << ps << endl;
	FFJSON ffo2(ps);
	ffo2["amphibians"]["genome"].setEFlag(FFJSON::E_FLAGS::B64ENCODE);
	ffo2["amphibians"]["salamanders"] = "salee";
	std::string ps2 = ffo2.prettyString(false, true);
	cout << ps2 << endl;
	ffo2["amphibians"]["salamanders"] = "malee";
	ffo2["amphibians"]["count"] = 4;
	ffo2["amphibians"]["density"] = 5.6;
	ffo2["amphibians"]["count"] = 5;
	ffo2["amphibians"]["genome"] = "<xml>sadfalejhjroifndk</xml>";
	if (ffo2["amphibians"]["gowtham"]) {
		cout << ffo2["amphibians"].size << endl;
		ffo2["amphibians"].trim();
		cout << ffo2["amphibians"].size << endl;
	};
	ffo2["animals"][3] = "satish";
	cout << ffo2["animals"][4].prettyString() << endl;
	cout << "size: " << ffo2["animals"].size << endl;
	ffo2["animals"][3] = "bear";
	cout << "after bear inserted at 4" << ffo2["animals"].prettyString() << endl;
	cout << "size: " << ffo2["animals"].size << endl;
	ffo2["animals"].trim();
	cout << "size after trim: " << ffo2["animals"].size << endl;
	std::string ps3 = ffo2.prettyString();
	cout << ps3 << endl;
	cout << "FFJSON signature size: " << sizeof (ffo2) << endl;

	std::cout << "sizeInfo test 1" << std::endl;

	std::cout << "size of char: " << sizeof (char) << std::endl;
	std::cout << "size of short: " << sizeof (short) << std::endl;
	std::cout << "size of int: " << sizeof (int) << std::endl;
	std::cout << "size of long: " << sizeof (long) << std::endl;
	std::cout << "size of long long: " << sizeof (long long) << std::endl;

	std::cout << "size of float: " << sizeof (float) << std::endl;
	std::cout << "size of double: " << sizeof (double) << std::endl;

	std::cout << "size of pointer: " << sizeof (int *) << std::endl;

	ffo2["amphibians"]["frogs"].setQType(FFJSON::QUERY_TYPE::QUERY);
	ffo2["amphibians"]["salamanders"].setQType(FFJSON::QUERY_TYPE::DELETE);
	ffo2["amphibians"]["genome"].setQType(FFJSON::QUERY_TYPE::SET);
	ffo2["birds"][1].setQType(FFJSON::QUERY_TYPE::DELETE);
	ffo2["birds"][2].setQType(FFJSON::QUERY_TYPE::SET);
	ffo2["birds"][3].setQType(FFJSON::QUERY_TYPE::QUERY);
	string query = ffo2.queryString();
	ffo2["amphibians"]["genome"] = "<xml>gnomechanged :p</xml>";
	ffo2["birds"][2] = "kiwi";
	cout << ffo2.prettyString() << endl;
	cout << query << endl;
	FFJSON qo(query);
	query = qo.queryString();
	cout << query << endl;

	if (ffo2["amphibians"]["frogs"].isEFlagSet(FFJSON::E_FLAGS::EXTENDED)) {
		std::cout << "already extended" << std::endl;
	}
	FFJSON* ao = ffo2.answerObject(&qo);
	if (ffo2["amphibians"]["frogs"].isEFlagSet(FFJSON::E_FLAGS::EXTENDED)) {
		std::cout << "already extended" << std::endl;
	}

	cout << ao->stringify() << endl;
	string ffo2a = ffo2.prettyString();
	cout << ffo2a << endl;
	FFJSON ffo2ao(ffo2a);
	ffo2a = ffo2ao.stringify();
	cout << ffo2a << endl;
	ffo2a = ffo2ao.prettyString();
	cout << ffo2a << endl;
	delete ao;
	cout << "3rd students Maths marks: " <<
			ffo2["studentsMarks"][2]["Maths"].prettyString() << endl;
	cout << "end of test" << endl;
	return;
}

struct testStruct {
	string* s;
};

void test2() {
	std::cout << "TestFFJSON test 2" << std::endl;
	string fn = "/home/gowtham/Projects/ferrymediaserver/offpmpack.json";
	ifstream ifs(fn.c_str(), ios::in | ios::ate);
	if (ifs.is_open()) {
		string ffjsonStr;
		ifs.seekg(0, std::ios::end);
		ffjsonStr.reserve(ifs.tellg());
		ifs.seekg(0, std::ios::beg);
		ffjsonStr.assign((std::istreambuf_iterator<char>(ifs)),
				std::istreambuf_iterator<char>());
		FFJSON ffo(ffjsonStr);
		ffo["ferryframes"].setEFlag(FFJSON::B64ENCODE);
		string* s = new string(ffo.stringify(true));
		cout << *s << endl;
		s->append(":)");
		testStruct ts;
		ts.s = s;
		delete ts.s;
	}
	cout << "%TEST_PASSED%" << endl;
}

void test3() {
	cout << "===================================================" << endl;
	cout << "               TestFFJSON test 3                   " << endl;
	cout << "===================================================" << endl;

	FFJSON sample("file://sample.ffjson");
	if ((int) sample["donkeys"] < 4) {
		cout << "alert: my donkey is missing" << endl;
	}
	if (strcmp("John", sample["example"]["employees"][0]["firstName"]) == 0) {
		cout << "info: yes John is fist employee!" << endl;
	}
	cout << "%TEST_PASSED%" << endl;
}

void test4() {
	cout << "===================================================" << endl;
	cout << "			TestFFJSON test 4 (testing links)		" << endl;
	cout << "===================================================" << endl;
	FFJSON f("file://sample.ffjson");
}

void test5() {
	cout << "===================================================" << endl;
	cout << "		TestFFJSON test 5 (testing extensions)		" << endl;
	cout << "===================================================" << endl;
	FFJSON f("file://ExtensionTest.ffjson");
	cout << f.prettyString() << endl;

	cout << "Marks[0]['Maths']: " << f["Marks"][0]["Maths"].prettyString()
			<< endl;

	cout << "StudentsMarks['Gowtham']['Maths']: "
			<< f["School"]["Class1"]["StudentsMarks"]["Gowtham"]["Maths"].prettyString()
			<< endl;
	FFJSON f2(f.prettyString());
	cout << f2.prettyString() << endl;

	FFJSON f3(f2);
	cout << "f3 StudentsMarks['Gowtham']['Maths']: "
			<< f3["School"]["Class1"]["StudentsMarks"]["Gowtham"]["Maths"].prettyString()
			<< endl;
	FFJSON f4(f2.stringify());
	cout << f4.stringify() << endl;
}

void test6() {
	cout << "===================================================" << endl;
	cout << "	TestFFJSON test 6 (testing data type sizes)		" << endl;
	cout << "===================================================" << endl;
	std::map<string, FFJSON*> m;
	std::pair<string, FFJSON*> p(std::string("gowtham"), (FFJSON*) NULL);
	cout << &p.first << endl;
	m.insert(p);
	cout << &(*m.find("gowtham")) << endl;
	cout << &(*m.find("gowtham")) << endl;
	int i;
	FFJSON f;
	std::vector<string*> v;
	std::map<string, FFJSON*>::iterator it;
	cout << "map:" << sizeof (m) << endl;
	cout << "int:" << sizeof (i) << endl;
	cout << "ffjson:" << sizeof (f) << endl;
	cout << "vector:" << sizeof (v) << endl;
	cout << "iterator:" << sizeof (it) << endl;
	cout << "FerryTimeStamp:" << sizeof (FerryTimeStamp) << endl;
    cout << "FerryTimeStamp&:" << sizeof (FerryTimeStamp&) << endl;
	cout << "double:" << sizeof (double) << endl;
    
}

void test7() {
	cout << "===================================================" << endl;
	cout << "	TestFFJSON test 7 (testing MultiLineArray)		" << endl;
	cout << "===================================================" << endl;
	FFJSON f("file://MultiLineArray.ffjson");
	string sF = f.prettyString();
	cout << sF << endl;
	FFJSON f2(sF);
	string sF2 = f2.stringify();
	cout << f2 << endl;
	FFJSON f3(sF2);
	string sF3 = f3.prettyString();
	cout << sF3 << endl;
	FFJSON f4(sF3);
	string sF4 = f4.stringify();
	cout << sF4 << endl;
	FFJSON f5(sF4);
	string sF5 = f5.prettyString();
	cout << sF5 << endl;
}

void test8() {
	cout << "===================================================" << endl;
	cout << "           TestFFJSON test 8 sample.ffjson         " << endl;
	cout << "===================================================" << endl;
	FFJSON f("file://example.json");
	cout << f.prettyString() << endl;
	FFJSON f2(f.prettyString());
	string sF2 = f2.stringify();
	cout << f2 << endl;
	FFJSON f3(sF2);
	string sF3 = f3.prettyString();
	cout << sF3 << endl;

}

void test9() {
	cout << "===================================================" << endl;
	cout << "						erase test					" << endl;
	cout << "===================================================" << endl;
	FFJSON f("{}");
	f["cameras"].erase(string("cam"));

}

struct MyStruct {
	int tv_sec;
	int tv_nsec;
};

void test10() {
	cout << "===================================================" << endl;
	cout << "				 typecast   test					" << endl;
	cout << "===================================================" << endl;
	FFJSON f("{}");
	f["a"] = *(new timespec());
	timespec& t = (timespec&) f["a"];
	FFJSON& ff = f;
	timespec& tt = (timespec&) ff["a"];
	tt.tv_sec = 'a';
	tt.tv_nsec = 'b';
	cout << ff << endl;
	cout << "parsing string" << endl;
	FFJSON f3(ff.prettyString());
	cout << f3 << endl;
	timespec& t3 = (timespec&) f3["a"];
	cout << (char) t3.tv_sec << "," << (char) t3.tv_nsec << endl;
}

void test11() {
	cout << "===================================================" << endl;
	cout << "		subscript operator exection flow			" << endl;
	cout << "===================================================" << endl;
	FFJSON f("{}");
	f["a"]["b"] = 2;
	FFJSON& b = f["a"]["b"];
	int bb = (int) f["a"]["b"];

}

void test12() {
	cout << "===================================================" << endl;
	cout << "					update query					" << endl;
	cout << "===================================================" << endl;
    FFJSON f("file://UpdateTest.json");
	int j = 10;
	while (j) {
		cout << "Creating new answer object: " << endl;
		FFJSON tf("{}");
        static FerryTimeStamp ft;
        FFJSON* ff = f.answerObject(&tf,NULL,ft);
        ft.Update();
        if(!ff)return;
		cout << *ff << endl;
		f["newState"] = "RECORD";
		j--;
	}
}

//void test1() {
//	std::cout << "mytest1" << std::endl;
//}
//
//void test2() {
//	std::cout << "mytest2" << std::endl;
//	std::cout << "%TEST_PASSED% time=0 testname=test2 (TestFFJSON) message=error message sample" << std::endl;
//}

int main(int argc, char** argv) {
	std::cout << "%SUITE_STARTING% TestFFJSON" << std::endl;
	std::cout << "%SUITE_STARTED%" << std::endl;

	FerryTimeStamp ftsStart;
	FerryTimeStamp ftsEnd;
	FerryTimeStamp ftsDiff;
	FerryTimeStamp ftsSuiteStart;
    FerryTimeStamp ftsSuiteEnd;
    ftsSuiteStart.Update();

	std::cout << "%TEST_STARTED% test1 (TestFFJSON)" << std::endl;
	ftsStart.Update();
	test1();
    ftsEnd.Update();
	ftsDiff = ftsEnd-ftsStart;
	std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test1 (TestFFJSON)" << std::endl;

	std::cout << "%TEST_STARTED% test2 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test2();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test2 (TestFFJSON)" << std::endl;

	std::cout << "%TEST_STARTED% test3 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test3();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test3 " << std::endl;

	std::cout << "%TEST_STARTED% test4 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test4();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test4 " << std::endl;

	std::cout << "%TEST_STARTED% test5 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test5();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test5 " << std::endl;

    std::cout << "%TEST_STARTED% test6 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test6();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test6 " << std::endl;

	std::cout << "%TEST_STARTED% test7 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test7();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test7 " << std::endl;


	std::cout << "%TEST_STARTED% test8 (TestFFJSON)\n" << std::endl;
    ftsStart.Update();
    test8();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test8 " << std::endl;

	std::cout << "%TEST_STARTED% test9\n" << std::endl;
    ftsStart.Update();
    test9();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test9 " << std::endl;

	std::cout << "%TEST_STARTED% test10\n" << std::endl;
    ftsStart.Update();
    test10();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test10 " << std::endl;

	std::cout << "%TEST_STARTED% test11\n" << std::endl;
    ftsStart.Update();
    test11();
    ftsEnd.Update();
    ftsDiff = ftsEnd-ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test11 " << std::endl;

    std::cout << "%TEST_STARTED% test12\n" << std::endl;
    ftsStart.Update();
    test12();
    ftsEnd.Update();
    ftsDiff = ftsEnd - ftsStart;
    std::cout << "%TEST_FINISHED% time=" << ftsDiff << " test12 " << std::endl;

    ftsSuiteEnd.Update();
	ftsDiff = ftsSuiteEnd-ftsSuiteStart;
	std::cout << "%SUITE_FINISHED% time=" << ftsDiff << std::endl;

	return (EXIT_SUCCESS);
}
