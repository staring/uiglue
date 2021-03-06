//===-- Definition of Win32 wrapper functions -----------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#include "util.h"

#include "curt.h"
#include "error.h"
#include "include_windows.h"

#include <limits>

using std::string;
using std::wstring;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace curt {

HINSTANCE thisModule() {
  return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

string wideToUtf8(wstring wide) {
  if (wide.empty())
    return {};

  const auto maxSize = std::numeric_limits<int>::max();
  if (wide.size() > static_cast<wstring::size_type>(maxSize))
    throw std::runtime_error("String too long to convert to UTF-8");

  const auto nChar = static_cast<int>(wide.size());
#if (WINVER >= 0x0600)
  const auto flags = WC_ERR_INVALID_CHARS;
#else
  const auto flags = 0;
#endif

  const auto nBytes = wideCharToMultiByte(
      CP_UTF8, flags, wide.data(), nChar, nullptr, 0, nullptr, nullptr);

  auto narrow = string(nBytes, '\0');
  wideCharToMultiByte(
      CP_UTF8, flags, wide.data(), nChar, &narrow[0], nBytes, nullptr, nullptr);

  return narrow;
}

wstring utf8ToWide(string utf8) {
  if (utf8.empty())
    return {};

  const auto maxSize = std::numeric_limits<int>::max();
  if (utf8.size() > static_cast<string::size_type>(maxSize))
    throw std::runtime_error("String too long to convert to wide chars");

  const auto nBytes = static_cast<int>(utf8.size());
  const auto flags = MB_ERR_INVALID_CHARS;

  const auto nChar
      = multiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, nullptr, 0);

  auto wide = std::wstring(nChar, '\0');
  multiByteToWideChar(CP_UTF8, flags, utf8.data(), nBytes, &wide[0], nChar);

  return wide;
}

wstring loadStringW(unsigned int resId) {
  wchar_t* buff;
  auto asCharPtr = reinterpret_cast<wchar_t*>(&buff);
  auto len = curt::loadString(thisModule(), resId, asCharPtr, 0);
  return wstring(buff, len);
}

string loadString(unsigned int resId) {
  return wideToUtf8(loadStringW(resId));
}

string getWindowTextString(HandleOr<HWND> wnd) {
  auto wide = getWindowTextStringW(wnd);
  return curt::wideToUtf8(std::move(wide));
}

wstring getWindowTextStringW(HandleOr<HWND> wnd) {
  auto len = curt::getWindowTextLength(wnd);
  auto wide = std::wstring(len + 1, 0);
  curt::getWindowText(wnd, &wide[0], len + 1);
  return wide;
}

WPARAM pumpMessages() {
  MSG msg;
  while (getMessage(&msg, nullptr, 0, 0)) {
    translateMessage(&msg);
    dispatchMessage(&msg);
  }

  return msg.wParam;
}

WPARAM pumpMessages(HandleOr<HWND> translateWnd, HACCEL accelTable) {
  MSG msg;
  while (getMessage(&msg, nullptr, 0, 0)) {
    if (!translateAccelerator(translateWnd, accelTable, &msg)) {
      translateMessage(&msg);
      dispatchMessage(&msg);
    }
  }

  return msg.wParam;
}

static LRESULT CALLBACK
ctrlBgProc(HWND wnd, UINT msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR color) {
  try {
    switch (msg) {
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC: {
      auto hdc = reinterpret_cast<HDC>(w);
      setDCBrushColor(hdc, static_cast<unsigned long>(color));
      return reinterpret_cast<LRESULT>(getStockObject(DC_BRUSH));
    }
    default:
      return defSubclassProc(wnd, msg, w, l);
    }
  }
  catch (...) {
    saveCurrentException();
    return 0;
  }
}

void subclassControlBackground(HandleOr<HWND> wnd, unsigned long rgb) {
  setWindowSubclass(wnd, ctrlBgProc, 0, rgb);
}

static LRESULT CALLBACK
appViewProc(HWND wnd, UINT msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR) {
  try {
    if (msg == WM_DESTROY)
      PostQuitMessage(0);

    return defSubclassProc(wnd, msg, w, l);
  }
  catch (...) {
    saveCurrentException();
    return 0;
  }
}

void subclassAppView(HandleOr<HWND> wnd) {
  setWindowSubclass(wnd, appViewProc, 0, 0);
}

Font defaultFont() {
  auto metrics = NONCLIENTMETRICSW{ sizeof(NONCLIENTMETRICSW) };
  systemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
  return createFontIndirect(&metrics.lfMessageFont);
}

void setControlToDefaultFont(HandleOr<HWND> parent, int id) {
  static auto font = defaultFont();
  auto asWParam = reinterpret_cast<WPARAM>(font.get());
  curt::sendDlgItemMessage(parent, id, WM_SETFONT, asWParam, 1);
}

static HWND
createCtrl(LPCWSTR className, unsigned long style, HWND parent, int id) {
  auto x = CW_USEDEFAULT; // All dimensions are default
  auto menuOrId = reinterpret_cast<HMENU>(id);
  auto control
      = curt::createWindowEx(0,
                             className,
                             nullptr,
                             WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | style,
                             x,
                             x,
                             x,
                             x,
                             parent,
                             menuOrId,
                             curt::thisModule(),
                             nullptr);

  return control.release();
}

HWND createStatic(HandleOr<HWND> parent, int id, unsigned long styles) {
  return createCtrl(L"STATIC", styles, parent, id);
}

HWND createButton(HandleOr<HWND> parent, int id, unsigned long styles) {
  return createCtrl(L"BUTTON", BS_PUSHBUTTON | styles, parent, id);
}

HWND createEdit(HandleOr<HWND> parent, int id, unsigned long styles) {
  const auto editStyles = ES_LEFT | ES_AUTOHSCROLL | WS_BORDER;
  return createCtrl(L"EDIT", editStyles | styles, parent, id);
}

HWND createCheckbox(HandleOr<HWND> parent, int id, unsigned long styles) {
  return createCtrl(L"BUTTON", BS_AUTOCHECKBOX | styles, parent, id);
}

} // end namespace curt
