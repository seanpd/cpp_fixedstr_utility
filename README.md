# FixedStr utility C++ class

1-feb-2021

Sean Dempsey

This is a very simple string class which attempts to
allocate its storage within the object itself using an embedded array.
If there's insufficient space to store the content it
automatically uses the heap.

Let's say you need a string to hold what's normally a smallish size.
You might do something like this:

```cpp
char myStr [20];
```

This is allocated efficiently on the stack.  But what if there's a chance the string can overflow?  That's the problem FixedStr tries to solve.

You'd say:

```cpp
FixedStr<20>  myStr;

WFixedStr<20> myWideCharStr;
```

The unit test FixedStrTest.cpp has example usage.
It has functions for copying, appending, printf-type formatting etc.

Notes:

1.  This is not intended to be a comprehensive string class.  If you 
    need more stuff, use std::string or whatever.  It only has 
    the bare essentials.

1.  It's intended only for strings expected to fit inside.  Otherwise
    this doesn't provide much value.

1.  The main goal is to avoid unneeded heap usage.




