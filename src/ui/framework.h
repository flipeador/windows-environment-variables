#pragma once

// Use `ComCtl32.dll` v6 to enable visual styles and `TaskDialog`.
//#pragma comment(linker, "\"/manifestdependency:type='win32' \
//    name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
//    processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "ComCtl32.lib")

#pragma warning(disable :  4100) // Unreferenced formal parameter.
#pragma warning(disable :  4706) // Assignment used as a condition.
#pragma warning(disable :  4458) // Declaration of 'X' hides class member.
#pragma warning(disable : 28251) // Inconsistent annotation for function.

#define _WIN32_WINNT  _WIN32_WINNT_WIN10 // Windows 10
#define NTDDI_VERSION NTDDI_WIN10 // Windows 10 1507 "Threshold"

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

// https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
#define UTF8        65001
#define UTF16LE_BOM 0xFEFF

#define LONG_MAXPATH UNICODE_STRING_MAX_CHARS // 32767

#define NOMINMAX
#define OEMRESOURCE
#define NOVIRTUALKEYCODES

#include <windows.h>
#undef   GetObject
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <shellapi.h>
#include <processthreadsapi.h>
#include <synchapi.h>
#include <objidl.h>

#undef NEAR
#undef near
#undef FAR
#undef far
#undef VOID

#ifdef _DEBUG
#define ASSERT(expr) ((expr) ? (VOID)0 : __debugbreak())
#define ASSERTDBG(expr) ASSERT(expr)
#else
#define ASSERT(expr) (expr)
#define ASSERTDBG(expr) ((VOID)0)
#endif

#include <map>
#include <list>
#include <vector>
#include <string>
#include <format>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <experimental/generator>

namespace std {
    template <typename T>
    using generator = experimental::generator<T>;
}

typedef std::wstring WSTR;
typedef std::wstring_view WSTRV;

typedef void VOID;

#include "lib/string.h"
#include "lib/error.h"
#include "lib/console.h"
#include "lib/util.h"
#include "lib/gdi.h"
#include "lib/com.h"
#include "lib/windows.h"
#include "lib/menu.h"

#include "window/Window.h"
#include "window/controls/WebView.h"
#include "window/controls/common/CommonControl.h"
#include "window/controls/common/TreeView.h"

#include "../resources/resources.h"

#include "App.h"
