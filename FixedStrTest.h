/*
 *  FixedStrTest.h
 *  FixedStr
 *
 *  Created by Sean Dempsey on 12/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *  
 *  Unit tests for FixedStr.
 */

#include "SimpleTest.h"

class FixedStrTest : public SimpleTest {
public:
    FixedStrTest() {
    }

    void testSizeof();
    void testAssign();
    void testCtors();
    void testAppend();
    void testEmbeddedZeroes();
    void testEmbeddedZeroesW();
    void testNonMemberOpers();
    void testEqualStr();    
    void testWFixedStr();
    void testFormat();
    void testSubstring();
    void testPerf();


    void runTests() {
        // all tests must be called out here.
      
        testSizeof();
        testAssign();
        testCtors();
        testAppend();
        testEmbeddedZeroes();        
        testEmbeddedZeroesW();
        testNonMemberOpers();
        testEqualStr();        
        testWFixedStr();
        testFormat();        
        testSubstring();        
                        
        testPerf();
        
    }

private:
    // disable these...
    FixedStrTest(const FixedStrTest& other);
    FixedStrTest& operator=(const FixedStrTest& other);
};
