//===-- Forward declare Windows.h types -----------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef FWD_WINDOWS_H
#define FWD_WINDOWS_H

typedef unsigned __int64 WPARAM;
typedef __int64 LPARAM;
typedef __int64 LRESULT;


struct HINSTANCE__;
typedef struct HINSTANCE__* HINSTANCE;

struct HWND__;
typedef struct HWND__* HWND;

struct HICON__;
typedef struct HICON__* HICON;

#endif
