// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "native_library.h"

#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <windows.h>

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

//#include "base/files/file_util.h"
//#include "base/strings/stringprintf.h"
//#include "base/strings/utf_string_conversions.h"
//#include "base/threading/thread_restrictions.h"

namespace base {

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
typedef HMODULE (WINAPI* LoadLibraryFunction)(const wchar_t* file_name, unsigned long res);
#else
typedef HMODULE (WINAPI* LoadLibraryFunction)(const wchar_t* file_name);
#endif

namespace {

NativeLibrary LoadNativeLibraryHelper(const std::string& library_path, 
             LoadLibraryFunction load_library_api, 
             NativeLibraryLoadError* error) { 
    // LoadLibrary() opens the file off disk. 
    //ThreadRestrictions::AssertIOAllowed(); 

    // Switch the current directory to the library directory as the library 
    // may have dependencies on DLLs in this directory. 
    WCHAR current_directory[MAX_PATH] = {0}; 
    bool restore_directory = false; 

    // THIS IS NOT THE PROPER WAY TO CONVERT A std::string TO A std::wstring! 
    // This only works properly for ASCII strings. You need to use 
    // MultiByteToWideChar() or std::wstring_convert or other equivalent 
    // to convert ANSI data to UNICODE data. Otherwise, library_path should 
    // be passed as a std::wstring to begin with... 
    // 
    std::wstring lp = std::wstring(library_path.begin(), library_path.end()); 

    std::wstring plugin_path, plugin_value;  
    const wchar_t *res = wcsrchr(lp.c_str(), L'\\'); 
    if (res) 
    { 
    plugin_path.assign(lp.c_str(), res); 
    plugin_value.assign(++res); 

    if (!plugin_path.empty()) 
    { 
     GetCurrentDirectoryW(MAX_PATH, current_directory); 
     restore_directory = SetCurrentDirectoryW(plugin_path.c_str()); 
    } 
    } 
    else 
    plugin_value = lp; 

    HMODULE module = (*load_library_api)(plugin_value.c_str()); 
    if (!module && error) { 
    // GetLastError() needs to be called immediately after |load_library_api|. 
    error->code = GetLastError(); 
    } 

    if (restore_directory) 
    SetCurrentDirectoryW(current_directory); 

    return module; 
} 

}  // namespace

std::string NativeLibraryLoadError::ToString() const
{
	char buf[32];
	return int2char(code, buf);
}

// static
NativeLibrary LoadNativeLibrary(const std::string& library_path,
                                NativeLibraryLoadError* error) {
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
  return LoadNativeLibraryHelper(library_path, LoadPackagedLibrary, error);
#else
  return LoadNativeLibraryHelper(library_path, LoadLibraryW, error);
#endif
}

NativeLibrary LoadNativeLibraryDynamically(const std::string& library_path) { 
    LoadLibraryFunction load_library; 
    load_library = reinterpret_cast<LoadLibraryFunction>(
     GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW")); 

    return LoadNativeLibraryHelper(library_path, load_library, NULL); 
} 

// static
void UnloadNativeLibrary(NativeLibrary library) {
  FreeLibrary(library);
}

// static
void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
                                          const char* name) {
  return (void (*))GetProcAddress(library, name);
}

// static
//string16 GetNativeLibraryName(const string16& name) {
//  return name + ASCIIToUTF16(".dll");
//}

}  // namespace base
