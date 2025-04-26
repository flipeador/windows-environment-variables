class Menu final
{
public:
    explicit Menu(UINT style = 0, UINT maxHeight = 0);
    ~Menu() = default;

    VOID Add();
    UINT Add(WSTRV name, UINT flags = 0);
    UINT Show(HWND hWnd, INT x, INT y);

    WSTR GetText(UINT id);
private:
    HMENU m_hMenu = nullptr;
};
