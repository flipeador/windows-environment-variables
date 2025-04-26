#include "../framework.h"

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    ((std::vector<HWND>*)(lParam))->push_back(hWnd);
    return TRUE;
}

HWND GetWindowRoot(HWND hWnd)
{
    return GetAncestor(hWnd, GA_ROOT);
}

HWND GetWindowOwner(HWND hWnd)
{
    return GetWindow(hWnd, GW_OWNER);
}

HWND GetWindowParent(HWND hWnd)
{
    return GetAncestor(hWnd, GA_PARENT);
}

HFONT GetWindowFont(HWND hWnd)
{
    return (HFONT)SendMessageW(hWnd, WM_GETFONT, NULL, NULL);
}

VOID SetWindowFont(HWND hWnd, HFONT hFont)
{
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}

WSTR GetWindowText(HWND hWnd)
{
    std::wstring text;
    text.resize_and_overwrite(
        GetWindowTextLengthW(hWnd),
        [hWnd](wchar_t* ptr, size_t count) -> size_t {
            auto size = static_cast<INT>(count);
            return GetWindowTextW(hWnd, ptr, size);
        }
    );
    return text;
}

WSTR GetWindowClass(HWND hWnd)
{
    std::wstring name;
    name.resize_and_overwrite(256,
        [hWnd](wchar_t* ptr, size_t count) -> size_t {
            auto size = static_cast<INT>(count);
            return GetClassNameW(hWnd, ptr, size);
        }
    );
    return name;
}

UINT GetWindowStyle(HWND hWnd)
{
    return static_cast<UINT>(GetWindowLongPtrW(hWnd, GWL_STYLE));
}

UINT GetWindowExStyle(HWND hWnd)
{
    return static_cast<UINT>(GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}

UINT GetWindowThreadId(HWND hWnd)
{
    return GetWindowThreadProcessId(hWnd, nullptr);
}

UINT GetWindowProcessId(HWND hWnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid;
}

UINT GetWindowRect(HWND hWnd, HWND hWndMapTo, Rect& rc)
{
    RECT rc2;
    if (!GetWindowRect(hWnd, &rc2)
        || !MapWindowRect(HWND_DESKTOP, hWndMapTo, rc2))
        return FALSE;
    return rc.Set(rc2, GetDpiForWindow(hWnd));
}

UINT GetClientRect(HWND hWnd, HWND hWndMapTo, Rect& rc)
{
    RECT rc2;
    if (!GetClientRect(hWnd, &rc2)
        || !MapWindowRect(hWnd, hWndMapTo, rc2))
        return FALSE;
    return rc.Set(rc2, GetDpiForWindow(hWnd));
}

BOOL MapWindowRect(HWND hWndFrom, HWND hWndTo, RECT& rc)
{
    if (hWndFrom == hWndTo)
        return TRUE;
    return MapWindowPoints(hWndFrom, hWndTo, (POINT*)&rc, 2);
}

VOID AdjustWindowRect(HWND hWnd, RECT& rc)
{
    UINT style = GetWindowStyle(hWnd);
    UINT exStyle = GetWindowStyle(hWnd);
    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
}

BOOL MoveWindow(HWND hWnd, const RECT& rc, BOOL update)
{
    return MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, update);
}

BOOL MoveWindow(HWND hWnd, const Rect& rc, BOOL update)
{
    RECT rc2;
    rc.Get(rc2, GetDpiForWindow(hWnd));
    return MoveWindow(hWnd, rc2, update);
}

BOOL MoveWindow(HWND hWnd, const Size& si, BOOL update)
{
    SIZE si2;
    si.Get(si2, GetDpiForWindow(hWnd));
    return SetWindowPos(hWnd, NULL, 0, 0, si2.cx, si2.cy, update ? 534 : 542);
}

BOOL MoveWindow(HWND hWnd, const Point& pt, BOOL update)
{
    POINT pt2;
    pt.Get(pt2, GetDpiForWindow(hWnd));
    return SetWindowPos(hWnd, NULL, pt2.x, pt2.y, 0, 0, update ? 533 : 541);
}

BOOL PeekMessage(PMSG pMsg, HWND hWnd, UINT min, UINT max, UINT flags)
{
    if (pMsg != nullptr)
        return PeekMessageW(pMsg, hWnd, min, max, flags);
    MSG msg;
    return PeekMessageW(&msg, hWnd, min, max, flags);
}

VOID ActivateWindow(HWND hWnd)
{
    HWND hWndRoot = GetWindowRoot(hWnd);
    if (IsIconic(hWndRoot))
        ShowWindow(hWndRoot, SW_RESTORE);
    SetForegroundWindow(hWndRoot);
    SendMessageW(hWndRoot, WM_NULL, NULL, NULL);
    HWND hWndFore = GetForegroundWindow();
    if (hWndRoot == hWndFore
        || hWndRoot == GetWindowOwner(hWndFore))
        return;
    WINDOWPLACEMENT wp { sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(hWndRoot, &wp);
    ShowWindow(hWndRoot, SW_HIDE);
    ShowWindow(hWndRoot, SW_MINIMIZE);
    SetWindowPlacement(hWndRoot, &wp);
    SetForegroundWindow(hWndRoot);
}

std::vector<HWND> EnumChildWindows(HWND hWnd)
{
    if (hWnd == nullptr)
        return { };
    std::vector<HWND> vec;
    EnumChildWindows(hWnd, EnumWindowsProc, (LPARAM)&vec);
    return vec;
}

std::vector<HWND> EnumThreadWindows(UINT tid)
{
    std::vector<HWND> vec;
    EnumThreadWindows(tid, EnumWindowsProc, (LPARAM)&vec);
    return vec;
}

BOOL IsMainWindow(HWND hWnd)
{
    return IsTopLevelWindow(hWnd) && !GetWindowOwner(hWnd);
}

BOOL IsTopLevelWindow(HWND hWnd)
{
    HWND hWndRoot = GetWindowRoot(hWnd);
    return hWndRoot != nullptr && hWnd == hWndRoot;
}

BOOL IsCurrentThreadWindow(HWND hWnd)
{
    UINT hWndThreadId = GetWindowThreadId(hWnd);
    return hWndThreadId && hWndThreadId == GetCurrentThreadId();
}

WindowRef::WindowRef(HWND hWnd)
    : m_phWnd(new HWND(hWnd))
{
    m_id = std::format(L"{:p}", (PVOID)m_phWnd);
    SetPropW(hWnd, m_id.data(), (HANDLE)TRUE);
}

WindowRef::WindowRef(WindowRef&& other) noexcept
    : m_id(std::move(other.m_id))
    , m_phWnd(std::exchange(other.m_phWnd, nullptr))
{ }

WindowRef::~WindowRef()
{
    RemovePropW(*m_phWnd, m_id.data());
    delete m_phWnd;
}

BOOL WindowRef::Exists()
{
    return GetPropW(*m_phWnd, m_id.data()) != 0;
}

WindowRef& WindowRef::operator=(WindowRef&& other) noexcept
{
    if (this != &other)
    {
        delete m_phWnd;
        m_id = std::move(other.m_id);
        m_phWnd = std::exchange(other.m_phWnd, nullptr);
    }
    return *this;
}
