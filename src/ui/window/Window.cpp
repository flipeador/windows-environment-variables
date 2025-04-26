#include "../framework.h"

Window::Window(HWND hWndParentOrOwner, UINT styles, UINT exStyles)
    : Window(GetWindowClass(hWndParentOrOwner), hWndParentOrOwner, styles, exStyles)
{ }

Window::Window(WSTRV className, HWND hWndParentOrOwner, UINT styles, UINT exStyles)
{
    m_hWnd = CreateWindowExW(
        exStyles,               // The window extended styles.
        className.data(),       // The window class name or atom.
        nullptr,                // The window name displayed in the title bar.
        styles,                 // The window styles.
        CW_USEDEFAULT,          // The window x-coordinate, in device units.
        CW_USEDEFAULT,          // The window y-coordinate, in device units.
        CW_USEDEFAULT,          // The window width, in device units.
        CW_USEDEFAULT,          // The window height, in device units.
        hWndParentOrOwner,      // A handle to the parent or owner window.
        nullptr,                // A handle to a menu, or a child-window identifier.
        GetModuleHandleW(NULL), // The module to be associated with the window.
        this                    // A pointer value (CREATESTRUCT.lpCreateParams; WM_CREATE lParam).
    );

    if (m_hWnd == nullptr)
        throw WinError(L"CreateWindowExW");

    m_dpi = GetDpiForWindow(m_hWnd);

    SetWindowLongPtrW(m_hWnd, GWLP_ID, (LONG_PTR)m_hWnd);
    SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ChangeWindowMessageFilterEx(m_hWnd, WM_APPINFO, MSGFLT_ALLOW, nullptr);

    // Allows the window frame to be drawn in dark mode colors.
    // All windows default to light mode regardless of the system setting.
    BOOL allowDarkMode = TRUE; // Windows 11 Build B22000
    DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &allowDarkMode, sizeof(allowDarkMode));

    // Set the system-drawn backdrop material of the window.
    INT sysBackdropType = DWMSBT_TABBEDWINDOW; // Windows 11 Build 22621
    DwmSetWindowAttribute(m_hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &sysBackdropType, sizeof(sysBackdropType));

    SetPropW(m_hWnd, L"_Window", this);
    PostMessage(WM_APP_INITIALIZE, NULL, NULL);
}

Window::~Window()
{
    Destroy();
}

BOOL Window::Destroy()
{
    return DestroyWindow(m_hWnd);
}

BOOL Window::WaitForInit()
{
    if (MsgWaitFor(Hwnd(), [this] (MSG&) { return m_init > 1; }))
    {
        for (auto& pWnd : Children())
            if (!pWnd->WaitForInit())
                return FALSE;
        return TRUE;
    }
    return FALSE;
}

HWND Window::Hwnd()
{
    ASSERTDBG(GetInstance(m_hWnd) == this);
    return m_hWnd;
}

bool Window::IsVisible()
{
    return IsWindowVisible(Hwnd());
}

bool Window::IsEnabled()
{
    return IsWindowEnabled(Hwnd());
}

std::generator<Window*> Window::Children()
{
    for (auto& hWnd : EnumChildWindows(Hwnd()))
        if (auto pWindow = GetInstance(hWnd))
            co_yield pWindow;
}

INT Window::Dpi()
{
    return m_dpi;
}

VOID Window::Redraw()
{
    InvalidateRect(Hwnd(), nullptr, TRUE);
}

VOID Window::Enable()
{
    EnableWindow(Hwnd(), TRUE);
}

VOID Window::Disable()
{
    EnableWindow(Hwnd(), FALSE);
}

VOID Window::Activate()
{
    Enable();
    ActivateWindow(Hwnd());
}

VOID Window::Show(INT cmd)
{
    ShowWindow(Hwnd(), cmd);
}

VOID Window::Close()
{
    HWND hWndRoot = GetWindowRoot(Hwnd());
    SendMessageW(hWndRoot, WM_CLOSE, NULL, NULL);
}

Window* Window::GetOwner()
{
    return GetInstance(::GetWindowOwner(Hwnd()));
}

Window* Window::GetParent()
{
    return GetInstance(::GetWindowParent(Hwnd()));
}

Window* Window::GetChild(INT index)
{
    for (auto& pWindow : Children())
        if (--index < 0) return pWindow;
    return nullptr;
}

WSTR Window::GetText()
{
    return GetWindowText(Hwnd());
}

HANDLE Window::GetProp(WSTRV name)
{
    return GetPropW(Hwnd(), name.data());
}

BOOL Window::SetProp(WSTRV name, HANDLE data)
{
    return SetPropW(Hwnd(), name.data(), data);
}

BOOL Window::SetText(WSTRV text)
{
    return SetWindowTextW(Hwnd(), text.data());
}

VOID Window::SetResult(LRESULT result)
{
    m_msg.result = result;
    m_msg.processed = true;
}

BOOL Window::PeekMessage(PMSG pMsg, UINT min, UINT max, UINT flags)
{
    return ::PeekMessage(pMsg, Hwnd(), min, max, flags);
}

BOOL Window::PostMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return PostMessageW(Hwnd(), msg, wParam, lParam);
}

LRESULT Window::SendMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return SendMessageW(Hwnd(), msg, wParam, lParam);
}

VOID Window::OnCreate()
{ }

VOID Window::OnClose()
{
    // Prevent child windows from being destroyed.
    if (IsTopLevelWindow(Hwnd()))
        this->SetResult(Destroy());
}

