inline std::vector<std::wstring_view> StrSplit(std::wstring_view str, std::wstring_view delimiter, size_t limit)
{
    size_t start = 0, end;
    std::vector<std::wstring_view> result;

    while (limit--)
    {
        if (!limit || (end = str.find(delimiter, start)) == std::wstring_view::npos)
        {
            result.emplace_back(str.substr(start));
            break;
        }
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }

    return result;
}
