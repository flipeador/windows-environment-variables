#include "WebView2.h"

class WebView;

template <DerivedFromIUnknown T>
class WebViewCallback abstract : public Unknown<T>
{
public:
    explicit WebViewCallback(WebView* pWebView)
        : m_pWebView(pWebView)
    { }
    virtual ~WebViewCallback() = default;
protected:
    WebView* m_pWebView;
};

class WebViewMessageEventHandler final
    : public WebViewCallback<ICoreWebView2WebMessageReceivedEventHandler>
{
public:
    using WebViewCallback::WebViewCallback;
    virtual ~WebViewMessageEventHandler();

    [[nodiscard]] EventRegistrationToken* Token() noexcept
    {
        return &m_token;
    }
private:
    EventRegistrationToken m_token;

    HRESULT CALLBACK Invoke(ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs*) override;
};

class WebViewCreateControllerEventHandler final
    : public WebViewCallback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
{
public:
    using WebViewCallback::WebViewCallback;
private:
    HRESULT CALLBACK Invoke(HRESULT, ICoreWebView2Controller*) override;
};

class WebViewCreateEnvironmentEventHandler final
    : public WebViewCallback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
{
public:
    using WebViewCallback::WebViewCallback;
private:
    HRESULT CALLBACK Invoke(HRESULT, ICoreWebView2Environment*) override;
};

// Reference (WebView2 Win32 C++)
// https://learn.microsoft.com/en-us/microsoft-edge/webview2/reference/win32
class WebView : public Window
{
public:
    explicit WebView(HWND hWndParent, UINT style, UINT exStyle);
    virtual ~WebView() = default;

    // WebView Core
    // https://learn.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2

    virtual HRESULT Navigate(WSTRV uri);
    virtual HRESULT NavigateHTML(WSTRV html);
    virtual HRESULT ExecuteScript(WSTRV script);
    virtual HRESULT PostWebMessage(WSTRV msg);
    virtual HRESULT PostWebMessageJson(WSTRV msg);

    virtual HRESULT SetVirtualHostNameToFolderMapping(WSTRV hostName, WSTRV dirPath,
        COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND accessKind = COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY);
    virtual HRESULT ClearVirtualHostNameToFolderMapping(WSTRV hostName);

    [[nodiscard]] virtual BOOL IsSuspended();

    // WebView Settings
    // https://learn.microsoft.com/en-us/microsoft-edge/webview2/reference/win32/icorewebview2settings

    VOID SetDevToolsEnabled(BOOL enabled);

    static WebView* GetInstance(HWND hWnd);
protected:
    virtual VOID OnWebMessage(CoTaskStr& uri, CoTaskStr& msg);
    
    virtual VOID OnSize(const Size& si) override;
    virtual VOID OnVisibilityChanged(BOOL visible) override;
private:
    // Interface Pointers

    ComPtr<ICoreWebView2Environment14> m_pEnvironment;
    ComPtr<ICoreWebView2Controller> m_pController;
    ComPtr<ICoreWebView2_9> m_pCoreWebView;
    ComPtr<ICoreWebView2Settings> m_pSettings;

    // Event Handlers

    WebViewCreateEnvironmentEventHandler m_environment { this };
    WebViewCreateControllerEventHandler m_controller { this };
    WebViewMessageEventHandler m_messageEventHandler { this };

    // Class Events

    virtual VOID OnCreateEnvironment() final;
    virtual VOID OnCreateController() final;

    // Class Friends

    friend class WebViewMessageEventHandler;
    friend class WebViewCreateControllerEventHandler;
    friend class WebViewCreateEnvironmentEventHandler;
};
