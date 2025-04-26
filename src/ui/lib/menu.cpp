#include "../framework.h"

Menu::Menu(UINT style, UINT maxHeight)
{
    m_hMenu = CreatePopupMenu();

    MENUINFO mi{ sizeof(MENUINFO) };
    mi.fMask = MIM_MENUDATA | MIM_STYLE | MIM_MAXHEIGHT;
    mi.dwStyle = style;
    mi.cyMax = maxHeight;
    mi.dwMenuData = reinterpret_cast<ULONG_PTR>(this);
    SetMenuInfo(m_hMenu, &mi);
}

VOID Menu::Add()
{
    AppendMenuW(m_hMenu, MF_SEPARATOR, NULL, nullptr);
}

UINT Menu::Add(WSTRV name, UINT flags)
{
    UINT id = ComputeCrc32(name, flags);
    AppendMenuW(m_hMenu, MF_STRING | flags, id, name.data());
    return id;
}

UINT Menu::Show(HWND hWnd, INT x, INT y)
{
    return TrackPopupMenuEx(m_hMenu, TPM_NONOTIFY | TPM_RETURNCMD, x, y, hWnd, nullptr);
}

WSTR Menu::GetText(UINT id)
{
    std::wstring text;
    text.resize_and_overwrite(260,
        [this, id](wchar_t* ptr, size_t count) -> size_t {
            INT length = static_cast<INT>(count);
            return GetMenuStringW(m_hMenu, id, ptr, length, MF_BYCOMMAND);
        }
    );
    return text;
}
