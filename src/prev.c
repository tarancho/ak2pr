/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-18 13:07:15 tfuruka1>
 *
 * 「ak2psのようなもの」の印刷プレビュー
 *
 * $Id: prev.c,v 1.2 2001/12/18 04:07:51 tfuruka1 Exp $
 * $Log: prev.c,v $
 * Revision 1.2  2001/12/18 04:07:51  tfuruka1
 * プレビューウインドウの位置とサイズを記憶しておくように修正
 *
 * Revision 1.1  2001/12/14 17:09:38  tfuruka1
 * Initial revision
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

PPREVIEW_INFO pPrevInfo = NULL;

static VOID
DoPaint(HWND hWnd)
{
    HDC hDC;
    PAINTSTRUCT ps;
    RECT rc;
    int wd, ht;
    int x, y;

    GetClientRect(hWnd, &rc);

    // クライアント領域の演算(クラアント領域に収まるようにアスペクトル
    // 比を保持したままサイズを計算する)
    wd = rc.right;
    ht = pPrevInfo->ht * wd / pPrevInfo->wd;
    if (ht > rc.bottom) {
        ht = rc.bottom;
        wd = pPrevInfo->wd * ht / pPrevInfo->ht;
    }

    // クライアント領域の中央に配置
    x = rc.right / 2 - wd / 2;
    y = rc.bottom / 2 - ht / 2;
    
    hDC = BeginPaint(hWnd, &ps);

    if (wd != rc.right) {
        // 横方向の余白を塗りつぶす
        rc.left = x + wd;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));

        rc.left = 0;
        rc.right = x;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));
    }
    else if (ht != rc.bottom) {
        // 縦方向の余白を塗りつぶす
        rc.top = y + ht;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));

        rc.top = 0;
        rc.bottom = y;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));
    }

    // プレビューイメージを転送
    if (!StretchBlt(hDC, x, y, wd, ht,
                    pPrevInfo->hDC, 0, 0, pPrevInfo->wd, pPrevInfo->ht,
                    SRCCOPY)) {
        int nErr = GetLastError();
        DbgPrint(NULL, 'E', "DoPaint: %s",
                 GetLastErrorMessage("StretchBlt()", nErr));
    }

    EndPaint(hWnd, &ps);
}

static BOOL CALLBACK
DialogProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch (uMsg) {
    case WM_INITDIALOG:
        SetWindowPos(hWnd, HWND_TOPMOST,
                     pPrevInfo->rc.left, pPrevInfo->rc.top,
                     pPrevInfo->rc.right - pPrevInfo->rc.left,
                     pPrevInfo->rc.bottom - pPrevInfo->rc.top,
                     0);
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    case WM_SYSCOMMAND:
        switch (wParam) {
        case SC_CLOSE:
            EndDialog(hWnd, FALSE);
            return TRUE;
        default:
            return FALSE;
        }
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hWnd, TRUE);
            pPrevInfo->status = PVI_PRINT;
            GetWindowRect(hWnd, &pPrevInfo->rc);
            SetPreViewPos(&pPrevInfo->rc);
            return TRUE;
        case IDCANCEL:
            EndDialog(hWnd, FALSE);
            pPrevInfo->status = PVI_CANCEL;
            return TRUE;
        default:
            return FALSE;
        }
        return FALSE;
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    case WM_PAINT:
        DoPaint(hWnd);
        return TRUE;
    }
    return FALSE;
}

BOOL
PrintPreview(
    HWND hWnd,                                  // ウインドウハンドル
    PPREVIEW_INFO ppi                           // プレビュー情報
    )
{
    int nRet;
    HINSTANCE hInst = GetWindowInstance(hWnd);

    pPrevInfo = ppi;
    nRet = DialogBox(hInst, MAKEINTRESOURCE(IDD_PREV), hWnd, DialogProc);
    if (-1 == nRet) {
        DbgPrint(NULL, 'E', "%s", GetLastErrorMessage("DialogBox",
                                                      GetLastError()));
        return FALSE;
    }
    return nRet;
}

/*--------------------------------------------------------------------
 * hDCPrinterで指定されてたデバイスコンテキストから、プレビュー用のデ
 * バイスコンテキストを作成する。プレビュー用のデバイスコンテキストは
 * 使用し終わった後に破棄する必要があります。また、デバイスコンテキス
 * トが所有しているビットマップも破棄する必要があります。
 * *-------------------------------------------------------------------*/
HDC WINAPI
MakePreviewInfo(HWND hWnd,                      // ウインドウハンドル
                HDC hDCPrinter,                 // プリンタDC
                PPREVIEW_INFO pPrev             // プレビュー情報
    )
{
    HDC hDC = GetDC(hWnd);
    int nDPIWPrt, nDPIHPrt;                     // プリンタの解像度
    int nWidthPrt, nHeightPrt;                  // プリンタのサイズ
    int nPaperMarginW, nPaperMarginH;           // 印刷不可エリア

    if (!hDC) {
        DbgPrint(NULL, 'E', "%s",
                 GetLastErrorMessage("GetDC" __FILE__,
                                     GetLastError()));
        return NULL;
    }

    if (!(pPrev->hDC = CreateCompatibleDC(hDC))) {
        DbgPrint(NULL, 'E', "%s",
                 GetLastErrorMessage("CreateCompatibleDC" __FILE__,
                                     GetLastError()));
        ReleaseDC(hWnd, hDC);
        return NULL;
    }

    // プリンタの解像度を得る
    nDPIWPrt = GetDeviceCaps(hDCPrinter, LOGPIXELSX);
    nDPIHPrt = GetDeviceCaps(hDCPrinter, LOGPIXELSY);

    // 画面の解像度を得る
    pPrev->dpiW = GetDeviceCaps(pPrev->hDC, LOGPIXELSX);
    pPrev->dpiH = GetDeviceCaps(pPrev->hDC, LOGPIXELSY);

    // プリンタの用紙サイズを得る
    nWidthPrt = GetDeviceCaps(hDCPrinter, PHYSICALWIDTH);
    nHeightPrt = GetDeviceCaps(hDCPrinter, PHYSICALHEIGHT);

    // 画面のサイズを決定する
    pPrev->wd = nWidthPrt * pPrev->dpiW / nDPIWPrt;
    pPrev->ht = nHeightPrt * pPrev->dpiH / nDPIHPrt;

    // プリンタの印刷不可エリアを得る
    nPaperMarginW = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETX);
    nPaperMarginH = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETY);

    // 画面の印刷不可エリアを設定する
    pPrev->xoff = nPaperMarginW * pPrev->dpiW / nDPIWPrt;
    pPrev->yoff = nPaperMarginH * pPrev->dpiH / nDPIHPrt;

    // ビットマップを作成する
    if (!(pPrev->hBitmap
          = CreateCompatibleBitmap(hDC, pPrev->wd, pPrev->ht))) {
        GetLastErrorMessage("CreateCompatibleBitmap" __FILE__,
                            GetLastError());
        ReleaseDC(hWnd, hDC);
        return NULL;
    }
    SelectObject(pPrev->hDC, pPrev->hBitmap);

    DbgPrint(NULL, 'D', "プレビュー用デバイスコンテキストを作成しました\n"
             "     |   Printer   |   Preview\n"
             "DPI  | %5dx%5d | %5dx%5d\n"
             "SIZE | %5dx%5d | %5dx%5d",
             nDPIWPrt, nDPIHPrt, pPrev->dpiW, pPrev->dpiH,
             nWidthPrt, nHeightPrt, pPrev->wd, pPrev->ht);

    ReleaseDC(hWnd, hDC);

    return pPrev->hDC;
}
