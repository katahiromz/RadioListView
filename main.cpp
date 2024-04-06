#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>
#include "resource.h"

#define I_UNCHECKED 1
#define I_CHECKED 2

INT s_nUpdating = 0;

// ラジオボタン風の状態を表示するためのイメージリストを作成する。
HIMAGELIST DoCreateRadioImageList(HWND hwnd)
{
    HIMAGELIST himl;
    HDC hdc_wnd, hdc;
    HBITMAP hbm_im, hbm_mask;
    HGDIOBJ hbm_orig;
    HBRUSH hbr_white = GetStockBrush(WHITE_BRUSH), hbr_black = GetStockBrush(BLACK_BRUSH);
    RECT rc;

    // 小さいイメージリストを作成する。
    himl = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                            ILC_COLOR | ILC_MASK, 2, 2);
    // いろいろ準備。
    hdc_wnd = GetDC(hwnd);
    hdc = CreateCompatibleDC(hdc_wnd);
    hbm_im = CreateCompatibleBitmap(hdc_wnd, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
    hbm_mask = CreateBitmap(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 1, 1, NULL);
    ReleaseDC(hwnd, hdc_wnd);

    // マスクビットマップを描画。
    SetRect(&rc, 0, 0, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
    hbm_orig = SelectObject(hdc, hbm_mask);
    FillRect(hdc, &rc, hbr_white);
    InflateRect(&rc, -1, -1);
    SelectObject(hdc, hbr_black);
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);

    // チェックしていないアイコンを追加。
    SelectObject(hdc, hbm_im);
    SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, hbm_orig);
    ImageList_Add(himl, hbm_im, hbm_mask); 

    // チェック済みのアイコンを追加。
    SelectObject(hdc, hbm_im);
    SelectObject(hdc, GetSysColorBrush(COLOR_WINDOW));
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    InflateRect(&rc, -3, -3);
    SelectObject(hdc, GetSysColorBrush(COLOR_WINDOWTEXT));
    Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
    SelectObject(hdc, hbm_orig);
    ImageList_Add(himl, hbm_im, hbm_mask);

    // 後片付け。
    DeleteObject(hbm_mask);
    DeleteObject(hbm_im);
    DeleteDC(hdc);

    return himl;
}

// リストビューのチェック位置を取得する。
INT DoGetCurSel(HWND hwndLst1)
{
    INT cItems = ListView_GetItemCount(hwndLst1);
    for (INT iItem = 0; iItem < cItems; ++iItem)
    {
        UINT uState = ListView_GetItemState(hwndLst1, iItem, LVIS_STATEIMAGEMASK);
        if (uState & INDEXTOSTATEIMAGEMASK(I_CHECKED))
            return iItem;
    }
    return -1;
}

// リストビューのチェック情報を更新する。
void DoSetCurSel(HWND hwndLst1, INT iSelect)
{
    INT cItems = ListView_GetItemCount(hwndLst1);
    for (INT iItem = 0; iItem < cItems; ++iItem)
    {
        if (iItem == iSelect)
        {
            ListView_SetItemState(hwndLst1, iItem, INDEXTOSTATEIMAGEMASK(I_CHECKED), LVIS_STATEIMAGEMASK);
        }
        else
        {
            ListView_SetItemState(hwndLst1, iItem, INDEXTOSTATEIMAGEMASK(I_UNCHECKED), LVIS_STATEIMAGEMASK);
        }
    }

    ListView_SetItemState(hwndLst1, iSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(hwndLst1, iSelect, FALSE);
}

const LPCTSTR s_Items[][2] =
{
    { TEXT("Item #0"), TEXT("This is a test") },
    { TEXT("Item #1"), TEXT("This is a test") },
    { TEXT("Item #2"), TEXT("This is a test") },
    { TEXT("Item #3"), TEXT("This is a test") },
    { TEXT("Item #4"), TEXT("This is a test") },
    { TEXT("Item #5"), TEXT("This is a test") },
};

// リストビューの項目を追加。
void PopulateListView(HWND hwndLst1)
{
    // すべての項目を消去する。
    ListView_DeleteAllItems(hwndLst1);

    // リストビューを埋める。
    INT iItem = 0;
    for (auto& entry : s_Items)
    {
        LV_ITEM item = { LVIF_TEXT };
        item.pszText = const_cast<LPTSTR>(entry[0]);
        item.iItem = iItem;
        item.iSubItem = 0;
        ListView_InsertItem(hwndLst1, &item);
        item.iItem = iItem;
        item.iSubItem = 1;
        item.pszText = const_cast<LPTSTR>(entry[1]);
        ListView_SetItem(hwndLst1, &item);
        ++iItem;
    }
}

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndLst1 = GetDlgItem(hwnd, lst1);

    // 拡張リストビュースタイルを設定。
    ListView_SetExtendedListViewStyle(hwndLst1,
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

    // 辞書リストのヘッダーを初期化。
    LV_COLUMN column = { LVCF_TEXT | LVCF_WIDTH };
    TCHAR szText1[] = TEXT("Filename");
    column.pszText = szText1;
    column.cx = 200;
    ListView_InsertColumn(hwndLst1, 0, &column);
    TCHAR szText2[] = TEXT("Display Name");
    column.pszText = szText2;
    column.cx = 250;
    ListView_InsertColumn(hwndLst1, 1, &column);

    // イメージリストを設定。
    HIMAGELIST himl = DoCreateRadioImageList(hwndLst1);
    ListView_SetImageList(hwndLst1, himl, LVSIL_STATE);

    // リストビューの項目を追加。
    PopulateListView(hwndLst1);

    // リストビューの最初の項目を選択。
    ++s_nUpdating;
    DoSetCurSel(hwndLst1, 0);
    --s_nUpdating;

    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        // ダイアログを終了する。
        EndDialog(hwnd, id);
        break;
    }
}

LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    switch (idFrom)
    {
    case lst1:
        // 更新中ではなく、選択が変更された。
        if (pnmhdr->code == LVN_ITEMCHANGED && !s_nUpdating)
        {
            NM_LISTVIEW *pListView = (NM_LISTVIEW *)pnmhdr;
            if (pListView->uNewState & LVIS_SELECTED)
            {
                HWND hwndLst1 = GetDlgItem(hwnd, lst1);

                // リストビューのチェック状態を更新する。
                ++s_nUpdating;
                DoSetCurSel(hwndLst1, pListView->iItem);
                --s_nUpdating;
            }
        }
        break;
    }
    return 0;
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    InitCommonControls();
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc);
    return 0;
}
