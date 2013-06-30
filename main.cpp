#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "FixedStr.hpp"
#include "FixedStrTest.h"

using std::cout;
using std::wcout;
using std::endl;

//
// Driver app for running unit tests.
//
int main(int argc, char** argv)
{
    try {
        FixedStrTest tests;
        tests.runTests();
    }
    catch (const std::exception& ex) {
        cout << "Tests failed:  " << ex.what() << endl; 
        return 1;
    }    
    return 0;
}