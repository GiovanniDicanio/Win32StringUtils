////////////////////////////////////////////////////////////////////////////////
//
// Win32 STL String Utils ** Tests **
//
// by Giovanni Dicanio
// 
////////////////////////////////////////////////////////////////////////////////


#include "StringUtils.h"    // Module to test
#include "resource.h"       // For testing loading string from resources
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>
#include <atlstr.h>


//
// Use CString::Format as reference result for formatting functions.
// 

inline CString ExpectedFormatByte(BYTE b)
{
    CString str;
    str.Format(L"%02X", b);
    return str;
}

inline CString ExpectedFormatWord(WORD w)
{
    CString str;
    str.Format(L"%04X", w);
    return str;
}

inline CString ExpectedFormatDword(DWORD dw)
{
    CString str;
    str.Format(L"%08X", dw);
    return str;
}

inline CString ExpectedFormatHResult(HRESULT hr)
{
    CString str;
    str.Format(L"%08X", hr);
    return str;
}


//
// Very Simple Test Infrastructure
// 

inline bool Equal(const std::wstring& s1, const CString& s2)
{
    return (s1.length() == static_cast<size_t>(s2.GetLength())) && (s1 == s2.GetString());
}

inline void Check(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::logic_error(message);
    }
}


//
// Various Tests
// 

void TestFormatByte()
{
    for (int i = 0; i < 256; i++)
    {
        const BYTE b = static_cast<BYTE>(i);
        std::wstring s1 = win32::FormatByte(b);
        CString s2 = ExpectedFormatByte(b);

        Check(Equal(s1, s2), "FormatByte result differs from expected result.");
    }
}


void TestFormatWord()
{
    const int maxWord = static_cast<int>((std::numeric_limits<WORD>::max)());
    for (int i = 0; i <= maxWord; i++)
    {
        const WORD w = static_cast<BYTE>(i);
        std::wstring s1 = win32::FormatWord(w);
        CString s2 = ExpectedFormatWord(w);

        Check(Equal(s1, s2), "FormatWord result differs from expected result.");
    }
}


void TestFormatDword()
{
    const std::vector<DWORD> testDwords
    {
        0x00000000,
        0xFFFFFFFF,
        0x0000FFFF,
        0xFFFF0000,
        0x12345678,
        0xABCDEFAB
    };
    for (const auto& dw : testDwords)
    {
        std::wstring s1 = win32::FormatDword(dw);
        CString s2 = ExpectedFormatDword(dw);

        Check(Equal(s1, s2), "FormatDword result differs from expected result.");
    }
}


void TestFormatHResult()
{
    const std::vector<HRESULT> testHResults
    {
        S_OK,
        E_FAIL,
        E_INVALIDARG,
        E_OUTOFMEMORY,
    };
    for (const auto& hr : testHResults)
    {
        std::wstring s1 = win32::FormatHResult(hr);
        CString s2 = ExpectedFormatHResult(hr);

        Check(Equal(s1, s2), "FormatHResult result differs from expected result.");
    }
}


void TestStringPrintf()
{
    std::wstring s1 = win32::StringPrintf(L"Testing StringPrintf %s %d 0x%08X.", L"Hello",
        10, E_INVALIDARG);

    CString s2;
    s2.Format(L"Testing StringPrintf %s %d 0x%08X.", L"Hello", 10, E_INVALIDARG);

    Check(Equal(s1, s2), "StringPrintf result differs from expected result.");
}


void TestLoadStringResource()
{
    std::wstring resString1 = win32::LoadStringResource(GetModuleHandle(nullptr), IDS_TEST_STRING);
    
    CString resString2;
    resString2.LoadString(IDS_TEST_STRING);
    
    Check(Equal(resString1, resString2), "LoadStringResource result differs from expected result.");
}


int main()
{
    constexpr int kExitOk = 0;
    constexpr int kExitError = 1;
    int exitCode = kExitOk;

    try
    {
        std::wcout << L"*** Testing Win32 StringUtils ***\n\n";
        std::wcout << L"    by Giovanni Dicanio\n\n";

        TestFormatByte();
        TestFormatWord();
        TestFormatDword();
        TestFormatHResult();
        TestStringPrintf();
        TestLoadStringResource();

        std::wcout << L"All right!\n";
    }
    catch (const std::exception& e)
    {    
        std::wcerr << L"\n*** ERROR: " << e.what() << L'\n';
        exitCode = kExitError;
    }

    return exitCode;
}

