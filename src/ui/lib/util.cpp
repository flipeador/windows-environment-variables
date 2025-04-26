#include "../framework.h"

static VOID TimerProc(HWND hWnd, UINT msg, UINT_PTR id, DWORD time)
{
    ((TimerArgs*)(id))->fn((HANDLE)id, time);
}

static DWORD CALLBACK ThreadProc(PVOID data)
{
    std::unique_ptr<ThreadArgs> args((ThreadArgs*)data);
    return PostThreadMessageW(args->threadId, WM_APP_THREADARGS, (WPARAM)args->id, (LPARAM)args->fn());
}

LCID GetLocaleID(PCWSTR name)
{
    if (IS_INTRESOURCE(name))
        #pragma warning(suppress: 4311 4302)
        return reinterpret_cast<LCID>(name);
    return LocaleNameToLCID(name, LOCALE_ALLOW_NEUTRAL_NAMES);
}

WSTR GetFullPathName(WSTRV path)
{
    WSTR fpath;

    fpath.resize_and_overwrite(LONG_MAXPATH,
        [&path](wchar_t* ptr, size_t count) -> size_t {
            return GetFullPathNameW(path.data(), (DWORD)count, ptr, NULL);
        }
    );

    return fpath;
}

WSTR LoadResource(WSTRV res, WSTRV lang)
{
    auto parts = StrSplit(res, L"/", 2);
    WSTR type(parts[0]), name(parts[1]);
    return LoadResource(type.data(), name.data(), lang.data());
}

WSTR LoadResource(PCWSTR type, PCWSTR name, PCWSTR lang)
{
    wchar_t* ptr = nullptr;
    size_t bytes = LoadResource((PVOID&)ptr, type, name, lang);
    if (ptr == nullptr || bytes == 0)
        return { }; // empty string
    if (bytes % sizeof(wchar_t) != 0 || ptr[0] != UTF16LE_BOM)
        throw std::runtime_error("Expected UTF-16LE string.");
    return { ptr + 1, bytes / sizeof(wchar_t) - 1 };
}

UINT LoadResource(PVOID& rpt, PCWSTR type, PCWSTR name, PCWSTR lang)
{
    auto lcid = static_cast<WORD>(GetLocaleID(lang));
    if (auto hResInfo = FindResourceExW(nullptr, type, name, lcid))
    {
        if (auto hResData = LoadResource(nullptr, hResInfo))
        {
            rpt = LockResource(hResData);
            return SizeofResource(nullptr, hResInfo);
        }
    }
    return std::numeric_limits<UINT>::max();
}

BOOL KillTimer(HANDLE timer)
{
    auto args = (TimerArgs*)timer;
    auto result = KillTimer(args->hWnd, (UINT_PTR)timer);
    if (result) delete args;
    return result;
}

HANDLE SetTimer(UINT time, TimerFn fn)
{
    auto vector = EnumThreadWindows(GetCurrentThreadId());
    auto args = new TimerArgs(vector.front(), std::move(fn));

    if (!SetTimer(args->hWnd, (INT_PTR)args, time, TimerProc))
    {
        delete args;
        return NULL;
    }

    return reinterpret_cast<HANDLE>(args);
}

Thread::Thread(ThreadFn fn)
{
    auto args = new ThreadArgs(this, GetCurrentThreadId(), std::move(fn));
    m_hThread = CreateThread(NULL, 0, ThreadProc, args, 0, NULL);
}

Thread::~Thread()
{
    CloseHandle(m_hThread);
}

LRESULT Thread::Wait()
{
    LRESULT lResult = 0;
    MsgWaitFor(NULL,
        [this, &lResult](MSG& msg) -> BOOL {
            lResult = msg.lParam;
            return msg.message == WM_APP_THREADARGS && msg.wParam == (WPARAM)this;
        }
    );
    return lResult;
}

Point::Point() noexcept
    : x(0.0), y(0.0)
{ }

Point::Point(const Point& pt) noexcept
    : x(pt.x), y(pt.y)
{ }

Point::Point(double x, double y) noexcept
    : x(x), y(y)
{ }

