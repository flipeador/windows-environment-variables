#define CURRENT_THREAD ((HANDLE)-2)

#undef WM_APP
#undef WM_USER

#undef GetProp
#undef SetProp
#undef RemoveProp

#undef SendMessage
#undef PostMessage
#undef PeekMessage
#undef GetWindowText
#undef DefWindowProc
#undef DefWindowProc

#define WM_SIZEWAIT                      0x00000004
#define WM_SETVISIBLE                    0x00000009
#define WM_SYSTEMERROR                   0x00000017
#define WM_CTLCOLOR                      0x00000019
#define WM_LOGOFF                        0x00000025
#define WM_ALTTABACTIVE                  0x00000029
#define WM_SHELLNOTIFY                   0x00000034
#define WM_ISACTIVEICON                  0x00000035
#define WM_QUERYPARKICON                 0x00000036
#define WM_WINHELP                       0x00000038
#define WM_FULLSCREEN                    0x0000003A
#define WM_CLIENTSHUTDOWN                0x0000003B
#define WM_DDEMLEVENT                    0x0000003C
#define WM_TESTING                       0x00000040
#define WM_OTHERWINDOWCREATED            0x00000042
#define WM_OTHERWINDOWDESTROYED          0x00000043
#define WM_COPYGLOBALDATA                0x00000049
#define WM_KEYF1                         0x0000004D
#define WM_ACCESS_WINDOW                 0x0000004F
#define WM_FINALDESTROY                  0x00000070
#define WM_MEASUREITEM_CLIENTDATA        0x00000071
#define WM_SYNCTASK                      0x00000089
#define WM_KLUDGEMINRECT                 0x0000008B
#define WM_LPKDRAWSWITCHWND              0x0000008C
#define WM_UAHDESTROYWINDOW              0x00000090
#define WM_UAHDRAWMENU                   0x00000091
#define WM_UAHDRAWMENUITEM               0x00000092
#define WM_UAHINITMENU                   0x00000093
#define WM_UAHMEASUREMENUITEM            0x00000094
#define WM_UAHNCPAINTMENUPOPUP           0x00000095
#define WM_UAHUPDATE                     0x00000096
#define WM_NCUAHDRAWCAPTION              0x000000AE
#define WM_NCUAHDRAWFRAME                0x000000AF
#define EM_SETFONT                       0x000000C3
#define EM_SETWORDBREAK                  0x000000CA
#define EM_MSGMAX                        0x000000DA
#define WM_YOMICHAR                      0x00000108
#define WM_CONVERTREQUEST                0x0000010A
#define WM_CONVERTRESULT                 0x0000010B
#define WM_INTERIM                       0x0000010C
#define WM_SYSTIMER                      0x00000118
#define WM_GESTUREINPUT                  0x0000011B
#define WM_GESTURENOTIFIED               0x0000011C
#define WM_LBTRACKPOINT                  0x00000131
#define CB_MSGMAX_OLD                    0x00000162
#define CB_MULTIPLEADDSTRING             0x00000163
#define LBCB_CARETON                     0x000001A3
#define LBCB_CARETOFF                    0x000001A4
#define LB_INSERTSTRINGUPPER             0x000001AA
#define LB_INSERTSTRINGLOWER             0x000001AB
#define LB_ADDSTRINGUPPER                0x000001AC
#define LB_ADDSTRINGLOWER                0x000001AD
#define LBCB_STARTTRACK                  0x000001AE
#define LBCB_ENDTRACK                    0x000001AF
#define LB_MSGMAX_OLD                    0x000001B0
#define LB_MULTIPLEADDSTRING             0x000001B1
#define MN_FIRST                         0x000001E0
#define WM_DROPOBJECT                    0x0000022A
#define WM_QUERYDROPOBJECT               0x0000022B
#define WM_BEGINDRAG                     0x0000022C
#define WM_DRAGLOOP                      0x0000022D
#define WM_DRAGSELECT                    0x0000022E
#define WM_DRAGMOVE                      0x0000022F
#define WM_STOPINERTIA                   0x0000023B
#define WM_ENDINERTIA                    0x0000023C
#define WM_EDGYINERTIA                   0x0000023D
#define WM_NCPOINTERLAST                 0x00000244
#define WM_VISIBILITYCHANGED             0x00000270
#define WM_VIEWSTATECHANGED              0x00000271
#define WM_UNREGISTER_WINDOW_SERVICES    0x00000272
#define WM_CONSOLIDATED                  0x00000273
#define WM_IME_REPORT                    0x00000280
#define WM_IME_SYSTEM                    0x00000287
#define WM_KANJILAST                     0x0000029F
#define WM_TRACKMOUSEEVENT_LAST          0x000002AF
#define WM_POINTERDEVICEADDED            0x000002C8
#define WM_POINTERDEVICEDELETED          0x000002C9
#define WM_FLICK                         0x000002CB
#define WM_FLICKINTERNAL                 0x000002CD
#define WM_BRIGHTNESSCHANGED             0x000002CE
#define WM_SYSMENU                       0x00000313
#define WM_HOOKMSG                       0x00000314
#define WM_EXITPROCESS                   0x00000315
#define WM_WAKETHREAD                    0x00000316
#define WM_UAHINIT                       0x0000031B
#define WM_DESKTOPNOTIFY                 0x0000031C
#define WM_DWMEXILEFRAME                 0x00000322
#define WM_MAGNIFICATION_STARTED         0x00000324
#define WM_MAGNIFICATION_ENDED           0x00000325
#define WM_DWMTHUMBNAILSIZECHANGED       0x00000327
#define WM_MAGNIFICATION_OUTPUT          0x00000328
#define WM_BSDRDATA                      0x00000329
#define WM_DWMTRANSITIONSTATECHANGED     0x0000032A
#define WM_KEYBOARDCORRECTIONCALLOUT     0x0000032C
#define WM_KEYBOARDCORRECTIONACTION      0x0000032D
#define WM_UIACTION                      0x0000032E
#define WM_ROUTED_UI_EVENT               0x0000032F
#define WM_MEASURECONTROL                0x00000330
#define WM_GETACTIONTEXT                 0x00000331
#define WM_FORWARDKEYDOWN                0x00000333
#define WM_FORWARDKEYUP                  0x00000334
#define WM_NOTIFYWOW                     0x00000340
#define WM_COALESCE_FIRST                0x00000390
#define WM_COALESCE_LAST                 0x0000039F
#define WM_INTERNAL_DDE_FIRST            0x000003E0
#define WM_INTERNAL_DDE_LAST             0x000003EF
#define WM_CBT_RESERVED_FIRST            0x000003F0
#define WM_CBT_RESERVED_LAST             0x000003FF

