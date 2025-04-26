#include "../../framework.h"

#define WEBVIEW_CHECK_HR1(hr) WEBVIEW_CHECK_HR(hr, false, false);
#define WEBVIEW_CHECK_HR2(hr) WEBVIEW_CHECK_HR(hr, true, false);
#define WEBVIEW_TRY_CHECK_HR(hr) WEBVIEW_CHECK_HR(hr, false, true);

#define WEBVIEW_CHECK_HR(hr, destroy, t) \
    {                                    \
        HRESULT _ = hr;                  \
        if (FAILED(_)) {                 \
            if constexpr (destroy)       \
                m_pWebView->Destroy();   \
            if (_ == E_ABORT)            \
                return _;                \
            if constexpr (!t) throw;     \
            else if (_ != E_INVALIDARG)  \
                throw;                   \
        }                                \
    }

WebViewMessageEventHandler::~WebViewMessageEventHandler()
{
    m_pWebView->m_pCoreWebView->remove_WebMessageReceived(m_token);
}

HRESULT WebViewMessageEventHandler::Invoke(ICoreWebView2*, ICoreWebView2WebMessageReceivedEventArgs* pArgs)
{
    CoTaskStr uri, msg;
    WEBVIEW_CHECK_HR1(pArgs->get_Source(&uri.Ref()));
    WEBVIEW_TRY_CHECK_HR(pArgs->TryGetWebMessageAsString(&msg.Ref()));
    if (msg.Get() == nullptr) // if it was not a string message
        WEBVIEW_CHECK_HR1(pArgs->get_WebMessageAsJson(&msg.Ref()));
    m_pWebView->OnWebMessage(uri, msg);
    return S_OK;
}

HRESULT WebViewCreateControllerEventHandler::Invoke(HRESULT hr, ICoreWebView2Controller* pController)
{
    WEBVIEW_CHECK_HR2(hr);
    pController->AddRef();
    m_pWebView->m_pController = pController;
    m_pWebView->OnCreateController();
    return S_OK;
}

HRESULT WebViewCreateEnvironmentEventHandler::Invoke(HRESULT hr, ICoreWebView2Environment* pEnvironment)
{
    WEBVIEW_CHECK_HR2(hr);
    COM_QUERY_INTERFACE(pEnvironment, m_pWebView->m_pEnvironment.Ref());
    m_pWebView->OnCreateEnvironment();
    return S_OK;
}

WebView::WebView(HWND hWndParent, UINT style, UINT exStyle)
    : Window(hWndParent, style | WS_CHILD, exStyle)
{
    // Remove all `WM_APP_INITIALIZE` messages from the current GUI thread queue.
    while (PeekMessage(NULL, WM_APP_INITIALIZE, WM_APP_INITIALIZE, PM_REMOVE));

    COM_CHECK_HR(CreateCoreWebView2Environment(&m_environment));

    SetProp(L"_WebView", this);
}

HRESULT WebView::Navigate(WSTRV uri)
{
    if (uri.starts_with(L"./"))
        return Navigate(GetFullPathName(uri));
    return m_pCoreWebView->Navigate(uri.data());
}

HRESULT WebView::NavigateHTML(WSTRV html)
{
    return m_pCoreWebView->NavigateToString(html.data());
}

HRESULT WebView::ExecuteScript(WSTRV script)
{
    return m_pCoreWebView->ExecuteScript(script.data(), NULL);
}

HRESULT WebView::PostWebMessage(WSTRV msg)
{
    return m_pCoreWebView->PostWebMessageAsString(msg.data());
}

HRESULT WebView::PostWebMessageJson(WSTRV msg)
{
    return m_pCoreWebView->PostWebMessageAsJson(msg.data());
}

HRESULT WebView::SetVirtualHostNameToFolderMapping(WSTRV hostName,
    WSTRV dirPath, COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND accessKind)
{
    if (dirPath.starts_with(L"./"))
        return SetVirtualHostNameToFolderMapping(hostName, GetFullPathName(dirPath), accessKind);
    return m_pCoreWebView->SetVirtualHostNameToFolderMapping(hostName.data(), dirPath.data(), accessKind);
}

HRESULT WebView::ClearVirtualHostNameToFolderMapping(WSTRV hostName)
{
    return m_pCoreWebView->ClearVirtualHostNameToFolderMapping(hostName.data());
}

BOOL WebView::IsSuspended()
{
    BOOL suspended = -1;
    m_pCoreWebView->get_IsSuspended(&suspended);
    return suspended;
}

VOID WebView::SetDevToolsEnabled(BOOL enabled)
{
    m_pSettings->put_AreDevToolsEnabled(enabled);
}

WebView* WebView::GetInstance(HWND hWnd)
{
    return (WebView*)Window::GetInstance(hWnd, L"_WebView");
}

VOID WebView::OnWebMessage(CoTaskStr&, CoTaskStr&)
{ }

VOID WebView::OnCreateEnvironment()
{
    COM_CHECK_HR(m_pEnvironment->CreateCoreWebView2Controller(Hwnd(), &m_controller));
}

VOID WebView::OnCreateController()
{
    RECT rc;
    GetClientRect(Hwnd(), &rc);
    m_pController->put_Bounds(rc);

    ICoreWebView2* pWebView = nullptr;
    m_pController->get_CoreWebView2(&pWebView);
    COM_UPGRADE_INTERFACE(pWebView, m_pCoreWebView.Ref());

    m_pCoreWebView->get_Settings(&m_pSettings.Ref());

    m_pCoreWebView->add_WebMessageReceived(&m_messageEventHandler, m_messageEventHandler.Token());

    #ifndef _DEBUG
    SetDevToolsEnabled(FALSE);
    #endif // _DEBUG

    PostMessage(WM_APP_INITIALIZE, NULL, NULL);
}

VOID WebView::OnSize(const Size& si)
{
    RECT rc { 0, 0 };
    si.Get((SIZE&)rc.right, Dpi());
    m_pController->put_Bounds(rc);
}

VOID WebView::OnVisibilityChanged(BOOL visible)
{
    m_pController->put_IsVisible(visible);
}
