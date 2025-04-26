#define WM_APPINFO WM_APP_MAX

class Window;

template <typename T>
concept DerivedFromWindow = std::derived_from<T, Window>;

struct WINDOW_MESSAGE
{
    HWND    wnd = nullptr;
    UINT    msg = WM_NULL;
    WPARAM  wParam = NULL;
    LPARAM  lParam = NULL;
    LRESULT result = NULL;
    bool processed = false;

    inline bool is(LRESULT value) const
    {
        return processed && result == value;
    }
};

class Window
{
public:
    explicit Window(HWND hWndParentOrOwner, UINT styles, UINT exStyles);
    explicit Window(WSTRV className, HWND hWndParentOrOwner, UINT styles, UINT exStyles);
    virtual ~Window();
   
    virtual VOID Redraw();
    virtual VOID Enable();
    virtual VOID Disable();
    virtual VOID Activate();
    virtual VOID Show(INT cmd = SW_NORMAL);
    virtual VOID Close();
    virtual BOOL Destroy();
    virtual BOOL WaitForInit();

    [[nodiscard]] virtual HWND Hwnd();
    [[nodiscard]] virtual INT Dpi();
    [[nodiscard]] virtual bool IsVisible();
    [[nodiscard]] virtual bool IsEnabled();

    [[nodiscard]] virtual Window* GetOwner();
    [[nodiscard]] virtual Window* GetParent();
    [[nodiscard]] virtual Window* GetChild(INT index);
    [[nodiscard]] virtual std::generator<Window*> Children();

    [[nodiscard]] virtual WSTR GetText();
    [[nodiscard]] virtual HANDLE GetProp(WSTRV name);

    virtual BOOL SetProp(WSTRV name, HANDLE data);
    virtual BOOL SetText(WSTRV text);
    virtual VOID SetResult(LRESULT result);

    virtual BOOL PeekMessage(PMSG pMsg, UINT min, UINT max, UINT flags);
    virtual BOOL PostMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT SendMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    template <DerivedFromWindow T>
    static T& Create(HWND hWndParentOrOwner = nullptr, UINT styles = 0, UINT exStyles = 0);
    static Window* GetInstance(HWND hWnd, WSTRV name = L"_Window");

    static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);
    static LRESULT CALLBACK WndProcRouter(WNDPROC defd, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
    BOOL m_init = 0;

    virtual VOID OnCreate();
    virtual VOID OnClose();
    virtual VOID OnDestroy();
    virtual VOID OnNCDestroy();
    virtual VOID OnDpiChanged();
    virtual VOID OnSize(const Size& si);
    virtual VOID OnVisibilityChanged(BOOL visible);
    virtual VOID OnContextMenu(HWND hWnd, INT X, INT y);
    virtual VOID OnMessage();
private:
    HWND m_hWnd = nullptr;
    WINDOW_MESSAGE m_msg;
    INT m_dpi = 0;
    bool m_visible = false;

    friend class WindowMsg;
};

class WindowMsg final
{
public:
    WindowMsg(Window* wnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        m_pWindow = wnd;
        m_msg = wnd->m_msg;

        wnd->m_msg.msg = msg;
        wnd->m_msg.wParam = wParam;
        wnd->m_msg.lParam = lParam;
        wnd->m_msg.result = LRESULT(0);
        wnd->m_msg.processed = false;
    }

    ~WindowMsg()
    {
        m_pWindow->m_msg = m_msg;
    }
private:
    Window* m_pWindow;
    WINDOW_MESSAGE m_msg;
};

template<DerivedFromWindow T>
inline T& Window::Create(HWND hWndParentOrOwner, UINT styles, UINT exStyles)
{
    Window* pWindow = new T(hWndParentOrOwner, styles, exStyles);
    pWindow->SetProp(L"_delete", (HANDLE)TRUE);
    return *static_cast<T*>(pWindow);
}