VOID Window::OnDestroy()
{
    // Quit the app if the main window is destroyed.
    if (IsMainWindow(m_hWnd))
        this->SetResult(0);
}

VOID Window::OnNCDestroy()
{
    bool _quit = GetProp(L"_quit");
    bool _quitting = GetProp(L"_quitting");

    if (_quit && !_quitting)
    {
        for (HWND hWnd : EnumThreadWindows(GetCurrentThreadId()))
        {
            SetPropW(hWnd, L"_quitting", (HANDLE)TRUE);
            DestroyWindow(hWnd);
        }

        #pragma warning(suppress: 4311 4302)
        PostQuitMessage((INT)GetProp(L"_code"));
    }

    SetMenu(m_hWnd, nullptr);
    SetWindowLongPtrW(m_hWnd, GWLP_USERDATA, NULL);

    HWND hWnd = std::exchange(m_hWnd, nullptr);

    if (GetPropW(hWnd, L"_delete"))
        delete this;
}

VOID Window::OnDpiChanged()
{ }

VOID Window::OnSize(const Size& si)
{ }

VOID Window::OnVisibilityChanged(BOOL visible)
{ }

VOID Window::OnContextMenu(HWND hWnd, INT x, INT y)
{ }

VOID Window::OnMessage()
{ }

Window* Window::GetInstance(HWND hWnd, WSTRV name)
{
    if (hWnd == nullptr || name.size() == 0)
        return nullptr;
    if (GetWindowProcessId(hWnd) != GetCurrentProcessId())
        return nullptr;
    HANDLE pWnd = GetPropW(hWnd, name.data());
    return reinterpret_cast<Window*>(pWnd);
}

LRESULT Window::WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return WndProcRouter(::DefWindowProcW, hWnd, msg, wParam, lParam);
}

LRESULT Window::WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
    return WndProcRouter(::DefSubclassProc, hWnd, msg, wParam, lParam);
}

LRESULT Window::WndProcRouter(WNDPROC defWndProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* pWnd = nullptr;

    // Sent prior to the WM_CREATE message when a window is first created.
    // Return TRUE to continue creation of the window (`CreateWindow(Ex)`).
    if (msg == WM_NCCREATE)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        pWnd = reinterpret_cast<Window*>(cs->lpCreateParams);
        if (pWnd == nullptr)
            return FALSE;
        pWnd->m_hWnd = hWnd;
        return TRUE;
    }

    // Get the `Window` instance associated with the window.
    pWnd = (Window*)(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    // For messages arriving before the `WM_NCCREATE` message.
    if (pWnd == nullptr || pWnd->m_hWnd == nullptr)
        return defWndProc(hWnd, msg, wParam, lParam);

    if (pWnd->m_init == 0)
    {
        if (msg == WM_APP_INITIALIZE)
        {
            ++pWnd->m_init;
            pWnd->OnCreate();
            pWnd->SendMessage(WM_APP_VISIBILITYCHANGED, FALSE, TRUE);
            ++pWnd->m_init;
            return FALSE;
        }
        return defWndProc(hWnd, msg, wParam, lParam);
    }

    Window& wnd = *pWnd;
    WindowMsg _msg(pWnd, msg, wParam, lParam);

    wnd.OnMessage();

    if (wnd.m_msg.processed)
        return wnd.m_msg.result;

    switch (msg)
    {
    case WM_CLOSE:
        wnd.OnClose();
        return wnd.m_msg.result;
    case WM_DESTROY:
    {
        wnd.OnDestroy();
        if (!wnd.GetProp(L"_quit") && wnd.m_msg.processed)
        {
            wnd.SetProp(L"_quit", (HANDLE)TRUE);
            wnd.SetProp(L"_code", (HANDLE)wnd.m_msg.result);
        }
        break;
    }
    case WM_NCDESTROY:
        wnd.OnNCDestroy();
        break;
    case WM_DPICHANGED:
    {
        RECT& rc = *(RECT*)lParam;
        wnd.m_dpi = HIWORD(wParam);
        MoveWindow(wnd.Hwnd(), rc);
        wnd.OnDpiChanged();
        break;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
            wnd.OnSize(Size(LOWORD(lParam), HIWORD(lParam), wnd.m_dpi));
        break;
    }
    case WM_WINDOWPOSCHANGED:
        wnd.PostMessage(WM_APP_VISIBILITYCHANGED, FALSE, TRUE);
        break;
    case WM_CONTEXTMENU:
        wnd.OnContextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_APP_VISIBILITYCHANGED:
    {
        // Workaround for the `WM_SHOWWINDOW` message not being sent (hidden → visible).
        // The `WM_SHOWWINDOW` message is not sent when showing a minimized/maximized window.

        // If `wParam` is TRUE, `lParam` is the visibility state of the current window.
        // If `wParam` is FALSE, `lParam` is the visibility state of the parent window.
        bool visible = wParam ? !!lParam : lParam ? wnd.IsVisible() : false;

        if (visible != wnd.m_visible)
        {
            wnd.m_visible = visible;
            // Recursively inform all child windows of the change.
            for (auto& pWindow : wnd.Children())
                if (pWindow->m_init > 0)
                    pWindow->PostMessage(msg, FALSE, visible);
            wnd.OnVisibilityChanged(visible);
        }
        break;
    }
    }
    
    if (!wnd.m_msg.processed)
        return defWndProc(hWnd, msg, wParam, lParam);

    return wnd.m_msg.result;
}
