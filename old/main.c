/* -*- C++ -*-
 *
 * コマンドラインから使用できるプリンタ出力コマンド。実はメールを印刷
 * する為だけに作成しました。こちらはコマンドラインから引数を受ける事
 * が出来ます。
 *
 * 例えば、Mewから印刷する時は以下のように .emacsに記述します
 *
 * (setq mew-print-command-format "mpr")
 *
 * (defun mew-print-region (begin end)
 *  (interactive "r")
 *  (shell-command-on-region begin end mew-print-command-format))
 *
 * (defun mew-print-buffer ()
 *  (interactive)
 *  (mew-print-region (point-min) (point-max)))
 *
 * (setq mew-print-function (function mew-print-buffer)) */

// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")

#include "mpr.h"

#define TIME_STAMP "Time-stamp: <99/04/16 19:31:57 tfuruka1>"

#define FROM_POINT(n) (int)((n) / 72.0 * nDPIW)
#define FROM_CM(n)    (int)((n) * nDPIW / 2.54)

#define FN_MSM      TEXT("ＭＳ 明朝")
#define FN_MSPG     TEXT("ＭＳ Ｐゴシック")
#define FN_MSG      TEXT("ＭＳ ゴシック")

#define SZ_IN_REPLY_TO "In-Reply-To:"
#define SZ_REFERENCES  "References:"
#define SZ_FROM        "From:"
#define SZ_DATE        "Date:"
#define SZ_RECEIVED    "Received:"

static BOOL bKanji, bKeisen;
static TCHAR szTitleStr[80], g_szReference[512], g_szMessageID[256];
static TCHAR g_szFrom[128], g_szDate[128];
static int nPaperWidth, nPaperHeight, nDPIW, nDPIH, nPaperMarginW,
    nPaperMarginH, nStartX, nEndX, nStartY, nEndY, nPageNo, nCurrentX,
    nCurrentY, nCenter; nHalfWidth, nBasePoint, nBaseLine, nBaseLineK;
static PRINTDLG pd;
static DOCINFO di;
static LOGFONT lf = {-13, 0, 0, 0, 800, 0, 0, 0, SHIFTJIS_CHARSET, 3, 2, 1, 49,
                     TEXT("ＭＳ ゴシック")};
static LOGFONT lf2 = {-1, 0, 0, 0, 800, 0, 0, 0, SHIFTJIS_CHARSET, 3, 2, 1, 49,
                      TEXT("ＭＳ ゴシック")};
static LPTSTR szKeisen = {"│┃││┃┃┃┃─┘┛┐┤┨┓┨┨━┛┛┓"
                          "┥┫┓┫┫─└┗┌├┠┏┠┠─┴┸┬┼╂┰"
                          "╂╂━┷┻┯┿╋┳╋╋━┗┗┏┝┣┏┣┣━"
                          "┷┻┯┿╋┳╋╋━┷┻┯┿╋┳╋╋"};
static BOOL
bInitPrinter(HWND hwnd)
{
    LPTSTR szErr = NULL;

    // Initialize the PRINTDLG members.
    pd.lStructSize = sizeof(PRINTDLG);
    pd.hDevMode = (HANDLE)NULL;
    pd.hDevNames = (HANDLE)NULL;
    pd.Flags = PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION;
    pd.hwndOwner = hwnd;
    pd.hDC = (HDC)NULL;
    pd.nFromPage = 1;
    pd.nToPage = 1;
    pd.nMinPage = 0;
    pd.nMaxPage = 0;
    pd.nCopies = 1;
    pd.hInstance = (HANDLE)NULL;
    pd.lCustData = 0L;
    pd.lpfnPrintHook = (LPPRINTHOOKPROC)NULL;
    pd.lpfnSetupHook = (LPSETUPHOOKPROC)NULL;
    pd.lpPrintTemplateName = (LPSTR)NULL;
    pd.lpSetupTemplateName = (LPSTR)NULL;
    pd.hPrintTemplate = (HANDLE)NULL;
    pd.hSetupTemplate = (HANDLE)NULL;

    // Display the PRINT dialog box.
    if (!PrintDlg(&pd)) {
        return FALSE;
    }

    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = "Messaging in the Emacs World(Mew)の為に作ったmpr";
    di.lpszOutput = (LPTSTR)NULL;

    if (SP_ERROR == StartDoc(pd.hDC, &di)) {
        szErr = "StartDoc";
        goto Error;
    }

    nPaperWidth = GetDeviceCaps(pd.hDC, PHYSICALWIDTH);
    nPaperHeight = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT);
    nPaperMarginW = GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
    nPaperMarginH = GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);

    nDPIW = GetDeviceCaps(pd.hDC, LOGPIXELSX);
    nDPIH = GetDeviceCaps(pd.hDC, LOGPIXELSY);

    nCurrentX = nCurrentY = 0;
    nPageNo = 1;
    bKanji = FALSE;
    bKeisen = FALSE;

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        nBasePoint = FROM_POINT(9);
        nBaseLineK = FROM_POINT(14);
        nBaseLine = FROM_POINT(11);
    }
    else {                                      // 縦置きの場合
        nBasePoint = FROM_POINT(10);
        nBaseLineK = FROM_POINT(15);
        nBaseLine = FROM_POINT(12);
    }

    return TRUE;
