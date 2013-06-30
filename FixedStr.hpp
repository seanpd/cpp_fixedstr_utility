#ifndef FIXED_STR_H
#define FIXED_STR_H

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <wchar.h>
#include <stdexcept>
#include <limits.h>

/*
 *  FixedStr
 *  Very simple yet-another string class.  This is intended for
 *  cases where the max reasonable size is known at compile time.
 */

namespace {
    
////////////////////////
// We try to put as much logic as possible outside the template.
// Otherwise we could have massive template bloat.  We don't want
// FixedStr<10> and FixedStr<11> to each have their own impls of all the code!
////////////////////////

    inline void my_va_copy(va_list& newVa, va_list& origVa) {
#ifndef _MSC_VER
        // C99 macro, but widely available.
        va_copy(newVa, origVa);
#else
        // Visual Studio doesn't have va_copy() but this works on VS and 
        // most other platforms.  Not AMD64.
        newVa = origVa;
#endif
    }

    // Moved outside to avoid template bloat.
    // returns new overflowAlloc.    
    template<typename _CharT>
    inline size_t assignImpl (
    
                    // new string to add.
                    const _CharT*   newStr, 
                    size_t          newStrLen, 
                    
                    // sizeof array below
                    size_t          alloc, 
                    
                    // fixed allocated array
                    _CharT*         array, 
                    
                    // if too big for 'array', where to store content.
                    _CharT**        overflow,
                    
                    // only access if 'overflow' non null; otherwise it's garbage
                    // due to the union.
                    size_t          overflowAlloc) {

        if (newStrLen == 0) {
            // not dereferencing 'newStr' -- allows a null pointer passed in.
            array[0] = '\0';
            delete[] *overflow;
            *overflow = NULL;
            return 0;            
        }
        if (newStrLen <= alloc) {
            // fits in the static array.
            memcpy (array, newStr, newStrLen * sizeof (_CharT));
            array[newStrLen] = '\0';
            
            // get rid of existing heap if needed.
            delete[] *overflow;
            *overflow = NULL;
            // if array is used, overflowAlloc will be garbage due to union.
            return 0;
        } // fits
        else {
            // too big for static array.
            if (!*overflow || newStrLen > overflowAlloc) {
                // existing alloc too small.
                // if overflow null, overflowAlloc will have gibberish.
                delete [] *overflow;
                // +1:  allow for terminator. 
                *overflow = new _CharT[newStrLen + 1];
                overflowAlloc = newStrLen;
            }                        
            memcpy (*overflow, newStr, newStrLen * sizeof (_CharT));
            (*overflow)[newStrLen] = '\0';                     
            return overflowAlloc;
        }
        // won't get here.
        return 0;        
    }
    
    // returns new overflowAlloc.        
    template<typename _CharT>
    inline size_t appendImpl (
                    const _CharT*   newStr, 
                    size_t          newStrLen,  
                    
                    // modified.
                    size_t*         origLen,
                    size_t          alloc, 
                    _CharT*         array, 
                    _CharT**        overflow,
                    size_t          overflowAlloc) {

        size_t sizeNeeded = *origLen + newStrLen;
        size_t newOverflowAlloc = 0;
        _CharT* target = NULL;
        // not a loop; break out idiom.
        do {
            if (sizeNeeded <= alloc) {
                // fits in array
                newOverflowAlloc = 0;
                target = array;
                break;
            }
            if (*overflow && sizeNeeded <= overflowAlloc) {
                // fits in existing heap alloc
                newOverflowAlloc = overflowAlloc;
                target = *overflow;
                break;
            }
            size_t expandSz = sizeNeeded*2;
            if (!*overflow) {
                // existing array but out of space.  Need to use overflow.
                // +1:  allow for terminator.
                delete [] *overflow;
                *overflow = new _CharT[expandSz+1];
                // original terminator isn't copied, because after doing this
                // it's expected we'll append to the string.
                memcpy (*overflow, array, *origLen * sizeof (_CharT));
                newOverflowAlloc = expandSz;
                target = *overflow;
                break;
            }
            // existing overflow but out of space.
            _CharT* newOverflow = new _CharT[expandSz+1];
            memcpy (newOverflow, *overflow, *origLen * sizeof (_CharT));
            delete [] *overflow;
            *overflow = newOverflow;
            newOverflowAlloc = expandSz;
            target = *overflow;                    
        } while (false);
        
        // if 'newStrLen' 0 not dereferencing 'newStr' -- allows a null pointer passed in.          
        if (newStrLen > 0) {
            memcpy(target + *origLen, newStr, newStrLen * sizeof (_CharT));
            *origLen += newStrLen;        
            target[*origLen] = '\0';            
        }                                        
        return newOverflowAlloc;    
    }
        
