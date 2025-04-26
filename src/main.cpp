#include "ui/framework.h"

WSTRV g_className = L"{0194F4F6-E085-7DB5-8FAE-5E20B6A073B8}";

WSTRV g_usrKey = L"Environment";
WSTRV g_usrVKey = L"Volatile Environment";
WSTRV g_sysKey = L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

#define USRKEY HKEY_CURRENT_USER, g_usrKey
#define USRVKEY HKEY_CURRENT_USER, g_usrVKey
#define SYSKEY HKEY_LOCAL_MACHINE, g_sysKey

VOID AddEnvVars(HKEY key, WSTRV subkey, TreeView& tv, WSTRV scope);
VOID AddProcessEnvVars(TreeView& tv, WSTRV scope);
std::generator<WSTRV> EnumEnvVarValues(WSTRV data);
std::generator<std::tuple<DWORD,WSTRV,PVOID,DWORD>> EnumRegValues(HKEY hKey);

class EditWindowWebView final : public WebView
{
public:
    explicit EditWindowWebView(HWND hWndParent, UINT style, UINT exStyle)
        : WebView(hWndParent, style, exStyle)
    { }

    VOID Set(WSTRV name, WSTRV value, BOOL bname, UINT regsz, UINT unique, UINT strict)
    {
        ExecuteScript(std::format(
            L"$name.value='{}';$value.value='{}';$regsz.checked={};$unique.checked={};$strict.checked={}",
            name, value, regsz & 1 ? L"1" : L"0", unique & 1 ? L"1" : L"0", strict & 1 ? L"1" : L"0"
        ));

        ExecuteScript(std::format(
            L"$name.disabled={};$regsz.disabled={};$strict.disabled={}",
            bname ? L"0" : L"1", regsz & 2 ? L"0" : L"1", strict & 2 ? L"0" : L"1"
        ));
    }
protected:
    VOID OnCreate() override
    {
        // "https://root.app" -> "./webview" (prevent CORS errors).
        //SetVirtualHostNameToFolderMapping(L"root.app", L"./webview");
        //Navigate(L"https://root.app/EditWindow.html");

        // Load the HTML file from an embedded resoure instead.
        NavigateHTML(LoadResource(L"WEBVIEW/EDITWINDOW_HTML"));

        // Wait until the page is ready.
        MsgWaitFor(Hwnd(), [this] (MSG&) { return m_ready; });
    }

    VOID OnWebMessage(CoTaskStr&, CoTaskStr& msg) override
    {
        // Exchange messages as STRING instead of JSON for simplicity.
        auto parts = StrSplit(msg, L"\t", INFINITE);

        if (parts[0] == L"Init")
        {
            auto pWndParent = GetParent();

            int width = wcstol(parts[1].data(), nullptr, 10);
            int height = wcstol(parts[2].data(), nullptr, 10);

            RECT rc { 0, 0, width, height };
            AdjustWindowRect(pWndParent->Hwnd(), rc);

            // Adjust window client area to fit page content.
            Size si(rc.right - rc.left, rc.bottom - rc.top);
            MoveWindow(pWndParent->Hwnd(), si);

            m_ready = TRUE;
        }
        else if (parts[0] == L"Save")
        {
            Close();
        }
    }
private:
    BOOL m_ready = FALSE;
};

class EditWindow final : public Window
{
public:
    explicit EditWindow(HWND hWndParent, UINT styles, UINT exStyles)
        : Window(hWndParent, styles, exStyles)
    { }
protected:
    VOID OnCreate() override
    {
        GetOwner()->Disable();
        Window::Create<EditWindowWebView>(Hwnd(), WS_VISIBLE);
    }

    VOID OnClose() override
    {
        Show(SW_HIDE);
        GetOwner()->Activate();
    }

    VOID OnSize(const Size& si) override
    {
        MoveWindow(this->GetChild(0)->Hwnd(), si, TRUE);
    }

    VOID OnVisibilityChanged(BOOL visible) override
    {
        if (visible)
        {
            GetOwner()->Disable();

            Rect rc;
            GetClientRect(GetOwner()->Hwnd(), NULL, rc);
            MoveWindow(Hwnd(), Point(rc.x + 10, rc.y + 10));
        }
    }
};

class MainWindow final : public Window
{
public:
    MainWindow()
        : Window(g_className, NULL, WS_OVERLAPPEDWINDOW, 0)
    {
        SetText(L"Environment Variables Editor"); // title
    }
protected:
    VOID OnCreate() override
    {
        auto& tv = Window::Create<TreeView>(Hwnd(),
            WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | TVS_SHOWSELALWAYS |
            TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS,
            WS_EX_CLIENTEDGE
        );

        tv.SetFont(L"Consolas");
        tv.SetBkColor(0xD1EDED);

        AddEnvVars(USRKEY, tv, L"User");
        AddEnvVars(SYSKEY, tv, L"System");
        AddEnvVars(USRVKEY, tv, L"User (volatile)");
        AddProcessEnvVars(tv, L"Process");

        Show();
    }

    VOID OnSize(const Size& si) override
    {
        MoveWindow(this->GetChild(0)->Hwnd(), si, TRUE);
    }