Error:
    // Delete the printer DC.
    DeleteDC(pd.hDC);
    if (szErr) {
        MessageBox(hwnd, szErr, TEXT("bInitPrinter"), MB_ICONSTOP);
    }
    return FALSE;
}

static BOOL
bEndPage(HWND hwnd)
{
    HFONT hFont, hOldFont;
    RECT rc, rc2, rcBase;
    TCHAR szBuf[768];
    int nHt;

    // タイトルの出力
    lf.lfHeight = FROM_POINT(18);
    lf.lfWeight = 700;
    lf.lfItalic = lf.lfUnderline = lf.lfStrikeOut = FALSE;
    strcpy(&lf.lfFaceName[0], FN_MSPG);

    hFont = CreateFontIndirect(&lf);
    hOldFont = SelectObject(pd.hDC, hFont);
    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        rc.left = nDPIW / 2 - nPaperMarginW;
        rc.top = nDPIH - nPaperMarginH;
        rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
        rc.bottom = rc.top + FROM_POINT(20);
    }
    else {                                      // 縦置きの場合
        rc.left = nDPIW - nPaperMarginW;
        rc.top = nDPIH / 2 - nPaperMarginH;
        rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
        rc.bottom = rc.top + FROM_POINT(20);
    }
    memcpy(&rcBase, &rc, sizeof(RECT));

    rc.left += FROM_POINT(10);
    rc.right -= FROM_POINT(10);
    DrawText(pd.hDC, szTitleStr, -1, &rc,
             DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);

    // Fromと受信日時の表示
    sprintf(szBuf, "%s\r\n%s", g_szFrom, g_szDate);

    lf.lfHeight = FROM_POINT(8);
    hFont = CreateFontIndirect(&lf);
    hOldFont = SelectObject(pd.hDC, hFont);

    memcpy(&rc2, &rc, sizeof(RECT));
    nHt = DrawText(pd.hDC, szBuf, -1, &rc2,
                   DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK | DT_CALCRECT);
    rc.top += ((FROM_POINT(20) - nHt) / 2);
    rc.bottom = rc.top + nHt;

    DrawText(pd.hDC, szBuf, -1, &rc, DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

    // リファレンスの表示とメッセージIDの表示
    memcpy(&rc, &rcBase, sizeof(RECT));
    rc.top -= FROM_POINT(18);
    rc.bottom = rc.top + FROM_POINT(16);
    sprintf(szBuf, "%s\r\n%s", g_szReference, g_szMessageID);
    DrawText(pd.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);


    if (0 >= EndPage(pd.hDC)) {
        DeleteDC(pd.hDC);
        MessageBox(hwnd, TEXT("EndPage"), TEXT("bEndPage"), MB_ICONSTOP);
        return FALSE;
    }
    return TRUE;
}

static BOOL
bFinePrinter(HWND hwnd)
{
    int nError;
    LPTSTR szErr = NULL;

    if (!bEndPage(hwnd))
        return FALSE;

    nError = EndDoc(pd.hDC);
    if (nError <= 0) {
        MessageBox(hwnd, szErr, TEXT("bFinePrinter"), MB_ICONSTOP);
    }
    // Delete the printer DC.
    DeleteDC(pd.hDC);
    return TRUE;
}

