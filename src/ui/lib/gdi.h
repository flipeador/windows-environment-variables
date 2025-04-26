#undef CreateFont

class DeviceContext final
{
public:
    explicit DeviceContext(HWND hWnd);
    ~DeviceContext();

    HDC Hdc() const noexcept;
    HWND Hwnd() const noexcept;
private:
    HWND m_hWnd = nullptr;
    HDC m_hDC = nullptr;
    INT m_state = 0;
};

BOOL GetTextSize(HDC hDC, WSTRV text, SIZE& si);
UINT GetTextSize(HWND hWnd, WSTRV text, Size& si);

HFONT CreateFont(WSTRV name, INT size = 0, INT weight = 0);
