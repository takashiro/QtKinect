#ifndef KINECT_STDAFX_H
#define KINECT_STDAFX_H

//------------------------------------------------------------------------------
// <copyright file="stdafx.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

// include file for standard system and project includes

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// #define _WIN32_WINNT 0x0601             // Compile against Windows 7 headers

// Windows Header Files
#include <windows.h>

#include <Shlobj.h>

// Direct2D Header Files
#include <d2d1.h>

#pragma comment ( lib, "d2d1.lib" )

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// Safe release for interfaces
template<class Interface>
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
    if ( pInterfaceToRelease != nullptr )
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = nullptr;
    }
}

inline void SafeCloseHandle(HANDLE handle)
{
    if (handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
    }
}

#define CLOSING_FROM_STATUSCHANGED    1
#define READING_TEXT_BUFFER_SIZE    128

/// <summary>
/// Ensure the font object has been created correctly
/// </summary>
inline void EnsureFontCreated(HFONT& hFont, int fontSize, int fontWeight)
{
    if (nullptr == hFont)
    {
        hFont = CreateFontW(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    }
}

/// <summary>
/// Ensure the image object has been loaded correctly
/// </summary>
inline void EnsureImageLoaded(HBITMAP& hBitmap, UINT resourceId)
{
    if (nullptr == hBitmap)
    {
        hBitmap = LoadBitmapW(GetModuleHandle(NULL), MAKEINTRESOURCEW(resourceId));
    }
}

/// <summary>
/// Ensure the icon object has been loaded correctly
/// </summary>
inline void EnsureIconLoaded(HICON& hIcon, UINT resourceId)
{
    if (nullptr == hIcon)
    {
        hIcon = LoadIconW(GetModuleHandle(NULL), MAKEINTRESOURCEW(resourceId));
    }
}

inline SIZE GetWindowSize(HWND hWnd)
{
    RECT rect;
    GetWindowRect(hWnd, &rect);

    SIZE size = {rect.right - rect.left, rect.bottom - rect.top};
    return size;
}

inline SIZE GetClientSize(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    SIZE size = {rect.right, rect.bottom};
    return size;
}

// Safe delete pointers
template<class T>
inline void SafeDelete(T*& ptr)
{
    if(ptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

// Safe delete array
template<class T>
inline void SafeDeleteArray(T*& pArray)
{
    if(pArray)
    {
        delete[] pArray;
        pArray = nullptr;
    }
}

// Compare the reading, format and update to static control if it changed
template<class T>
void CompareUpdateValue(const T newValue, T& storedValue, HWND hWnd, UINT controlID, LPCWSTR format)
{
    if(storedValue != newValue)
    {
        storedValue = newValue;

        WCHAR buffer[READING_TEXT_BUFFER_SIZE];
        swprintf_s(buffer, READING_TEXT_BUFFER_SIZE, format, newValue);
        SetDlgItemTextW(hWnd, controlID, buffer);
    }
}

#endif // KINECT_STDAFX_H
