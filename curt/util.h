//===-- Declaration of Win32 wrapper functions ----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_WIN_UTIL_H
#define CURT_WIN_UTIL_H

#include "api_params.h"
#include "fwd_windows.h"
#include "types.h"

#include <string>

namespace curt {
    
HINSTANCE thisModule();

std::string wideToUtf8(std::wstring wide);
std::wstring utf8ToWide(std::string utf8);

std::wstring loadStringW(unsigned int resId);
std::string loadString(unsigned int resId);

WPARAM pumpMessages();
WPARAM pumpMessages(HandleOr<HWND> translateWnd, HACCEL accelTable);

void setControlBackground(HandleOr<HWND> wnd, unsigned long rgb);

Font defaultFont();
void setControlToDefaultFont(HandleOr<HWND> parent, int id);

HWND createStatic(HandleOr<HWND> parent, int id, unsigned long styles = 0);
HWND createButton(HandleOr<HWND> parent, int id, unsigned long styles = 0);
HWND createEdit(HandleOr<HWND> parent, int id, unsigned long styles = 0);
HWND createCheckbox(HandleOr<HWND> parent, int id, unsigned long styles = 0);

} // end namespace curt

#endif