    // returns new overflowAlloc.    
    inline size_t formatImpl (
                    const char*     formatStr, 
                    va_list         *args,     
                    size_t*         origLen,
                    size_t          alloc, 
                    char*           array, 
                    char**          overflow,
                    size_t          overflowAlloc,
                    
                    // returned.  (I could have used a ref but this
                    // may be more clear)
                    bool*           ok) {

        *ok = false;
        size_t newOverflowAlloc = 0;
        char*  buff;
        size_t buffAlloc;
        if (!*overflow) {
            buff =      array;
            buffAlloc = alloc;
        }
        else {
            buff =             *overflow;
            buffAlloc =        overflowAlloc;
            newOverflowAlloc = overflowAlloc;
        }
        // alloc+1:  we already have space for the null terminator.
        va_list argsHold;
        // In case we need to try vsnprintf again.
        my_va_copy (argsHold, *args);
        int required = vsnprintf (buff, buffAlloc+1, formatStr, *args);
        if (required < 0) {
            // error.
            va_end (argsHold);
            return newOverflowAlloc;
        }
        if (required > alloc) {
            // It didn't fit.                        
            // This shouldn't be a common use case.
            // don't use asprintf() -- not standard.  
            delete [] *overflow;                      
            *overflow = new char[required+1];
            newOverflowAlloc = required;
            buff = *overflow;
            int res = vsnprintf (buff, required+1, formatStr, argsHold);
            if (res < 0) {
                va_end (argsHold);
                return newOverflowAlloc;
            }                    
        }    
        *origLen = required;
        *ok = true;
        va_end (argsHold);        
        return newOverflowAlloc;
    }

    // returns new overflowAlloc.    
    inline size_t wideFormatImpl (
                    const wchar_t*  formatStr, 
                    va_list         *args,     
                    size_t*         origLen,
                    size_t          alloc, 
                    wchar_t*        array, 
                    wchar_t**       overflow,
                    size_t          overflowAlloc,                    
                    bool*           ok) {

        *ok = false;
        size_t newOverflowAlloc = 0;
        wchar_t*  buff;
        size_t    buffAlloc;
        if (!*overflow) {
            buff =      array;
            buffAlloc = alloc;
        }
        else {
            buff =             *overflow;
            buffAlloc =        overflowAlloc;
            newOverflowAlloc = overflowAlloc;
        }
        
        va_list argsHold;
        my_va_copy (argsHold, *args);
        
        // alloc+1:  we already have space for the null terminator.
        // Unlike vsnprintf() this returns -1 if it doesn't fit in
        // the buffer (not the count needed).              
        int result = vswprintf (buff, buffAlloc+1, formatStr, *args);
        if (result >= 0) {            
            *origLen = result;
            *ok = true;
            va_end (argsHold);
            // good result.
            return newOverflowAlloc;
        }
        // TODO:  If something like _vscwprintf is available, take advantage of it.
        size_t maxIters = 4;
        
        for (size_t i = 0; i < maxIters; ++i) {
            // Keep trying a bigger buffer until it fits.
            // Inelegant but this is really abusing the purpose of this class.
            buffAlloc *= 2;
            delete [] *overflow;                      
            *overflow = new wchar_t[buffAlloc+1];
            newOverflowAlloc = buffAlloc;
            buff = *overflow;
            
            va_list argsHere;
            my_va_copy (argsHere, argsHold);            
            int result = vswprintf (buff, buffAlloc+1, formatStr, argsHere);
            if (result >= 0) {            
                *origLen = result;
                *ok = true;
                va_end (argsHere);
                va_end (argsHold);                
                return newOverflowAlloc;
            }                            
            va_end (argsHere);

        }
        // Punt
        // Maybe we should throw an exception?  
        // The trouble is callers may not be prepared to deal with exceptions
        // vswprintf() and friends return bad status codes.
        *ok = false;
        va_end (argsHold);                        
        return newOverflowAlloc;
    }