static BOOL
bBackDrawPrinter(HWND hwnd)
{
    time_t t;
    HPEN hPen, hOldPen;
    HBRUSH hBrush, hOldBrush;
    LOGPEN lgpen;
    HFONT hFont, hOldFont;
    POINT pt[3];
    RECT rc;
    int nError, nLineTop, nHt, nWd;
    LPTSTR szErr = NULL;
    TCHAR szBuf[256];

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        if (0 == (nPageNo % 2)) {
            nStartX = nCenter + FROM_POINT(5);
            nEndX = nStartX + nHalfWidth;
            nPageNo++;
            return TRUE;
        }
    }

    nError = StartPage(pd.hDC);
    if (nError <= 0) {
        goto Error;
    }


    lgpen.lopnStyle = PS_SOLID;
    lgpen.lopnWidth.x = nDPIW / 72 + 1;         // 1Point = 1/72inchi
    lgpen.lopnColor = RGB(0, 0, 0);

    // 外枠の描画
    hPen = CreatePenIndirect(&lgpen);
    hBrush = CreateSolidBrush(RGB(255, 255, 255));
    hOldPen = SelectObject(pd.hDC, hPen);
    hOldBrush = SelectObject(pd.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        RoundRect(pd.hDC,
                  nStartX = nDPIW / 2 - nPaperMarginW,
                  nLineTop = nStartY = nDPIH - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }
    else {                                      // 縦置きの場合
        RoundRect(pd.hDC,
                  nStartX = nDPIW - nPaperMarginW,
                  nLineTop = nStartY = nDPIH / 2 - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }
/*
    // 透かしの印刷
    rc.top = rc.left = 0;
    rc.right = nPaperWidth;
    rc.bottom = nPaperHeight;

    {
        char szStr[1024];
        sprintf(szStr, "%d %d %d %d", rc.left, rc.top, rc.right, rc.bottom);
        MessageBox(NULL, szStr, NULL, 0);
    }

    lf.lfHeight = FROM_POINT(92);
    lf.lfItalic = FALSE;
    lf.lfEscapement = (int)
        (atan((double)nPaperHeight / nPaperWidth) * 180 / PI * 10) - 1800;
    lf.lfOrientation = lf.lfEscapement;
    hFont = CreateFontIndirect(&lf);
    hOldFont = SelectObject(pd.hDC, hFont);

    SetBkMode(pd.hDC, TRANSPARENT);             // 透かす
    SetTextColor(pd.hDC, RGB(192, 192, 192));

    nHt = DrawText(pd.hDC, "Draft Version", -1, &rc,
                   DT_NOPREFIX | DT_CENTER | DT_VCENTER |
                   DT_SINGLELINE | DT_CALCRECT);
    nWd = (rc.right - rc.left);
    {
        char szStr[1024];
        sprintf(szStr, "%d %d", nHt, nWd);
        MessageBox(NULL, szStr, NULL, 0);
    }

    rc.top = (nPaperHeight - nHt) / 2;
    rc.bottom = rc.top + nHt;
    rc.left = (nPaperWidth - nWd) / 2;
    rc.right = rc.left + nWd;

    DrawText(pd.hDC, "Draft Version", -1, &rc,
             DT_NOPREFIX | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SetTextColor(pd.hDC, RGB(0, 0, 0));

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);

    lf.lfEscapement = lf.lfOrientation = 0;
    */

    // タイトルと本文の仕切り線
    nStartY += FROM_POINT(20);
    pt[0].x = nStartX;
    pt[0].y = pt[1].y = nStartY;
    pt[1].x = nEndX;
    Polyline(pd.hDC, &pt[0], 2);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        // 中央の線を描画する
        pt[0].x = pt[1].x = nPaperWidth / 2 - nPaperMarginW;
        pt[0].y = nStartY;
        pt[1].y = nEndY;
        Polyline(pd.hDC, &pt[0], 2);
        nEndX = nCenter = pt[0].x;
    }

    nStartX += FROM_POINT(5);
    nEndX -= nBaseLine;
    nStartY += FROM_POINT(5);
    nEndY -= nBaseLine;

    nHalfWidth = nEndX - nStartX + 1;

    SelectObject(pd.hDC, hOldBrush);
    SelectObject(pd.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // 綴じ位置を描画する
    lgpen.lopnWidth.x = 1;
    hPen = CreatePenIndirect(&lgpen);
    hBrush = CreateSolidBrush(RGB(0, 0, 0));
    hOldPen = SelectObject(pd.hDC, hPen);
    hOldBrush = SelectObject(pd.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        pt[0].x = nCenter;
        pt[1].y = pt[2].y = FROM_POINT(10);
        pt[0].y = 0;
        pt[1].x = pt[0].x + FROM_POINT(5);
        pt[2].x = pt[0].x - FROM_POINT(5);
    }
    else {                                      // 縦置きの場合
        pt[0].x = 0;
        pt[1].x = pt[2].x = FROM_POINT(10);
        pt[0].y = nCenter = (nPaperHeight / 2) - nPaperMarginH;
        pt[1].y = pt[0].y + FROM_POINT(5);
        pt[2].y = pt[0].y - FROM_POINT(5);
    }
    Polygon(pd.hDC, &pt[0], 3);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        Arc(pd.hDC,
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH),
            (int)(nCenter - FROM_CM(7.4) / 2),
            (int)(FROM_CM(.8) - nPaperMarginH + FROM_CM(.6)),
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH),
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH));
        Arc(pd.hDC,
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH),
            (int)(nCenter + FROM_CM(7.4) / 2),
            (int)(FROM_CM(.8) - nPaperMarginH + FROM_CM(.6)),
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH),
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginH));
    }
    else {
        Arc(pd.hDC,
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginW + FROM_CM(.6)),
            (int)(nCenter - FROM_CM(7.4) / 2),
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter - FROM_CM(7.4) / 2 - FROM_CM(.6)));
        Arc(pd.hDC,
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginW + FROM_CM(.6)),
            (int)(nCenter + FROM_CM(7.4) / 2),
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)),
            (int)(FROM_CM(.8) - nPaperMarginW),
            (int)(nCenter + FROM_CM(7.4) / 2 + FROM_CM(.6)));
    }

    SelectObject(pd.hDC, hOldBrush);
    SelectObject(pd.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // ページ番号の出力
    lf.lfHeight = FROM_POINT(8);
    lf.lfWeight = 400;
    lf.lfItalic = lf.lfUnderline = lf.lfStrikeOut = FALSE;
    strcpy(&lf.lfFaceName[0], FN_MSPG);

    hFont = CreateFontIndirect(&lf);
    hOldFont = SelectObject(pd.hDC, hFont);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        rc.left = nDPIW / 2 - nPaperMarginW;
    }
    else {                                      // 縦置きの場合
        rc.left = nDPIW - nPaperMarginW;
    }
    rc.top = nPaperHeight - nDPIH / 2 - nPaperMarginH + FROM_POINT(2);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
    rc.bottom = rc.top +FROM_POINT(10);

    t = 64;
    if (GetUserName(szBuf, &t)) {
        strcat(szBuf, " - ");
    }
    else {
        szBuf[0] = '\0';
    }

    t = time(NULL);
    sprintf(szBuf, "Printout time: %s", ctime(&t));
    szBuf[strlen(szBuf) - 1] = '\0';
    strcat(szBuf, " - ");

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        sprintf(szBuf + strlen(szBuf), "Page: %d", (nPageNo - 1) / 2 + 1);
        nPageNo++;
    }
    else {
        sprintf(szBuf + strlen(szBuf), "Page: %d", nPageNo++);
    }

    DrawText(pd.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);

    // Copyright表示
    rc.top = nPaperHeight - nDPIH / 2 - nPaperMarginH + FROM_POINT(2);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
    rc.bottom = rc.top +FROM_POINT(10);

    lf.lfHeight = FROM_POINT(5);
    hFont = CreateFontIndirect(&lf);
    hOldFont = SelectObject(pd.hDC, hFont);

    DrawText(pd.hDC, "&Messaging in the &Emacs &World(Mew)の為に作ったmpr/"
             TIME_STAMP
             "\r\n"
             "Copyright 1999 By T.Furukawa"
             "(tfuruka1@vdnet.or.jp, HGA00616@nifty.ne.jp)", -1, &rc,
             DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_WORDBREAK);

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);

    return TRUE;
