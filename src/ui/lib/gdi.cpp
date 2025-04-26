#include "../framework.h"

DeviceContext::DeviceContext(HWND hWnd)
    : m_hWnd(hWnd)
{
    m_hDC = GetDC(hWnd);
    if (m_hDC != nullptr)
        m_state = SaveDC(m_hDC);
}

DeviceContext::~DeviceContext()
{
    RestoreDC(m_hDC, m_state);
    ReleaseDC(m_hWnd, m_hDC);
}

HDC DeviceContext::Hdc() const noexcept
{
    return m_hDC;
}

HWND DeviceContext::Hwnd() const noexcept
{
    return m_hWnd;
}

BOOL GetTextSize(HDC hDC, WSTRV text, SIZE& si)
{
    return GetTextExtentPoint32W(hDC, text.data(), (INT)text.size(), &si);
}

UINT GetTextSize(HWND hWnd, WSTRV text, Size& si)
{
    SIZE size;
    DeviceContext dc(hWnd);
    if (dc.Hdc() == nullptr)
        return FALSE;
    if (auto hFont = GetWindowFont(hWnd))
        SelectObject(dc.Hdc(), hFont);
    if (!GetTextSize(dc.Hdc(), text, size))
        return FALSE;
    return si.Set(size, GetDpiForWindow(hWnd));
}

HFONT CreateFont(WSTRV name, INT size, INT weight)
{
    return CreateFontW(size, 0, 0, 0, weight, 0, 0, 0, 0,
        0, 0, CLEARTYPE_QUALITY, FF_DONTCARE, name.data());
}