    // generic strlen()-type function.
    template<typename _CharT>
    inline size_t countLen (
                    const _CharT* str) {
       size_t len = 0;
       if (!str) {
            return 0;
       }
       while (*str != '\0') {
            ++str;
            ++len;
       }
       return len;                      
    } 

    template<typename _CharT>
    void clearImpl(
            size_t*     len,
            _CharT*     array,
            _CharT**    overflow) {

        *len = 0;
        array[0] = '\0';
        delete [] *overflow;
        *overflow = NULL;
    }

    // generic equality comparison.
    template<typename _CharT>
    inline bool isEqualImpl (
                    const _CharT*   lhs, 
                    size_t          lhsLen, 
                    const _CharT*   rhs, 
                    size_t          rhsLen) {

        if (lhsLen != rhsLen) {
            return false;
        } 
        
        for (size_t i=0; i<lhsLen; ++i) {
            if (*lhs != *rhs) {
                return false;
            }
            ++lhs;
            ++rhs;
        }
        return true;                           
    }

#if UINT_MAX == 4294967295 && CHAR_BIT == 8

    // optimized equality check for char strings.
    // Note the assumptions it makes for these sizes.
    inline bool isEqualImpl_char (
                    const char*   lhs, 
                    size_t        lhsLen, 
                    const char*   rhs, 
                    size_t        rhsLen) {

        if (lhsLen != rhsLen) {
            return false;
        } 
        
        const int* lhsRaw = reinterpret_cast<const int*> (lhs);
        const int* rhsRaw = reinterpret_cast<const int*> (rhs);

        size_t big_loop = lhsLen / 4;

        // Comparing 4 bytes at a time.
        // This is usually at least twice as fast but could be slower
        // for comparing short strings due to the extra logic needed.
        for (size_t i=0; i<big_loop; ++i) {
            if (*lhsRaw != *rhsRaw) {
                return false;
            }
            ++lhsRaw;
            ++rhsRaw;
        }
        
        size_t offset = big_loop * 4;
        size_t remain = lhsLen - offset;
        lhs += offset;
        rhs += offset;
        // assumes an int is 4 chars...
        switch (remain) {
            case 3:
               if (*lhs != *rhs) {
                  return false;
               }
               ++lhs;
               ++rhs;
            // intentional case fall-through.
            case 2:
               if (*lhs != *rhs) {
                  return false;
               }
               ++lhs;
               ++rhs;            
            case 1:
               if (*lhs != *rhs) {
                  return false;
               }
        }
        return true;        
    }
#endif
        