Point::Point(INT x, INT y, UINT dpi) noexcept
{
    Set(x, y, dpi);
}

UINT Point::Get(POINT& pt, UINT dpi) const noexcept
{
    pt.x = static_cast<INT>(::DpiScale(x, dpi));
    pt.y = static_cast<INT>(::DpiScale(y, dpi));
    return dpi;
}

UINT Point::Set(INT x, INT y, UINT dpi) noexcept
{
    this->x = DpiUnscale(x, dpi);
    this->y = DpiUnscale(y, dpi);
    return dpi;
}

UINT Point::Set(const POINT& pt, UINT dpi) noexcept
{
    return Set(pt.x, pt.y, dpi);
}

VOID Point::Set(const Point& pt) noexcept
{
    x = pt.x;
    y = pt.y;
}

VOID Point::Set(double x, double y) noexcept
{
    this->x = x;
    this->y = y;
}

Size::Size() noexcept
    : width(0.0), height(0.0)
{ }

Size::Size(const Size& si) noexcept
    : width(si.width), height(si.height)
{ }

Size::Size(double width, double height) noexcept
    : width(width), height(height)
{ }

Size::Size(INT width, INT height, UINT dpi) noexcept
{
    Set(width, height, dpi);
}

UINT Size::Get(SIZE& si, UINT dpi) const noexcept
{
    si.cx = static_cast<INT>(DpiScale(width, dpi));
    si.cy = static_cast<INT>(DpiScale(height, dpi));
    return dpi;
}

UINT Size::Set(INT width, INT height, UINT dpi) noexcept
{
    this->width = DpiUnscale(width, dpi);
    this->height = DpiUnscale(height, dpi);
    return dpi;
}

UINT Size::Set(const SIZE& si, UINT dpi) noexcept
{
    return Set(si.cx, si.cy, dpi);
}

VOID Size::Set(const Size& si) noexcept
{
    width = si.width;
    height = si.height;
}

VOID Size::Set(double width, double height) noexcept
{
    this->width = width;
    this->height = height;
}

Rect::Rect(const Rect& rc) noexcept
    : Point(rc.x, rc.y), Size(rc.width, rc.height)
{ }

Rect::Rect(const Point& pt, const Size& si) noexcept
    : Point(pt), Size(si)
{ }

Rect::Rect(double x, double y, const Size& si) noexcept
    : Point(x, y), Size(si)
{ }

Rect::Rect(const Point& pt, double width, double height) noexcept
    : Point(pt), Size(width, height)
{ }

Rect::Rect(double x, double y, double width, double height) noexcept
    : Point(x, y), Size(width, height)
{ }

UINT Rect::Get(RECT& rc, UINT dpi) const noexcept
{
    Get(*reinterpret_cast<POINT*>(&rc), dpi);
    rc.right = static_cast<INT>(DpiScale(x + width, dpi));
    rc.bottom = static_cast<INT>(DpiScale(y + height, dpi));
    return dpi;
}

UINT Rect::Get(POINT& pt, SIZE& si, UINT dpi) const noexcept
{
    Get(pt, dpi);
    Get(si, dpi);
    return dpi;
}

UINT Rect::Set(const RECT& rc, UINT dpi) noexcept
{
    Set(*reinterpret_cast<const POINT*>(&rc), dpi);
    width = DpiUnscale(rc.right - rc.left, dpi);
    height = DpiUnscale(rc.bottom - rc.top, dpi);
    return dpi;
}

VOID Rect::Set(double x, double y, const Size& si) noexcept
{
    Set(x, y, si.width, si.height);
}

VOID Rect::Set(const Point& pt, double width, double height) noexcept
{
    Set(pt.x, pt.y, width, height);
}

VOID Rect::Set(double x, double y, double width, double height) noexcept
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

bool Rect::Contains(const Point& pt) const noexcept
{
    return pt.x >= x && pt.x <= x + width &&
        pt.y >= y && pt.y <= y + height;
}

bool Rect::Intersects(const Rect& rc) const noexcept
{
    return x < rc.x + rc.width && x + width > rc.x &&
        y < rc.y + rc.height && y + height > rc.y;
}
