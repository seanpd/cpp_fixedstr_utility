/*
 *  FixedStrTest.cpp
 *  FixedStr
 *
 *  Created by Sean Dempsey on 12/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "FixedStrTest.h"
#include "FixedStr.hpp"
#include <iostream>
using std::cout;
using std::wcout;
using std::endl;

void FixedStrTest::testSizeof() {

    FixedStr<5> f1;
    int sz = 0;

    // 16.  (8 for int members + 8 for array, padded)
    sz = sizeof f1;
    cout << "sizeof f1: " << sz << endl;

    FixedStr<8> f2;
    sz = sizeof f2;

    FixedStr<80> f3;
    sz = sizeof f3;
}

void FixedStrTest::testAssign() {

    FixedStr<5> f1;
    // assign null.
    f1.assign(NULL);
    assertEquals ("empty", "", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    // assign empty.
    f1.assign("");
    assertEquals ("empty", "", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    // at max size using internal array.
    f1.assign ("foooo");
    assertEquals ("init assign", "foooo", f1.c_str());
    assertEquals ("length", 5, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());
    
    // copy into an array of a different size.
    FixedStr<30> copy1 = f1;
    assertEquals ("copy1", "foooo", copy1.c_str());
    assertEquals ("isUsingOverflow", 0, copy1.isUsingOverflow());
    assertEquals ("alloc", 30, copy1.getAlloc());

    // new string needs overflow.
    FixedStr<4> copy2 = f1;
    assertEquals ("copy2", "foooo", copy2.c_str());
    assertEquals ("isUsingOverflow", 1, copy2.isUsingOverflow());
    assertEquals ("alloc", 5, copy2.getAlloc());

    // will cause overflow.
    f1.assign ("fooooZ");
    assertEquals ("cause overflow", "fooooZ", f1.c_str());
    assertEquals ("length", 6, f1.length());
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());
    assertEquals ("alloc", 6, f1.getAlloc());

    // existing overflow too small.
    f1.assign ("0123456");
    assertEquals ("expand overflow", "0123456", f1.c_str());
    assertEquals ("length", 7, f1.length());
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());
    assertEquals ("alloc", 7, f1.getAlloc());

    // overflow deleted.
    f1.assign ("abc");
    assertEquals ("init assign", "abc", f1.c_str());
    assertEquals ("length", 3, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    f1.assign ("");
    assertEquals ("init assign", "", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());
}

void FixedStrTest::testCtors() {
    
    FixedStr<5> f1;
    assertEquals ("empty", "", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    FixedStr<5> f2 ("foobar");
    assertEquals ("", "foobar", f2.c_str());
    assertEquals ("length", 6,  f2.length());
    assertEquals ("isUsingOverflow", 1, f2.isUsingOverflow());
    assertEquals ("alloc", 6, f2.getAlloc());    

    f1.assign ("foooo");
    
    // copy into an array of a different size.
    FixedStr<30> copy1 = f1;
    assertEquals ("copy1", "foooo", copy1.c_str());
    assertEquals ("isUsingOverflow", 0, copy1.isUsingOverflow());
    assertEquals ("alloc", 30, copy1.getAlloc());

    // new string needs overflow.
    FixedStr<4> copy2 = f1;
    assertEquals ("copy2", "foooo", copy2.c_str());
    assertEquals ("isUsingOverflow", 1, copy2.isUsingOverflow());
    assertEquals ("alloc", 5, copy2.getAlloc());

    // operator= same size.
    FixedStr<5> opEq;
    opEq = f1;
    assertEquals ("opEq", "foooo", opEq.c_str());
    assertEquals ("isUsingOverflow", 0, opEq.isUsingOverflow());
    assertEquals ("alloc", 5, opEq.getAlloc());

    // self-assignment
    opEq = opEq;
    assertEquals ("opEq", "foooo", opEq.c_str());

    // operator= different size
    FixedStr<4> opEq2;
    opEq2 = f1;
    assertEquals ("opEq2", "foooo", opEq2.c_str());
    assertEquals ("isUsingOverflow", 1, opEq2.isUsingOverflow());
    assertEquals ("alloc", 5, opEq2.getAlloc());

    // reassign
    FixedStr<10> str1 ("0123");
    opEq2 = str1;
    assertEquals ("opEq2", "0123", opEq2.c_str());
    assertEquals ("isUsingOverflow", 0, opEq2.isUsingOverflow());
    assertEquals ("alloc", 4, opEq2.getAlloc());

    // operator= from a string.  
    FixedStr<10> str2 ("0123");
        
    const char* newStuff = "new stuff";
    
    str2 = newStuff;
    assertEquals ("str2", "new stuff", str2.c_str());

}

void FixedStrTest::testAppend() {
    FixedStr<5> f1;
        
    const char* nullStr = NULL;
    f1.append(nullStr);
    assertEquals ("empty", "", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());
        
    f1 += '0';
    assertEquals ("init append", "0", f1.c_str());
    assertEquals ("length", 1, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    f1 += '1';
    assertEquals ("append", "01", f1.c_str());
    assertEquals ("length", 2, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    f1 +=  "234";
    assertEquals ("append", "01234", f1.c_str());
    assertEquals ("length", 5, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    f1 += "5";
    assertEquals ("append", "012345", f1.c_str());
    assertEquals ("length", 6, f1.length());
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());
    
    // size doubled.
    assertEquals ("alloc", 12, f1.getAlloc());

    // fits in existing overflow.
    f1 += "678901";
    assertEquals ("append", "012345678901", f1.c_str());
    assertEquals ("length", 12, f1.length());
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());    
    assertEquals ("alloc", 12, f1.getAlloc());

    // overflow needs expansion.
    f1 += "2";
    assertEquals ("append", "0123456789012", f1.c_str());
    assertEquals ("length", 13, f1.length());
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());    
    assertEquals ("alloc", 26, f1.getAlloc());
}

 void FixedStrTest::testEmbeddedZeroes() {
    FixedStr<4> f1;
    char c1[] = "abcde";
    c1[2] = '\0';
    
    // leaving off the 'e'
    f1.assign(c1, 4);
    // naturally truncated
    assertEquals ("assign-truncated", "ab", f1.c_str());
    assertEquals ("zero", 0, f1.c_str() [2]);    
    assertEquals ("after zero", "d", f1.c_str()+3);
    assertEquals ("length", 4, f1.length()); 
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());    

    f1 += 'e';
    assertEquals ("assign-truncated", "ab", f1.c_str());
    assertEquals ("zero", 0, f1.c_str() [2]);    
    assertEquals ("after zero", "de", f1.c_str()+3);
    assertEquals ("length", 5, f1.length()); 
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());   
    
    f1.clear();
    f1 += '\0';
    f1 += "foobar";
    f1 += '\0';
    f1 += '\0';
    f1 += "bazz";
    const char* res = f1.c_str();
    assertEquals ("r1", 0, res[0]);
    assertEquals ("r2", "foobar", res+1);
    assertEquals ("r3", 0, res[7]);
    assertEquals ("r4", 0, res[8]);
    assertEquals ("r5", "bazz", res+9);            
 }

 void FixedStrTest::testEmbeddedZeroesW() {
    WFixedStr<4> f1;
    wchar_t c1[] = L"abcde";
    c1[2] = '\0';
    
    // leaving off the 'e'
    f1.assign(c1, 4);
    // naturally truncated
    assertEquals ("assign-truncated", L"ab", f1.c_str());
    assertEquals ("zero", 0, f1.c_str() [2]);    
    assertEquals ("after zero", L"d", f1.c_str()+3);
    assertEquals ("length", 4, f1.length()); 
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());    

    f1 += 'e';
    assertEquals ("assign-truncated", L"ab", f1.c_str());
    assertEquals ("zero", 0, f1.c_str() [2]);    
    assertEquals ("after zero", L"de", f1.c_str()+3);
    assertEquals ("length", 5, f1.length()); 
    assertEquals ("isUsingOverflow", 1, f1.isUsingOverflow());   
    
    f1.clear();
    f1 += L'\0';
    f1 += L"foobar";
    f1 += L'\0';
    f1 += L'\0';
    f1 += L"bazz";
    const wchar_t* res = f1.c_str();
    assertEquals ("r1", 0, res[0]);
    assertEquals ("r2", L"foobar", res+1);
    assertEquals ("r3", 0, res[7]);
    assertEquals ("r4", 0, res[8]);
    assertEquals ("r5", L"bazz", res+9);            
}
 
void FixedStrTest::testNonMemberOpers() {
    FixedStr<4> empty;
    FixedStr<3> foo ("foo");      
    FixedStr<4> foo_2 ("foo");
    FixedStr<10> bar ("bar");
    FixedStr<10> barr ("barr");
    FixedStr<2>  fop  ("fop");

    FixedStr<2>  fopy  ("fopy");
    FixedStr<2>  fonz  ("fonz");

    FixedStr<2>  fon  ("fon");    
    FixedStr<4>  fooo ("fooo");
    assertTrue  ("operator==",   foo == foo_2);        
    assertFalse ("operator!=",   foo != foo_2);    

    assertFalse ("operator==",   foo == bar);        
    assertTrue  ("operator!=",   foo != bar);    

    assertTrue  ("operator==",   foo == foo);       

    assertTrue  ("operator== empty", empty == empty);       
    assertTrue  ("operator== empty", empty != foo);           
    
    assertTrue  ("operator==", foo != barr);       

    assertFalse ("operator<", empty < empty);       
    assertFalse ("operator<", foo  < foo);       
    assertFalse ("operator<", foo  < foo_2);       
    assertTrue  ("operator<", bar   < foo);    
    assertTrue  ("operator<", foo  < fop);       

    assertFalse ("operator<", fopy  < fonz);       

    assertTrue  ("operator<", empty < foo);       
    assertFalse ("operator<", foo   < empty);       
    assertTrue  ("operator<", foo   < fooo); 
    assertFalse ("operator<", fooo  < foo);
    
    // check for unsigned comparison.    
    const char* minCstr = "\x00";
    const char* maxCstr = "\xff";
    
    FixedStr<4> min;
    min.assign(minCstr, 1);
    
    FixedStr<4> max  (maxCstr);
    assertTrue("signed test", min < max);
    
    WFixedStr<4> wFob   (L"fob");
    WFixedStr<4> wFoo   (L"foo");
    assertTrue("signed test", wFob < wFoo);
        
}

void FixedStrTest::testWFixedStr() {

    // mostly testing the ctors.
    WFixedStr<5> ft;
    ft.assign (L"abcde");
    const wchar_t* v = ft.c_str();    
    assertEquals("init", L"abcde", v);
    
    WFixedStr<5> f1;
    assertEquals ("empty", L"", f1.c_str());
    assertEquals ("length", 0, f1.length());
    assertEquals ("isUsingOverflow", 0, f1.isUsingOverflow());
    assertEquals ("alloc", 5, f1.getAlloc());

    WFixedStr<5> f2 (L"foobar");
    assertEquals ("", L"foobar", f2.c_str());
    assertEquals ("length", 6,  f2.length());
    assertEquals ("isUsingOverflow", 1, f2.isUsingOverflow());
    assertEquals ("alloc", 6, f2.getAlloc());    

    f1.assign (L"foooo");
    
    // copy into an array of a different size.
    WFixedStr<30> copy1 = f1;
    assertEquals ("copy1", L"foooo", copy1.c_str());
    assertEquals ("isUsingOverflow", 0, copy1.isUsingOverflow());
    assertEquals ("alloc", 30, copy1.getAlloc());

    // new string needs overflow.
    WFixedStr<4> copy2 = f1;
    assertEquals ("copy2", L"foooo", copy2.c_str());
    assertEquals ("isUsingOverflow", 1, copy2.isUsingOverflow());
    assertEquals ("alloc", 5, copy2.getAlloc());

    // operator= same size.
    WFixedStr<5> opEq;
    opEq = f1;
    assertEquals ("opEq", L"foooo", opEq.c_str());
    assertEquals ("isUsingOverflow", 0, opEq.isUsingOverflow());
    assertEquals ("alloc", 5, opEq.getAlloc());

    // self-assignment
    opEq = opEq;
    assertEquals ("opEq", L"foooo", opEq.c_str());

    // operator= different size
    WFixedStr<4> opEq2;
    opEq2 = f1;
    assertEquals ("opEq2", L"foooo", opEq2.c_str());
    assertEquals ("isUsingOverflow", 1, opEq2.isUsingOverflow());
    assertEquals ("alloc", 5, opEq2.getAlloc());

    // reassign
    WFixedStr<10> str1 (L"0123");
    opEq2 = str1;
    assertEquals ("opEq2", L"0123", opEq2.c_str());
    assertEquals ("isUsingOverflow", 0, opEq2.isUsingOverflow());
    assertEquals ("alloc", 4, opEq2.getAlloc());
}

void FixedStrTest::testFormat() {
    typedef FixedStr<12> fstr_t;
    fstr_t s1;
    bool ok = s1.format("foo: %d-%s", 5, "abc");
    assertEquals ("format-1", "foo: 5-abc", s1.c_str());
    assertEqualsBool("result", true, ok);

    // at the limit
    s1.format("%s", "012345678901");
    assertEquals ("format-2", "012345678901", s1.c_str());
    assertFalse("overflow", s1.isUsingOverflow());

    // one over; needs overflow.
    s1.format("%s", "0123456789012");
    assertEquals ("format-3", "0123456789012", s1.c_str());
    assertTrue("overflow", s1.isUsingOverflow());

    // now this fits.
    s1.format("%s", "0123456789012");
    assertEquals ("format-4", "0123456789012", s1.c_str());
    assertTrue("overflow", s1.isUsingOverflow());

    // wchar_t, at the limit
    WFixedStr<5> ws1;
    ws1.format(L"%S", L"01234");
    assertEquals ("format-5", L"01234", ws1.c_str());
    assertFalse("overflow", ws1.isUsingOverflow());

    ws1.format(L"%S", L"012345");
    assertEquals ("format-6", L"012345", ws1.c_str());
    assertTrue("overflow", ws1.isUsingOverflow());

    ws1.format(L"%S", L"0123456");
    assertEquals ("format-7", L"0123456", ws1.c_str());
    assertTrue("overflow", ws1.isUsingOverflow());

    // Multiple iterations of trying to allocate.
    WFixedStr<4> ws2;
    ws2.format(L"%S", L"01234567890123456789");
    assertEquals ("format-7", L"01234567890123456789", ws2.c_str());
    assertTrue("overflow", ws2.isUsingOverflow());
        
}

void FixedStrTest::testSubstring() {

    FixedStr<5> f1;
    FixedStr<4> f2;
    
    f1.assign("01234");
    
    // will fit in f2.
    f1.substring(f2, 0, 2);    
    assertEquals ("substring-1", "01", f2.c_str());
           
    // requires expansion.
    FixedStr<2> f3;
    f1.substring(f2, 0, 3);    
    assertEquals ("substring-2", "012", f2.c_str());
       
    f1.substring(f2, 1, 4);    
    assertEquals ("substring-3", "123", f2.c_str());

    f1.substring(f2, 1, 0);    
    assertEquals ("substring-empty", "", f2.c_str());
    

    f1.substring(f2, 1, 0);    
    assertEquals ("substring-empty", "", f2.c_str());

    f1.substring(f2, 1, 5);    
    assertEquals ("substring-remainder", "1234", f2.c_str());

    f1.substring(f2, 1, -1);    
    assertEquals ("substring-remainder", "1234", f2.c_str());

    WFixedStr<4> wf1;
    WFixedStr<2> wf2;

    wf1 = L"01234";
    wf1.substring(wf2, 2, 4);
    assertEquals ("substring-w", L"23", wf2.c_str());
    
        
}