    template<typename _CharT, typename _UnsignedCharT>
    inline bool isLessImpl (
                    const _CharT*   lhs, 
                    size_t          lhsLen, 
                    const _CharT*   rhs, 
                    size_t          rhsLen) {
                    
        for (size_t i=0; i<lhsLen; ++i) {
            if (i >= rhsLen) {
                // left hand side longer
                return false;
            }        
            // ensure characters are compared as unsigned.
            // This matches strcmp() behavior.
            // (by default gcc assumes plain char as signed).
            _UnsignedCharT lhsCh = static_cast<_UnsignedCharT> (*lhs);
            _UnsignedCharT rhsCh = static_cast<_UnsignedCharT> (*rhs);
            int diff = lhsCh - rhsCh;
            if (diff != 0) {
                return diff < 0 ? true : false;
            }
            ++lhs;
            ++rhs;
        } // each char.
    
        if (lhsLen == rhsLen) {
            // same length; same chars (could both be length 0)
            return false;
        }
    
        // If we make it here, the left hand side is shorter.
        return true;                                                
    }
                        
 } // blank namespace

///////////////
// main class template.
///////////////

template<size_t _AllocSizeT, typename _CharT>
class BaseStr {
public:   

    /////////////////////////
    // ctor, dtor,...
    /////////////////////////
    BaseStr () 
        :
        m_len(0),
        m_overflow(NULL) {
        
        m_array[0] = '\0';
    }

    // Copy ctor; works on  with different
    // allocations.
    template<size_t newAllocT>
    BaseStr (const BaseStr<newAllocT, _CharT>& newStr)
        :
        m_len(0),
        m_overflow(NULL) {
        
        assign (newStr.c_str(), newStr.length());
    }

    explicit BaseStr (const _CharT* newStr)
        :
        m_len(0),
        m_overflow(NULL) {
        assign (newStr);
    }

    ~BaseStr() {
        delete [] m_overflow;
    }

    // assigning same-alloc strings; also detects self-assignment
    BaseStr<_AllocSizeT, _CharT>& operator=(const BaseStr<_AllocSizeT, _CharT>& rhs) {
        if (this == &rhs) {
            return *this;
        }
        assign (rhs.c_str(), rhs.length());
        return *this;
    }

    // assigns different-alloc strings.
    template<size_t newAllocT>
    BaseStr<_AllocSizeT, _CharT>& operator=(const BaseStr<newAllocT, _CharT>& rhs) {
        // self-assignment can't occur.
        assign (rhs.c_str(), rhs.length());
        return *this;
    }

    // assign from a C string.
    BaseStr<_AllocSizeT, _CharT>& operator=(const _CharT* rhs) { 
        assign (rhs);
        return *this;
    }
    
    /////////////////////////////////
    // accessors
    /////////////////////////////////
    const  _CharT* c_str() const {
        return m_overflow ? m_overflow : m_array; 
    }

    size_t length() const {
        return m_len;
    }

    bool empty() const {
        return m_len == 0;
    }

    bool isUsingOverflow() const {
        return m_overflow;
    }

    size_t getAlloc() const {    
        return m_overflow ? m_overflowAlloc : _AllocSizeT;
    }
  
    // Returned value is passed into 'target' (ref parameter) instead of 
    // returning an object.  It may look a little funny but it avoids having to copy.
    template<size_t targetAllocT>
    void substring (BaseStr<targetAllocT, _CharT>& target, size_t start, size_t end) {
    
        // TODO:  move this range checking logic outside.
        if (start >= length()) {
            // I'd prefer not using exceptions here but this seems reasonable.
            throw std::out_of_range("FixedStr.substring() start out of range");
        }

        if (end < start) {
            end = start;
        }
        if (end == -1 || end > length()) {
            // use remainder of string.
            end = length();
        }
    
        const _CharT *p = c_str();
        target.assign(p+start, end-start);
    }
    
    /////////////////////////////////
    // simple mutators.
    /////////////////////////////////
    
    void clear() {
        clearImpl (&m_len, m_array, &m_overflow);
    }
    
    void assign (const _CharT* newStr, size_t newStrLen) {
    
        // can't refer to both m_array and m_overflowAlloc due to union.
        // note how we avoid dereferencing m_overflowAlloc.
        size_t newOverflowAlloc = 
                    assignImpl (
                        newStr, 
                        newStrLen,
                        _AllocSizeT, 
                        m_array,
                        &m_overflow, 
                        m_overflowAlloc);

        if (newOverflowAlloc != 0) {
            // If it returns non-zero we're using the heap.
            m_overflowAlloc = newOverflowAlloc;
        }
        m_len = newStrLen;        
    }
    