#define WM_SYSTEM       0x00000000
#define WM_SYSTEM_MAX   0x000003FF
#define WM_USER         0x00000400
#define WM_USER_MAX     0x00007FFF
#define WM_APP          0x00008000
#define WM_APP_MAX      0x0000BFFF
#define WM_STRING       0x0000C000
#define WM_STRING_MAX   0x0000FFFF
#define WM_RESERVED     0x00010000
#define WM_RESERVED_MAX 0xFFFFFFFF

#define IS_WM_SYSTEM(m)    ( UINT(m) <= WM_SYSTEM_MAX )
#define IS_WM_USER(m)      IN_BOUNDS(UINT(m), WM_USER, WM_USER_MAX)
#define IS_WM_APP(m)       IN_BOUNDS(UINT(m), WM_APP, WM_APP_MAX)
#define IS_WM_STRING(m)    IN_BOUNDS(UINT(m), WM_STRING, WM_STRING_MAX)
#define IS_WM_APPSTRING(m) IN_BOUNDS(UINT(m), WM_APP, WM_STRING_MAX)
#define IS_WM_RESERVED(m)  ( UINT(m) >= WM_RESERVED )

#define WM_APP_TRAYICON          (WM_APP_MAX - 1)
#define WM_APP_THREADARGS        (WM_APP_MAX - 2)
#define WM_APP_INITIALIZE        (WM_APP_MAX - 3)
#define WM_APP_VISIBILITYCHANGED (WM_APP_MAX - 4)

#define ICC_ALL_CLASSES      0x0000FFFF
#define DWM_CLOAKED_NULL     0x00000000

HWND GetWindowRoot(HWND hWnd);
HWND GetWindowOwner(HWND hWnd);
HWND GetWindowParent(HWND hWnd);

HFONT GetWindowFont(HWND hWnd);
VOID SetWindowFont(HWND hWnd, HFONT hFont);

WSTR GetWindowText(HWND hWnd);
WSTR GetWindowClass(HWND hWnd);
UINT GetWindowStyle(HWND hWnd);
UINT GetWindowExStyle(HWND hWnd);

UINT GetWindowThreadId(HWND hWnd);
UINT GetWindowProcessId(HWND hWnd);

UINT GetWindowRect(HWND hWnd, HWND hWndMapTo, Rect& rc);
UINT GetClientRect(HWND hWnd, HWND hWndMapTo, Rect& rc);
BOOL MapWindowRect(HWND hWndFrom, HWND hWndTo, RECT& rc);
VOID AdjustWindowRect(HWND hWnd, RECT& rc);

BOOL MoveWindow(HWND hWnd, const RECT& rc, BOOL update = FALSE);
BOOL MoveWindow(HWND hWnd, const Rect& rc, BOOL update = FALSE);
BOOL MoveWindow(HWND hWnd, const Size& si, BOOL update = FALSE);
BOOL MoveWindow(HWND hWnd, const Point& pt, BOOL update = FALSE);

BOOL PeekMessage(PMSG pMsg, HWND hWnd, UINT min, UINT max, UINT flags);

VOID ActivateWindow(HWND hWnd);
std::vector<HWND> EnumChildWindows(HWND hWnd);
std::vector<HWND> EnumThreadWindows(UINT tid);

BOOL IsMainWindow(HWND hWnd);
BOOL IsTopLevelWindow(HWND hWnd);
BOOL IsCurrentThreadWindow(HWND hWnd);

class WindowRef final
{
public:
    explicit WindowRef(HWND hWnd);
    WindowRef(const WindowRef&) = delete;
    WindowRef(WindowRef&& other) noexcept;
    ~WindowRef();

    BOOL Exists();

    WindowRef& operator=(const WindowRef&) = delete;
    WindowRef& operator=(WindowRef&& other) noexcept;
private:
    WSTR m_id;
    HWND* m_phWnd = nullptr;
};
