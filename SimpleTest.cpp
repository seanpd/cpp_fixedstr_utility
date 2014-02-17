/*
 *  SimpleTest.cpp
 *  FixedStr
 *
 *  Created by Sean Dempsey on 12/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleTest.h"
#include <stdexcept>
#include <string>
#include <cstring>

using std::string;

namespace {
    string toNarrow(const wchar_t* str) {
        string toRet;
        if (str == NULL) {
            toRet = "[null]";
            return toRet;
        }
        size_t len = wcslen(str);
        for (size_t i = 0; i<len; ++i) {
            wchar_t ch = str[i];
            if (ch <= 0xff) {
                toRet += ch;                
            }
            else {
                toRet += '?';
            }
        }
        return toRet;
    }

} // end blank namespace

void SimpleTest::assertEquals (const char* msg, const char* expected, const char* actual) {
    if (expected == actual) {
        return;
    }
    bool mismatch = false;
    if (expected == NULL || actual == NULL) {
        // one is non-null; otherwise would have returned above.
        mismatch = true;
    }
    if (!mismatch && strcmp (expected, actual) != 0) {
        mismatch = true;
    }
    if (!mismatch) {
        return;
    }
    string errMsg;
    if (msg != NULL && msg[0] != '\0') {
        errMsg += msg;
        errMsg += ":  ";
    }
    errMsg += "expected <";
    errMsg += expected != NULL ? expected : "[null]";
    errMsg += "> actual <";
    errMsg += actual != NULL   ? actual   : "[null]";
    errMsg += ">";
    fail(errMsg.c_str()); 
}

void SimpleTest::assertEquals (const char* msg, const wchar_t* expected, const wchar_t* actual) {
    if (expected == actual) {
        return;
    }
    bool mismatch = false;
    if (expected == NULL || actual == NULL) {
        // one is non-null; otherwise would have returned above.
        mismatch = true;
    }
    if (!mismatch && wcscmp (expected, actual) != 0) {
        mismatch = true;
    }
    if (!mismatch) {
        return;
    }
    string errMsg;
    if (msg != NULL && msg[0] != '\0') {
        errMsg += msg;
        errMsg += ":  ";
    }
    errMsg += "expected <";
    errMsg += toNarrow(expected);
    errMsg += "> actual <";
    errMsg += toNarrow(actual);
    errMsg += ">";
    fail(errMsg.c_str()); 

}


void SimpleTest::assertEquals (const char* msg, int expected, int actual) {
    if (expected != actual) {
        string errMsg;
        if (msg != NULL && msg[0] != '\0') {
            errMsg += msg;
            errMsg += ":  ";
        }
        char buff[80];
        sprintf (buff, "expected <%d> actual <%d>", expected, actual);
        errMsg += buff;
        fail(errMsg.c_str());
    }
}

void SimpleTest::assertEqualsBool (const char* msg, bool expected, bool actual) {
    if (expected != actual) {
        string errMsg;
        if (msg != NULL && msg[0] != '\0') {
            errMsg += msg;
            errMsg += ":  ";
        }
        char buff[80];
        sprintf (buff, "expected <%s> actual <%s>", 
                expected ? "true" : "false", 
                actual   ? "true" : "false");
        errMsg += buff;
        fail(errMsg.c_str());
    }
}

void SimpleTest::assertFalse (const char* msg, bool v) {
    assertEqualsBool (msg, false, v);
} 

void SimpleTest::assertTrue (const char* msg, bool v) {
    assertEqualsBool (msg, true, v);
} 


void SimpleTest::fail(const char* msg) {
    throw std::runtime_error (msg);
}


