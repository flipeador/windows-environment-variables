class WinError
{
public:
    explicit WinError(WSTRV message);
    explicit WinError(DWORD code, WSTRV message);

    ~WinError();

    [[nodiscard]] DWORD GetErrorCode() const noexcept;
    [[nodiscard]] std::wstring_view GetErrorMessage();

    INT TaskDialog(HWND hWndOwner);
private:
    WSTR m_text;
    DWORD m_code = NO_ERROR;
    PWSTR m_message = nullptr;
};
