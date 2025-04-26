#include "../framework.h"

static HWND g_hWndCon = NULL;
static HANDLE g_hStdIn = NULL;
static HANDLE g_hStdOut = NULL;

static BOOL CheckConsole()
{
    if (IsWindowVisible(g_hWndCon))
        return TRUE;
    return ShowConsole(SW_SHOWNA);
}

VOID InitConsole()
{
    AllocConsole();

    g_hWndCon = GetConsoleWindow();
    ShowConsole(SW_HIDE);

    SetConsoleCP(UTF8);
    SetConsoleOutputCP(UTF8);

    g_hStdIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    g_hStdOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    DWORD hStdInMode, hStdOutMode;
    GetConsoleMode(g_hStdIn, &hStdInMode);
    GetConsoleMode(g_hStdOut, &hStdOutMode);

    // https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
    SetConsoleMode(g_hStdIn, hStdInMode | ENABLE_VIRTUAL_TERMINAL_INPUT);
    SetConsoleMode(g_hStdOut, hStdOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN);
}

BOOL ShowConsole(INT cmd)
{
    return ShowWindow(g_hWndCon, cmd);
}

WSTR ReadConsole(size_t count)
{
    CheckConsole();

    WSTR text;
    text.resize_and_overwrite(count,
        [](wchar_t* ptr, size_t count) -> size_t {
            DWORD length = 0;
            ReadConsoleW(g_hStdIn, ptr, (DWORD)count, &length, NULL);
            return length ? length - 2 : length; // trim trailing "\r\n"
        }
    );

    return std::move(text);
}

BOOL WriteConsole(WSTRV text)
{
    CheckConsole();

    return WriteConsoleW(g_hStdOut, text.data(), (UINT)text.length(), NULL, NULL);
}

BOOL Print(WSTRV text)
{
    return WriteConsole(std::format(L"{}\r\n", text));
}

WSTR Input(WSTRV text)
{
    if (!WriteConsole(text)) return { };
    return ReadConsole(DEFAULT_CONSOLE_PRINT_COUNT);
}