    void assign (const _CharT* newStr) {
        assign (newStr, countLen(newStr));
    }
      
    BaseStr<_AllocSizeT, _CharT>&  append(const _CharT* newStr, size_t newStrLen) {
        size_t newOverflowAlloc = appendImpl(
                    newStr,
                    newStrLen,
                    &m_len,
                    _AllocSizeT, 
                    m_array,
                    &m_overflow, 
                    m_overflowAlloc);

        if (newOverflowAlloc != 0) {
            // If it returns non-zero we're using the heap.
            m_overflowAlloc = newOverflowAlloc;
        }
        return *this;
    }

    BaseStr<_AllocSizeT, _CharT>& append (const _CharT* newStr) {
        return append (newStr, countLen(newStr));
    } 

    template<size_t origAlloc>
    BaseStr<_AllocSizeT, _CharT>& append(const BaseStr<origAlloc, _CharT>& newStr) {
        append(newStr.c_str(), newStr.length());
        return *this;
    }
    
    BaseStr<_AllocSizeT, _CharT>& append(_CharT ch) {  
        _CharT arr[1];
        arr[0] = ch;
        append(arr, 1);        
        return *this;
    }
    
    BaseStr<_AllocSizeT, _CharT>& operator+=(const _CharT* newStr) {
        return append(newStr);
    }
    
    template<size_t origAlloc>
    BaseStr<_AllocSizeT, _CharT>& operator+=(const BaseStr<origAlloc, _CharT>& newStr) {
        return append(newStr);
    }
    
    BaseStr<_AllocSizeT, _CharT>& operator+=(_CharT ch) {
        return append(ch);
    }
                
protected:

    // Used so we don't need to strlen()...
    // Doesn't include terminator.
    size_t           m_len;
    
    // If heap was needed it goes here...
    // If null, then m_array is valid.  Otherwise m_overflowAlloc is valid.
     _CharT*         m_overflow;

    // Below we use a union because if we have to use an overflow
    // then m_array is unusable so we can use its space for something else.
    // TODO:  change so a 16 bit len is outside the union, but the other stuff inside. 
    union {
        // size is fixed; based on int template.
        // +1 to include terminator.
         _CharT          m_array [_AllocSizeT+1];
        
        // If heap was needed; alloc size.  Real alloc size +1 to account for terminator.
        size_t           m_overflowAlloc;        
    };
    
}; // end class.


////////////////////
// Subclasses for the specific string types.
// These subclasses don't add state but are included for these reasons:
// 1.  Allows the caller to say 'FixedStr<10> str' instead of 'BaseStr<10, char> str'.
//     Using a typedef like how wstring refers to basic_string<wchar_t> doesn't work
//     with the extra _AllocSizeT param.
// 2.  Allows to impl char-specific format() impls.  We could probably try template specialization
//     but can't specialize part of a class; this would be a huge mess.
////////////////////

template<size_t _AllocSizeT>
class FixedStr : public BaseStr<_AllocSizeT, char>  {
public: 
    FixedStr() 
    {
    }

    template<size_t newAllocT>
    FixedStr (const FixedStr<newAllocT>& newStr)
        :
        BaseStr<_AllocSizeT, char> (newStr)
    {
    }

    // ok
    explicit FixedStr (const char* newStr)
        :
        BaseStr<_AllocSizeT, char> (newStr)
    {
    }

    // We need this because operator= doesn't inherit.
    // The copy assignment operator (where rhs is the same type as 'this')
    // is compiler generated and calls the base version.  However we need 
    // our own impl here for other types. 
    FixedStr<_AllocSizeT>& operator=(const char* rhs) { 
        BaseStr<_AllocSizeT, char>::operator=(rhs);
        return *this;
    }
        