Error:
    // Delete the printer DC.
    DeleteDC(pd.hDC);
    if (szErr) {
        MessageBox(hwnd, szErr, TEXT("bFinePrinter"), MB_ICONSTOP);
    }
    return TRUE;
}

static BOOL
bWritePrinterChar(
    HWND hwnd,
    LPTSTR szBuf,
    int cbString
    )
{
    HFONT hFont, hOldFont;
    SIZE Size;

    if ('\r' == *szBuf || '\n' == *szBuf) {
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;
        return TRUE;
    }

    if (nCurrentX > nEndX) {                    // 幅を超えている
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;
    }

    if (0x80 < *szBuf || 0 > *szBuf) {
        TCHAR szTmp[4];
        memcpy(szTmp, szBuf, 2);
        szTmp[2] = '\0';
        if (strstr(szKeisen, szTmp)) {
            bKeisen = TRUE;
        }
        bKanji = TRUE;
    }

    hFont = CreateFontIndirect(&lf2);
    hOldFont = SelectObject(pd.hDC, hFont);

    TextOut(pd.hDC, nCurrentX, nCurrentY, szBuf, cbString);
    GetTextExtentPoint32(pd.hDC, szBuf, cbString, &Size);
    nCurrentX += Size.cx;

    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);
    return TRUE;
}

