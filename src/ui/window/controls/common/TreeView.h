struct TreeViewItem
{
    HANDLE hItem;
    LPARAM lParam;
    WSTR text;
};

class TreeView : public CommonControl
{
public:
    explicit TreeView(HWND hWndParent, UINT style, UINT exStyle);
    virtual ~TreeView() = default;

    virtual HANDLE GetPrev(HANDLE hItem);
    virtual HANDLE GetNext(HANDLE hItem, UINT flags = TVGN_NEXT);

    virtual TreeViewItem GetItem(HANDLE hItem);
    virtual std::vector<std::vector<TreeViewItem>> GetSelectedItems();

    virtual VOID SetBkColor(COLORREF color);
    virtual VOID SetExtendedStyle(UINT style);
    virtual VOID SetExtendedStyle(UINT mask, UINT style);

    virtual HANDLE Add(HANDLE hParent, HANDLE hItem, WSTRV text, UINT state = 0, LPARAM lParam = 0);
};
