#include "../framework.h"

WinError::WinError(WSTRV message)
    : WinError(::GetLastError(), message)
{ }

WinError::WinError(DWORD code, WSTRV message)
    : m_code(code), m_text(message)
{ }

WinError::~WinError()
{
    LocalFree(m_message);
}

DWORD WinError::GetErrorCode() const noexcept
{
    return m_code;
}

std::wstring_view WinError::GetErrorMessage()
{
    if (m_message == nullptr)
    {
        DWORD size = FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_ALLOCATE_BUFFER,
            nullptr, m_code, 0, (PWSTR)&m_message, 0, nullptr);

        PWSTR end = m_message + size - 1;
        while (end >= m_message && iswspace(*end))
            *end-- = L'\0';
    }

    return m_message;
}

INT WinError::TaskDialog(HWND hWndOwner)
{
    auto inst = std::format(L"Error {}", m_code);
    auto content = std::format(L"{}\n\n{}", m_text, GetErrorMessage());

    INT button = 0;
    ::TaskDialog(hWndOwner, nullptr, nullptr, inst.data(), content.data(), TDCBF_OK_BUTTON, nullptr, &button);

    return button;
}