static BOOL
bWritePrinter(
    HWND hwnd,
    LPTSTR szBuf
    )
{
    HFONT hFont, hOldFont;
    SIZE Size;
    TCHAR szTmp[4];
    int cbString = 1, i;

    if (!nCurrentY) {
        if (!bBackDrawPrinter(hwnd)) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = FALSE;
        bKeisen = FALSE;
    }

    if (nCurrentX > nEndX) {                    // 幅を超えている
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;
    }

    if (nCurrentY > nEndY) {                    // 最終ラインを超えている
        // 縦置きの場合又は偶数ページの場合
        if (nPaperWidth < nPaperHeight || 1 == (nPageNo % 2)) {
            if (!bEndPage(hwnd)) {
                return FALSE;
            }
        }
        if (!bBackDrawPrinter(hwnd)) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = bKeisen = FALSE;
    }

    // 纏めて出力できるかどうか確かめる
    szTmp[0] = '\0';
    if ('\n' == *(szBuf + strlen(szBuf) - 1) ||
        '\r' == *(szBuf + strlen(szBuf) - 1)) {
        szTmp[0] = '\n';
        *(szBuf + strlen(szBuf) - 1) = '\0';
    }
    hFont = CreateFontIndirect(&lf2);
    hOldFont = SelectObject(pd.hDC, hFont);

    GetTextExtentPoint32(pd.hDC, szBuf, strlen(szBuf), &Size);
    if ((nCurrentX + Size.cx) < nEndX) {
        TextOut(pd.hDC, nCurrentX, nCurrentY, szBuf, strlen(szBuf));
        nCurrentX += Size.cx;
        SelectObject(pd.hDC, hOldFont);
        DeleteObject(hFont);

        for (i = 0; i < (int)strlen(szBuf); i++) {
            if (0x80 < *(szBuf + i) || 0 > *(szBuf + i)) {
                TCHAR szTmp[4];
                memcpy(szTmp, szBuf + i, 2);
                szTmp[2] = '\0';
                if (strstr(szKeisen, szTmp)) {
                    bKeisen = TRUE;
                }
                bKanji = TRUE;
                break;
            }
        }
        if (szTmp[0]) {
            bWritePrinterChar(hwnd, szTmp, 1);
        }
        return TRUE;
    }
    SelectObject(pd.hDC, hOldFont);
    DeleteObject(hFont);

    if (szTmp[0]) {
        strcat(szBuf, "\n");
    }
    // 纏めて出力できなかったのでちまちまと出力する
    for (; *szBuf; szBuf++) {
        szTmp[0] = *szBuf;
        cbString = 1;
        if (0 > szTmp[0] || 0x80 < szTmp[0]) {
            szTmp[1] = *(szBuf + 1);
            szBuf++;
            cbString = 2;
        }
        if (!bWritePrinterChar(hwnd, szTmp, cbString)) {
            return FALSE;
        }
    }
    return TRUE;
}

LPTSTR
ufgets(
    LPTSTR szBuf,
    int cbMax,
    FILE *fp
    )
{
    int i, j, c;

    for (i = 0; i < cbMax - 1; i++) {
        if (EOF == (c = getc(fp))) break;
        if ('\t' == c) {
            for (j = 0; j < (8 - (i % 8)); j++) {
                *szBuf++ = ' ';
            }
            i += (8 - (i % 8));
            i--;
            continue;
        }
        *szBuf++ = c;
        if ('\n' == c) {
            i++;
            break;
        }
    }
    *szBuf = '\0';

    if (!i) {
        return NULL;
    }
    return szBuf;
}

