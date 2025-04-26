#include "../framework.h"

CoTaskStr::CoTaskStr(size_t count)
{
    m_ptr = (wchar_t*)CoTaskMemAlloc(sizeof(wchar_t) * count);

    if (m_ptr == nullptr)
        throw;
}

CoTaskStr::CoTaskStr(CoTaskStr&& other) noexcept
    : m_ptr(std::exchange(other.m_ptr, nullptr))
{ }

CoTaskStr::~CoTaskStr()
{
    CoTaskMemFree(m_ptr);
}

wchar_t* CoTaskStr::Get() noexcept
{
    return m_ptr;
}

wchar_t*& CoTaskStr::Ref() noexcept
{
    return m_ptr;
}

std::wstring_view& CoTaskStr::View() noexcept
{
    if (m_view.data() != m_ptr)
        m_view = m_ptr;
    return m_view;
}

CoTaskStr::operator bool() const noexcept
{
    return m_ptr != nullptr;
}

CoTaskStr::operator std::wstring_view&() noexcept
{
    return View();
}

std::wstring_view* CoTaskStr::operator->() noexcept
{
    return &View();
}

CoTaskStr& CoTaskStr::operator=(CoTaskStr&& other) noexcept
{
    if (this != &other)
    {
        CoTaskMemFree(m_ptr);
        m_ptr = std::exchange(other.m_ptr, nullptr);
    }
    return *this;
}
