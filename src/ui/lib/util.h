#undef LoadResource
#undef GetFullPathName

typedef std::function<PVOID()> ThreadFn;
typedef std::function<VOID(HANDLE id, UINT time)> TimerFn;

struct ThreadArgs
{
    PVOID id;
    UINT threadId;
    ThreadFn fn;
};

struct TimerArgs
{
    HWND hWnd;
    TimerFn fn;
};

constexpr double DpiScale(double value, double to, double from = 96.0) noexcept
{
    return value * (to / from);
}

constexpr double DpiUnscale(double value, double to, double from = 96.0) noexcept
{
    return value * (from / to);
}

inline UINT ComputeCrc32(PVOID data, UINT size, UINT initial)
{
    static auto fRtlComputeCrc32 = reinterpret_cast<UINT(*)(UINT,PVOID,UINT)>(
            GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlComputeCrc32"));
    return data && size ? fRtlComputeCrc32(initial, data, size) : 0x00000000u;
}

inline UINT ComputeCrc32(WSTRV str, UINT initial = 0u)
{
    auto size = static_cast<UINT>(str.size() * sizeof(wchar_t));
    return ComputeCrc32((PVOID)str.data(), size, initial);
}

LCID GetLocaleID(PCWSTR name);
WSTR GetFullPathName(WSTRV path);

WSTR LoadResource(WSTRV res, WSTRV lang = L"en-US");
WSTR LoadResource(PCWSTR type, PCWSTR name, PCWSTR lang);
UINT LoadResource(PVOID& rpt, PCWSTR type, PCWSTR name, PCWSTR lang);

BOOL KillTimer(HANDLE timer);
HANDLE SetTimer(UINT time, TimerFn fn);

class Thread final
{
public:
    explicit Thread(ThreadFn fn);
    ~Thread();

    LRESULT Wait();
private:
    HANDLE m_hThread = nullptr;
};

class Point {
public:
    double x, y;

    Point() noexcept;
    Point(const Point& pt) noexcept;
    Point(double x, double y) noexcept;
    Point(INT x, INT y, UINT dpi) noexcept;

    UINT Get(POINT& pt, UINT dpi) const noexcept;

    UINT Set(INT x, INT y, UINT dpi) noexcept;
    UINT Set(const POINT& pt, UINT dpi) noexcept;

    VOID Set(const Point& pt) noexcept;
    VOID Set(double x, double y) noexcept;
};

class Size {
public:
    double width, height;

    Size() noexcept;
    Size(const Size& si) noexcept;
    Size(double width, double height) noexcept;
    Size(INT width, INT height, UINT dpi) noexcept;

    UINT Get(SIZE& si, UINT dpi) const noexcept;

    UINT Set(INT width, INT height, UINT dpi) noexcept;
    UINT Set(const SIZE& si, UINT dpi) noexcept;

    VOID Set(const Size& si) noexcept;
    VOID Set(double width, double height) noexcept;
};

class Rect : public Point, public Size {
public:
    using Point::Get;
    using Point::Set;

    using Size::Get;
    using Size::Set;

    Rect() noexcept = default;
    Rect(const Rect& rc) noexcept;
    Rect(const Point& pt, const Size& si) noexcept;
    Rect(double x, double y, const Size& si) noexcept;
    Rect(const Point& pt, double width, double height) noexcept;
    Rect(double x, double y, double width, double height) noexcept;

    UINT Get(RECT& rc, UINT dpi) const noexcept;
    UINT Get(POINT& pt, SIZE& si, UINT dpi) const noexcept;

    UINT Set(const RECT& rc, UINT dpi) noexcept;

    VOID Set(double x, double y, const Size& si) noexcept;
    VOID Set(const Point& pt, double width, double height) noexcept;
    VOID Set(double x, double y, double width, double height) noexcept;

    [[nodiscard]] bool Contains(const Point& pt) const noexcept;
    [[nodiscard]] bool Intersects(const Rect& rc) const noexcept;
};