void
PrintoutFile(
    HWND hwnd,
    LPTSTR FileName
    )
{
    static struct {
        LPTSTR szHeader;                        // ヘッダ文字列
        int nWeight;                            // ウエイト
        BOOL bUnderLine;                        // 下線
    } Header[] = {
        {"Subject: ", 400, TRUE},
        {"Message-ID: ", 400, TRUE},
        {"From: ", 400, TRUE},
        {"Reply-To: ", 400, TRUE},
        {"References: ", 400, TRUE},
        {NULL, 0, FALSE}};
    BOOL bHeader = TRUE;
    TCHAR szBuf[1024], szLastHeader[128];
    LPTSTR p, p1;
    FILE *fp;
    int bTitle = FALSE, c, i;

    strcpy(szTitleStr, "ほげほげ");
    strcpy(g_szMessageID, "Message-Id: わかりまへん");
    strcpy(g_szReference, " ");
    strcpy(g_szFrom, "From: 匿名希望");
    strcpy(g_szDate, "Date: 昭和40年10月28日");

    szLastHeader[0] = '\0';

    if (NULL == (fp = fopen(FileName, "rt"))) {
        return;
    }

    if (!bInitPrinter(hwnd)) {
        goto ErrorExit;
    }

    while (ufgets(szBuf, 1024, fp)) {
        lf2.lfHeight = nBasePoint;
        lf2.lfWeight = 400;
        lf2.lfItalic = lf2.lfUnderline = lf2.lfStrikeOut = FALSE;
        strcpy(&lf2.lfFaceName[0], FN_MSM);

        // 空行の場合はMHS終了
        if ('\r' == szBuf[0] || '\n' == szBuf[0]) {
            bHeader = FALSE;
        }

        // ヘッダの場合
        if (bHeader) {
            // タイトルの場合はタイトル設定
            if (0 == memicmp("Subject: ", szBuf, 9)) {
                strncpy(szTitleStr, szBuf + 9, 76);
                szTitleStr[strlen(szTitleStr) - 1] = '\0';
            }

            // メッセージIDの場合はメッセージIDを設定
            else if (0 == strnicmp("Message-Id:", szBuf, 11)) {
                strcpy(g_szMessageID, szBuf);
                g_szMessageID[strlen(g_szMessageID) - 1] = '\0';
            }

            // Fromの場合
            else if (0 == strnicmp(SZ_FROM, szBuf, strlen(SZ_FROM))) {
                strcpy(g_szFrom, szBuf);
                g_szFrom[strlen(g_szFrom) - 1] = '\0';
            }
            // Dateの場合
            else if (0 == strnicmp(SZ_DATE, szBuf, strlen(SZ_DATE))) {
                strcpy(g_szDate, szBuf);
                g_szDate[strlen(g_szDate) - 1] = '\0';
            }

            // リファレンスの場合はリファレンスを設定
            else if (0 == strnicmp(SZ_REFERENCES, szBuf,
                                   strlen(SZ_REFERENCES)) ||
                     0 == strnicmp(SZ_IN_REPLY_TO, szBuf,
                                   strlen(SZ_IN_REPLY_TO))) {
                strcpy(g_szReference, szBuf);
                g_szReference[strlen(g_szReference) - 1] = '\0';
            }

            // ヘッダの印字
            strcpy(&lf2.lfFaceName[0], FN_MSPG);
            p1 = strchr(szBuf, ' ');
            if ((p = strchr(szBuf, ':')) && (NULL == p1 ||
                                             (ULONG)p < (ULONG)p1)) {
                if (' ' == *(p + 1) || '\t' == *(p + 1)) {
                    p++;
                }
                c = *(p + 1);
                *(p + 1) = '\0';
                lf2.lfWeight = 700;
                TrimRight(strcpy(szLastHeader, szBuf));
            }
            else {
                p = szBuf;
                lf2.lfWeight = 400;
            }
#if 1
            if (0 == stricmp(SZ_RECEIVED, szLastHeader)) {
                continue;
            }
#endif

#if 1
            if (0 == memicmp("X-", szLastHeader, 2)) {
                continue;
            }
#endif
            // ヘッダのタイトル印字
            if (!bWritePrinter(hwnd, szBuf)) {
                goto ErrorExit;
            }
            // : が見つからなかった場合は次の行へ
            if (p == szBuf) {
                // リファレンスの継続
                if (0 == strnicmp(SZ_REFERENCES, szLastHeader,
                                  strlen(SZ_REFERENCES)) ||
                    0 == strnicmp(SZ_IN_REPLY_TO, szLastHeader,
                                  strlen(SZ_IN_REPLY_TO))) {
                    strcat(strcat(g_szReference, " "),
                           TrimString(szBuf));
                }
                continue;
            }
            *(p + 1) = c;
            p++;
            // ハイライトヘッダが含まれているかを検索
            for (i = 0; Header[i].szHeader; i++) {
                if (0 == memicmp(Header[i].szHeader, szBuf,
                                strlen(Header[i].szHeader))) {
                    // 必要ならアンダーラインを付加してヘッダの内容を表示
                    lf2.lfWeight = Header[i].nWeight;
                    lf2.lfUnderline = Header[i].bUnderLine;
                    if (!bWritePrinter(hwnd, p)) {
                        goto ErrorExit;
                    }
                    break;
                }
            }
            if (!Header[i].szHeader) {          // ハイライトヘッダではなかった
                lf2.lfWeight = 400;
                if (!bWritePrinter(hwnd, p)) {
                    goto ErrorExit;
                }
            }
            continue;                           // 次の行を処理する
        }

        // 引用記号が含まれている場合
        if (p1 = _mbschr(szBuf, '>')) {
            p = _mbschr(szBuf, '<');
            if (NULL == p || p1 < p) {
                lf2.lfWeight = 700;
            }
        }

        if ((p = strstr(szBuf, "http:")) || (p = strstr(szBuf, "mailto:"))) {
            p = szBuf;
            while ((p1 = strstr(p, "http:")) ||
                   (p1 = strstr(p, "mailto:"))) {
                if (p != p1) {
                    c = *p1;
                    *p1 = '\0';
                    strcpy(&lf2.lfFaceName[0], FN_MSM);
                    lf2.lfUnderline = FALSE;
                    if (!bWritePrinter(hwnd, p)) {
                        goto ErrorExit;
                    }
                    *p1 = c;
                }
                if ((p = strchr(p1, ' ')) || (p = strchr(p1, '\t')) ||
                    (p = strchr(p1, '\n'))) {
                    c = *p;
                    *p = '\0';
                    lf2.lfUnderline = TRUE;
                    strcpy(&lf2.lfFaceName[0], FN_MSPG);
                    if (!bWritePrinter(hwnd, p1)) {
                        goto ErrorExit;
                    }
                    *p = c;
                }
                else {
                    lf2.lfUnderline = TRUE;
                    strcpy(&lf2.lfFaceName[0], FN_MSPG);
                    if (!bWritePrinter(hwnd, p1)) {
                        goto ErrorExit;
                    }
                    break;
                }
            }
            if (*p) {
                strcpy(&lf2.lfFaceName[0], FN_MSM);
                lf2.lfUnderline = FALSE;
                if (!bWritePrinter(hwnd, p)) {
                    goto ErrorExit;
                }
            }
        }
        else {
            if (!bWritePrinter(hwnd, szBuf)) {
                goto ErrorExit;
            }
        }
    }
    bFinePrinter(hwnd);
ErrorExit:
    fclose(fp);
}

APIENTRY
WinMain(
    HINSTANCE hInstance,                        // 現在のインスタンスハンドル
    HINSTANCE hPrevInstance,                    // 以前のインスタンスハンドル
    LPSTR lpszCmdLine,                          // コマンド ラインアドレス
    int nCmdShow                                // ウィンドウの表示状態
    )
{
    LPTSTR lpszFileName;
    TCHAR szBuf[1024];

    switch (__argc) {
    case 2: case 3:
        lpszFileName = *(__argv + 1);
        break;
    default:
        wsprintf(szBuf, "Usage: %s [<FileName> [DEL]]", *(__argv));
        MessageBox(NULL, szBuf, NULL, MB_ICONERROR | MB_SETFOREGROUND);
        return 1;
    }

    PrintoutFile(NULL, lpszFileName);

    if (3 == __argc && 0 == stricmp("DEL", *(__argv + 2))) {
        unlink(*(__argv + 1));
    }

    return 0;
}