    // printf-style formatting.        
    bool format (const char* formatStr, ...) {
        va_list args;
        va_start(args, formatStr);    
        bool ok = false;            
        size_t newOverflowAlloc = formatImpl(
                    formatStr,
                    &args,
                    &(this->m_len),
                    _AllocSizeT, 
                    this->m_array,
                    &(this->m_overflow), 
                    this->m_overflowAlloc,
                    &ok);

        if (newOverflowAlloc != 0) {
            // If it returns non-zero we're using the heap.
            this->m_overflowAlloc = newOverflowAlloc;
        }  
        va_end (args);
        return ok;
    }
                        
};  

//
// wide char version.
template<size_t _AllocSizeT>
class WFixedStr : public BaseStr<_AllocSizeT, wchar_t>  {
public: 
    WFixedStr () 
    {
    }

    template<size_t newAllocT>
    WFixedStr (const WFixedStr<newAllocT>& newStr)
        :
        BaseStr<_AllocSizeT, wchar_t> (newStr)
    {
    }

    // ok
    explicit WFixedStr (const wchar_t* newStr)
        :
        BaseStr<_AllocSizeT, wchar_t> (newStr)
    {
    }
    
    WFixedStr<_AllocSizeT>& operator=(const wchar_t* rhs) { 
        BaseStr<_AllocSizeT, wchar_t>::operator=(rhs);
        return *this;
    }
    
    bool format (const wchar_t* formatStr, ...) {
        va_list args;
        va_start(args, formatStr);                    
        bool ok = false;            
        size_t newOverflowAlloc = wideFormatImpl(
                    formatStr,
                    &args,
                    &(this->m_len),
                    _AllocSizeT, 
                    this->m_array,
                    &(this->m_overflow), 
                    this->m_overflowAlloc,
                    &ok);

        if (newOverflowAlloc != 0) {
            // If it returns non-zero we're using the heap.
            this->m_overflowAlloc = newOverflowAlloc;
        }  
        va_end(args);
        return ok;
    }
                            
};  

//////////////////////////
// non-member operators.
//////////////////////////

template<size_t origAlloc1, size_t origAlloc2, typename _CharT>
bool operator==(const BaseStr<origAlloc1, _CharT> &lhs,
                const BaseStr<origAlloc2, _CharT> &rhs)
{
    return isEqualImpl (
                lhs.c_str(), lhs.length(),
                rhs.c_str(), rhs.length()); 
}


#if UINT_MAX == 4294967295 && CHAR_BIT == 8
template<size_t origAlloc1, size_t origAlloc2>
bool operator==(const FixedStr<origAlloc1> &lhs,
                const FixedStr<origAlloc2> &rhs)
{
    return isEqualImpl_char (
                lhs.c_str(), lhs.length(),
                rhs.c_str(), rhs.length()); 
}
#endif


template<size_t origAlloc1, size_t origAlloc2, typename _CharT>
bool operator!=(const BaseStr<origAlloc1, _CharT> &lhs,
                const BaseStr<origAlloc2, _CharT> &rhs)
{
    return !(lhs==rhs);
}

template<size_t origAlloc1, size_t origAlloc2>
bool operator<( const BaseStr<origAlloc1, char> &lhs,
                const BaseStr<origAlloc2, char> &rhs)
{
    // Compiler won't be able to figure out template params.
    return isLessImpl<char, unsigned char> (
                lhs.c_str(), lhs.length(),
                rhs.c_str(), rhs.length());       
}

template<size_t origAlloc1, size_t origAlloc2>
bool operator<( const BaseStr<origAlloc1, wchar_t> &lhs,
                const BaseStr<origAlloc2, wchar_t> &rhs)
{
    return isLessImpl<wchar_t, unsigned wchar_t> (
                lhs.c_str(), lhs.length(),
                rhs.c_str(), rhs.length());       
}


#endif
