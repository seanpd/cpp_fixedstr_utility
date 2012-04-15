#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H

/*
 *  SimpleTest.h
 *  FixedStr
 *
 *  Created by Sean Dempsey on 12/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 *  Very simple xUnit-ish test 'framework'.  I explicitly wanted to
 *  to avoid dependencies and just need something simple.
 */
 
 class SimpleTest {
 public:
    SimpleTest() {
    }
 
    void assertEquals (const char* msg, const char*    expected, const char*    actual);
    void assertEquals (const char* msg, const wchar_t* expected, const wchar_t* actual);
    void assertEquals (const char* msg, int  expected, int  actual);
    void assertEqualsBool (const char* msg, bool expected, bool actual);
    void assertFalse  (const char* msg, bool v);
    void assertTrue   (const char* msg, bool v);    
    void fail(const char* msg);

private:
    // disable these...
    SimpleTest(const SimpleTest& other);
    SimpleTest& operator=(const SimpleTest& other);
         
};
 
 #endif

