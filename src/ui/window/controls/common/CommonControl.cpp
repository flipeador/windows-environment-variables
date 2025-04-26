#include "../../../framework.h"

CommonControl::CommonControl(WSTRV className, HWND hWndParent, UINT style, UINT exStyle)
    : Window(className, hWndParent, style | WS_CHILD, exStyle)
{
    SetWindowSubclass(Hwnd(), WndProcRouter, (UINT_PTR)this, NULL);

    SetWindowTheme(Hwnd(), L"Explorer", nullptr);

    SetProp(L"_CommonControl", this);
}

CommonControl::~CommonControl()
{
    DeleteObject(m_font.hFont);
}

HFONT CommonControl::SetFont(WSTRV name, INT size, INT weight)
{
    DeleteObject(m_font.hFont);

    auto height = static_cast<INT>(DpiScale(size, Dpi()));
    m_font = { CreateFont(name, -height, weight), size };

    if (m_font.hFont != nullptr)
        SetWindowFont(Hwnd(), m_font.hFont);

    return m_font.hFont;
}

CommonControl* CommonControl::GetInstance(HWND hWnd)
{
    return (CommonControl*)Window::GetInstance(hWnd, L"_CommonControl");
}

VOID CommonControl::OnDpiChanged()
{
    Window::OnDpiChanged();

    if (m_font.hFont != nullptr)
    {
        LOGFONTW lf { };

        if (GetObjectW(m_font.hFont, sizeof(decltype(lf)), &lf))
        {
            lf.lfHeight = static_cast<LONG>(DpiScale(m_font.size, Dpi()));

            if (HFONT hFont = CreateFontIndirectW(&lf))
            {
                DeleteObject(m_font.hFont);
                SetWindowFont(Hwnd(), m_font.hFont = hFont);
            }
        }
    }
}
