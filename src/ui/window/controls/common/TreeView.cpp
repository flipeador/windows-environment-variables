#include "../../../framework.h"

TreeView::TreeView(HWND hWndParent, UINT style, UINT exStyle)
    : CommonControl(WC_TREEVIEW, hWndParent, style, exStyle)
{ }

HANDLE TreeView::GetPrev(HANDLE hItem)
{
    return GetNext(hItem, TVGN_PREVIOUS);
}

HANDLE TreeView::GetNext(HANDLE hItem, UINT flags)
{
    return (HANDLE)SendMessage(TVM_GETNEXTITEM, flags, (LPARAM)hItem);
}

TreeViewItem TreeView::GetItem(HANDLE hItem)
{
    TreeViewItem item { hItem };
    TVITEMEXW tvi { TVIF_TEXT | TVIF_PARAM, (HTREEITEM)hItem };
    item.text.resize_and_overwrite(260,
        [this, &tvi](wchar_t* ptr, size_t count) -> size_t {
            tvi.pszText = ptr;
            tvi.cchTextMax = static_cast<INT>(count);
            SendMessage(TVM_GETITEM, NULL, (LPARAM)&tvi);
            return wcslen(tvi.pszText);
        }
    );
    item.lParam = tvi.lParam;
    return item;
}

std::vector<std::vector<TreeViewItem>> TreeView::GetSelectedItems()
{
    HANDLE hItem = nullptr;
    std::vector<std::vector<TreeViewItem>> selectedItems;
    while (hItem = GetNext(hItem, TVGN_NEXTSELECTED))
    {
        std::vector<TreeViewItem> selectionTree { GetItem(hItem) };
        HANDLE hParent = hItem;
        while (hParent = GetNext(hParent, TVGN_PARENT))
            selectionTree.insert(selectionTree.begin(), GetItem(hParent) );
        selectedItems.insert(selectedItems.begin(), selectionTree);
    }
    return selectedItems;
}

VOID TreeView::SetBkColor(COLORREF color)
{
    SendMessage(TVM_SETBKCOLOR, NULL, color);
}

VOID TreeView::SetExtendedStyle(UINT style)
{
    SetExtendedStyle(style, style);
}

VOID TreeView::SetExtendedStyle(UINT mask, UINT style)
{
    SendMessage(TVM_SETEXTENDEDSTYLE, mask, style);
}

HANDLE TreeView::Add(HANDLE hParent, HANDLE hItem, WSTRV text, UINT state, LPARAM lParam)
{
    TVINSERTSTRUCTW lp { (HTREEITEM)hParent, (HTREEITEM)hItem };
    lp.itemex.state = state;
    lp.itemex.stateMask = state;
    lp.itemex.mask = TVIF_STATE | TVIF_TEXT | TVIF_PARAM;
    lp.itemex.pszText = const_cast<wchar_t*>(text.data());
    lp.itemex.lParam = lParam;
    return (HANDLE)SendMessage(TVM_INSERTITEM, NULL, (LPARAM)&lp);
}
