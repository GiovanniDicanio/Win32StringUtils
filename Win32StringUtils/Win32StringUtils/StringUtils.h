////////////////////////////////////////////////////////////////////////////////
// 
//                  Win32 STL String Utility Functions
//
//                      by Giovanni Dicanio 
//                 <giovanni.dicanio AT gmail.com>
// 
//----------------------------------------------------------------------------
// 
// FILE: StringUtils.h
// DESC: Header-only module containing Win32 STL string utility functions.
// 
//----------------------------------------------------------------------------
//
// Advantages of STL strings over ATL::CString:
//
//  - SSO (Small String Optimization)
//  - move semantics
//  - usable in range-for loop
//  - iterator interface
//  - well integrated with Boost
//  - C++11-compliant non-CoW (good for multi-threaded code)
//  
// Use std::wstring since Unicode UTF-16 is the "native" Unicode format 
// for Win32 APIs.
//
// Consider Boost String Algorithms Library as a set of additional features
// for STL strings.
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef GIOVANNI_DICANIO_WIN32_STRINGUTILS_H
#define GIOVANNI_DICANIO_WIN32_STRINGUTILS_H


#include <stdarg.h>     // For _vsnwprintf_s
#include <stdio.h>      // For _vsnwprintf_s, _vscwprintf
#include <string>       // For std::wstring
#include <Windows.h>    // Win32 Platform SDK


namespace win32
{

//==============================================================================
//                      String Utility Functions
//==============================================================================

//------------------------------------------------------------------------------
// Optimized hex formatters for binary types.
// Use upper case letters (A-F).
// The "0x" prefix is *not* appended for better flexibility.
//------------------------------------------------------------------------------

std::wstring FormatHResult(HRESULT hr);
std::wstring FormatByte(BYTE b);
std::wstring FormatWord(WORD w);
std::wstring FormatDword(DWORD dw);


//------------------------------------------------------------------------------
// Printf-like formatting to std::wstring.
//------------------------------------------------------------------------------
std::wstring StringPrintf(const wchar_t* format, ...);

//------------------------------------------------------------------------------
// Load string from resources.
// If the function can't find the requested string in resources, 
// returns an empty string.
//------------------------------------------------------------------------------
std::wstring LoadStringResource(HINSTANCE hInstance, UINT stringId);



//==============================================================================
//                          Implementation
//==============================================================================

namespace detail
{ 

//------------------------------------------------------------------------------
// Return a wchar_t containing the hex digit (0-9, A-F) of the *low* nibble
// in the input byte.
//------------------------------------------------------------------------------
inline wchar_t FormatLowNibble(const BYTE b)
{
    // Mask out the high nibble from byte
    const BYTE lowNibble = b & 0x0F;

    if (lowNibble >= 0 && lowNibble < 10)
    {
        // 0 - 9
        return L'0' + lowNibble;
    }
    else
    {
        // A - F
        return L'A' + (lowNibble - 10);
    }
}

} // namespace detail


inline std::wstring FormatByte(const BYTE b)
{
    wchar_t hexDigits[2];
    hexDigits[0] = detail::FormatLowNibble((b >> 4));
    hexDigits[1] = detail::FormatLowNibble(b);

    return std::wstring(hexDigits, _countof(hexDigits));
}


inline std::wstring FormatWord(const WORD w)
{
    wchar_t hexDigits[4];

    const BYTE highByte = static_cast<BYTE>(w >> 8);
    hexDigits[0] = detail::FormatLowNibble((highByte >> 4));
    hexDigits[1] = detail::FormatLowNibble(highByte);

    const BYTE lowByte = static_cast<BYTE>(w & 0x00FF);
    hexDigits[2] = detail::FormatLowNibble((lowByte >> 4));
    hexDigits[3] = detail::FormatLowNibble(lowByte);

    return std::wstring(hexDigits, _countof(hexDigits));
}


inline std::wstring FormatDword(const DWORD dw)
{
    const WORD lowWord = dw & 0x0000FFFF;   
    const WORD highWord = dw >> 16;

    return FormatWord(highWord) + FormatWord(lowWord);
}


inline std::wstring FormatHResult(const HRESULT hr)
{
    return FormatDword(hr);
}


inline std::wstring LoadStringResource(const HINSTANCE hInstance, const UINT stringId)
{
    wchar_t * buffer = nullptr;

    const int len = ::LoadString(
        hInstance,
        stringId,
        reinterpret_cast<wchar_t*>(&buffer),
        0);

    if (len)
    {
        return std::wstring(buffer, len);
    }
    else
    {
        return std::wstring();
    }
}


inline std::wstring StringPrintf(const wchar_t* const format, ...)
{
    va_list args;

    // Get result string length
    va_start(args, format);
    const int length = _vscwprintf(format, args) + 1; // +1 for terminating NUL
    va_end(args);

    // Make room in the destination string
    std::wstring result;
    result.resize(length);

    // Format using printf-like syntax
    va_start(args, format);
    _vsnwprintf_s(&result[0], length, _TRUNCATE, format, args);
    va_end(args);

    // Chop off the terminating NUL written by _vsnwprintf_s
    // (as the wstring is *already* NUL-terminated)
    result.pop_back();

    return result;
}


} // namespace win32


#endif // GIOVANNI_DICANIO_WIN32_STRINGUTILS_H

