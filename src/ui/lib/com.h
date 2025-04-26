#include <windows.h>
#include <expected>
#include <type_traits>

#define COM_CHECK_HR(hr) \
    if (FAILED(hr))      \
        throw;

#define COM_QUERY_INTERFACE(target, out)                 \
    target->QueryInterface(__uuidof(out), (PVOID*)&out); \
    if (out == nullptr) throw;

#define COM_UPGRADE_INTERFACE(target, out) \
    COM_QUERY_INTERFACE(target, out);      \
    target->Release();

template <typename T>
concept DerivedFromIUnknown = std::derived_from<T, IUnknown>;

template <DerivedFromIUnknown T>
inline VOID SafeRelease(T*& rpv)
{
    if (rpv != nullptr)
    {
        rpv->Release();
        rpv = nullptr;
    }
}

template <DerivedFromIUnknown T>
class ComPtr final
{
public:
    explicit ComPtr() = default;
    explicit ComPtr(T* ptr) : m_ptr(ptr) { }
    ~ComPtr() { SafeRelease(m_ptr); }

    [[nodiscard]] T* Get() noexcept { return m_ptr; }
    [[nodiscard]] T*& Ref() noexcept { return m_ptr; }

    T* operator->() noexcept { return m_ptr; }
    ComPtr& operator=(T* ptr) noexcept { m_ptr = ptr; return *this; }
private:
    T* m_ptr = nullptr;
};

class CoTaskStr final
{
public:
    CoTaskStr() = default;
    CoTaskStr(size_t count);
    CoTaskStr(const CoTaskStr&) = delete;
    CoTaskStr(CoTaskStr&& other) noexcept;
    ~CoTaskStr();

    [[nodiscard]] wchar_t* Get() noexcept;
    [[nodiscard]] wchar_t*& Ref() noexcept;
    [[nodiscard]] std::wstring_view& View() noexcept;

    operator bool() const noexcept;
    operator std::wstring_view& () noexcept;
    std::wstring_view* operator->() noexcept;
    CoTaskStr& operator=(const CoTaskStr&) = delete;
    CoTaskStr& operator=(CoTaskStr&& other) noexcept;
private:
    wchar_t* m_ptr = nullptr;
    std::wstring_view m_view;
};

template <DerivedFromIUnknown T>
class Unknown abstract : public T
{
public:
    explicit Unknown() = default;
    virtual ~Unknown() = default;

    virtual ULONG CALLBACK AddRef() override final
    {
        return _InterlockedIncrement(&m_count);
    }

    virtual ULONG CALLBACK Release() override final
    {
        LONG count = _InterlockedDecrement(&m_count);
        if (count == 0l)
            delete this;
        return count;
    }

    virtual HRESULT CALLBACK QueryInterface(const IID& iid, PVOID* ppv) override
    {
        // https://learn.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-qisearch

        if (ppv == nullptr)
            return E_POINTER;

        *ppv = nullptr;

        if (iid == __uuidof(IUnknown))
        {
            AddRef();
            *ppv = static_cast<PVOID>(this);
            return S_OK;
        }

        return E_NOINTERFACE;
    }
private:
    mutable long m_count = 1l;
};
