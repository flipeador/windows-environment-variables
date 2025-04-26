#undef GetMessage
#undef DispatchMessage

std::optional<LRESULT> DispatchMessage(MSG& msg);
BOOL GetMessage(MSG& msg, HWND hWnd = NULL, UINT min = 0, UINT max = 0);
BOOL MsgWaitFor(HWND hWnd, std::function<BOOL(MSG&)> fn);

class App
{
public:
    explicit App();
    virtual ~App() = default;
    
    INT Run() const;

    static ATOM Register(WSTRV className, BOOL singleInstance);
};