    VOID OnContextMenu(HWND hWnd, INT x, INT y) override
    {
        if (hWnd == Hwnd()) return; // non-client area

        auto& tv = (TreeView&)*this->GetChild(0);

        auto selectedItems = tv.GetSelectedItems();
        if (selectedItems.size() != 1) return;

        auto& selectionTree = selectedItems[0];
        if (!selectionTree.size() || selectionTree[0].text == L"Process")
            return;

        auto& selectedItem = selectionTree.back();
        auto hItemPrev = tv.GetPrev(selectedItem.hItem);
        auto hItemNext = tv.GetNext(selectedItem.hItem);

        Menu menu;

        if (selectionTree.size() == 1)
        {
            menu.Add(L"New variable");
        }
        else if (selectionTree.size() == 2)
        {
            menu.Add(L"New value");
            menu.Add(/* SEPARATOR */);
            menu.Add(L"Edit variable");
            menu.Add(L"Delete variable");
        }
        else if (selectionTree.size() == 3)
        {
            menu.Add(L"New value");
            menu.Add(L"Edit value");
            menu.Add(L"Delete value");
            menu.Add(/* SEPARATOR */);
            menu.Add(L"Move up", hItemPrev ? 0 : MF_DISABLED);
            menu.Add(L"Move down", hItemNext ? 0 : MF_DISABLED);
        }
 
        auto text = menu.GetText(menu.Show(Hwnd(), x, y));

        if (text.empty())
            return;

        if (m_pEditWindow == nullptr)
            m_pEditWindow = &Window::Create<EditWindow>(Hwnd(), WS_CAPTION | WS_SYSMENU);

        m_pEditWindow->WaitForInit();
        m_pEditWindow->SetText(text);
        m_pEditWindow->Show();

        auto pWebView = (EditWindowWebView*)m_pEditWindow->GetChild(0);

        if (text == L"New variable")
            pWebView->Set(L"", L"", TRUE, 2, 2, 3);
        else if (text == L"New value")
        {
            auto regsz = selectionTree[1].lParam == REG_EXPAND_SZ ? 1 : 0;
            pWebView->Set(selectionTree[1].text, L"", FALSE, regsz, 3, 0);
        }
    }
private:
    EditWindow* m_pEditWindow = nullptr;
};

INT CALLBACK wWinMain(HINSTANCE, HINSTANCE, PWSTR, INT)
{
    if (IsDebuggerPresent())
        InitConsole();

    if (!App::Register(g_className, true))
        return NO_ERROR;

    App app;
    MainWindow wnd;

    return app.Run();
}

VOID AddEnvVars(HKEY key, WSTRV subkey, TreeView& tv, WSTRV scope)
{
    HANDLE hItem1 = tv.Add(NULL, TVI_LAST, scope, TVIS_BOLD | TVIS_EXPANDED);

    HKEY hKey = nullptr;
    RegOpenKeyExW(key, subkey.data(), 0, KEY_READ, &hKey);

    if (hKey == nullptr)
        throw std::runtime_error("Failed to open reg key");

    for (auto& [type, name, data, size] : EnumRegValues(hKey))
    {
        if (type != REG_SZ && type != REG_EXPAND_SZ)
            continue;

        HANDLE hItem2 = tv.Add(hItem1, TVI_SORT, name, 0, type);

        for (auto& value : EnumEnvVarValues((PCWSTR)data))
            tv.Add(hItem2, TVI_LAST, value);
    }

    RegCloseKey(hKey);
}

VOID AddProcessEnvVars(TreeView& tv, WSTRV scope)
{
    HANDLE hItem1 = tv.Add(NULL, TVI_LAST, scope, TVIS_BOLD | TVIS_EXPANDED);

    if (wchar_t* pEnv = GetEnvironmentStringsW())
    {
        const wchar_t* pVar = pEnv;
        
        while (*pVar != '\0')
        {
            auto var = StrSplit(pVar, L"=", 3);
            if (var.size() == 2)
            {
                WSTR text(var[0]); // copy wstring_view substr
                HANDLE hItem2 = tv.Add(hItem1, TVI_SORT, text);
                for (auto& value : EnumEnvVarValues(var[1]))
                    tv.Add(hItem2, TVI_LAST, value);
            }
            pVar += wcslen(pVar) + 1;
        }

        FreeEnvironmentStringsW(pEnv);
    }
}

std::generator<WSTRV> EnumEnvVarValues(WSTRV data)
{
    std::wstring str;
    str.reserve(MAX_PATH);

    bool quote = false;
    auto ptr = data.data();
    
    for (wchar_t chr; chr = *ptr; ++ptr)
    {
        if (chr == '"')
        {
            quote = !quote;
            continue;
        }
        if (!quote && chr == ';')
        {
            co_yield str;
            str.clear();
        }
        else str += chr;
    }

    if (!str.empty())
        co_yield str;
}

std::generator<std::tuple<DWORD,WSTRV,PVOID,DWORD>> EnumRegValues(HKEY hKey)
{
    DWORD index = 0;
    std::wstring name;
    std::vector<BYTE> data { '\0' };
    
    for (;;)
    {
        DWORD type = REG_NONE;
        DWORD error = NO_ERROR;
        name.resize_and_overwrite(MAX_PATH,
            [hKey, &index, &type, &error](wchar_t* ptr, size_t count) -> size_t {
                DWORD size = static_cast<DWORD>(count);
                error = RegEnumValueW(hKey, index++, ptr, &size, nullptr, &type, nullptr, nullptr);
                return error == NO_ERROR ? size : 0;
            }
        );
        
        if (error == ERROR_NO_MORE_ITEMS) break;
        if (error != NO_ERROR) continue;

        auto size = static_cast<DWORD>(data.capacity());
        error = RegGetValueW(hKey, nullptr, name.data(), RRF_RT_ANY | RRF_NOEXPAND, &type, data.data(), &size);

        if (error == ERROR_MORE_DATA)
        {
            data.reserve(size);
            error = RegGetValueW(hKey, nullptr, name.data(), RRF_RT_ANY | RRF_NOEXPAND, &type, data.data(), &size);
        }

        if (error != NO_ERROR)
            continue;

        co_yield { type, name, data.data(), size };
    }
}
