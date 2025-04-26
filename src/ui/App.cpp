#include "framework.h"

std::optional<LRESULT> DispatchMessage(MSG& msg)
{
    if (msg.time)
    {
        HWND hWndRoot = GetWindowRoot(msg.hwnd);
        if (!IsDialogMessageW(hWndRoot, &msg))
        {
            TranslateMessage(&msg);
            return DispatchMessageW(&msg);
        }
    }

    return std::nullopt;
}

BOOL GetMessage(MSG& msg, HWND hWnd, UINT min, UINT max)
{
    BOOL result = GetMessageW(&msg, hWnd, min, max);
    if (result == -1)
        msg.time = 0;
    return result;
}

BOOL MsgWaitFor(HWND hWnd, std::function<BOOL(MSG&)> fn)
{
    if (hWnd && GetWindowThreadId(hWnd) != GetCurrentThreadId())
        throw std::runtime_error("The window belongs to a different thread.");

    MSG msg { };
    WindowRef window(hWnd);

    while (!fn(msg))
    {
        // If `hWnd` is not `nullptr`, ensure the [exact same] window still exists.
        // In case the window was destroyed outside this loop (i.e. in the `fn` function).
        // `IsWindow` would give a false positive if a new window reuses the same handle.
        // Although unlikely (https://devblogs.microsoft.com/oldnewthing/20070717-00/?p=25983).
        if (hWnd && !window.Exists())
            return FALSE;

        if (!GetMessage(msg))
        {
            // Repost `WM_QUIT` because `MsgWaitFor` is not meant to handle the main loop.
            PostQuitMessage((INT)msg.wParam);
            return FALSE;
        }

        DispatchMessage(msg);

        if (msg.hwnd == hWnd && msg.message == WM_DESTROY)
            return FALSE;
    }

    return TRUE;
}

App::App()
{
    // Set the process-default DPI awareness via application manifest instead.
    //SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!AreDpiAwarenessContextsEqual(GetThreadDpiAwarenessContext(), DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
        throw std::runtime_error("The current thread is not DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2.");

    BOOL info = FALSE;
    HANDLE hProcess = GetCurrentProcess();
    if (!SetUserObjectInformationW(hProcess, UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &info, sizeof(info)))
        throw WinError(L"SetUserObjectInformationW -> UOI_TIMERPROC_EXCEPTION_SUPPRESSION -> FALSE");
}

INT App::Run() const
{
    MSG msg;
    while (GetMessage(msg))
        DispatchMessage(msg);
    return (INT)msg.wParam;
}

ATOM App::Register(WSTRV className, BOOL singleInstance)
{
    if (singleInstance)
    {
        if (HWND hWnd = FindWindowW(className.data(), NULL))
        {
            ActivateWindow(hWnd);
            return 0;
        }
    }

    WNDCLASSW wc{ CS_DBLCLKS };
    wc.lpszClassName = className.data();
    wc.lpfnWndProc = Window::WndProcRouter;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    if (auto atom = RegisterClassW(&wc))
        return atom;

    throw WinError(L"RegisterClassW");
}
