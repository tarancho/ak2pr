/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-02-27 22:48:45 tfuruka1>
 *
 * 「ak2psのようなもの」のプリンタ制御関連
 *
 * $Id: printer.c,v 1.5 2003/03/01 09:04:09 tfuruka1 Exp $
 * $Log: printer.c,v $
 * Revision 1.5  2003/03/01 09:04:09  tfuruka1
 * ●本文以外の定形印刷部分の印刷文字色がボディ部の影響を受ける場合があっ
 * たのを修正。
 *
 * Revision 1.4  2001/12/23 10:10:33  tfuruka1
 * ●プレビュー有りの判断方法を変更した。
 * ●Copyright表示を印字しないモードを用意した。
 *
 * Revision 1.3  2001/12/18 12:58:41  tfuruka1
 * 段の幅を超えて文字を印字してしまう事がある問題を修正。この問題のデバッ
 * グを行う目的で、デバッグ用のチェックボックスを印刷設定の画面に追加した。
 *
 * Revision 1.2  2001/12/18 04:06:40  tfuruka1
 * プレビューに対応しました。
 *
 * Revision 1.1  2001/02/05 17:46:22  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

// フォントのテンプレート
static LOGFONT lft = {-13, 0, 0, 0, 800, 0, 0, 0, SHIFTJIS_CHARSET,
                      3, 2, 1, 49, TEXT("ＭＳ ゴシック")};

// プリンタの固有情報
static int nPaperWidth, nPaperHeight;           // 用紙のサイズ
static int nDPIW, nDPIH;                        // 解像度
static int nPaperMarginW, nPaperMarginH;        // 印刷不可エリア
static int nPageNo;                             // ページ番号
static int nBasePoint, nBaseLineK, nBaseLine;   // ベースライン
static int nCurrentX, nCurrentY;                // 現在の座標
static int nStartX, nEndX;                      // 水平方向の範囲
static int nStartY, nEndY;                      // 垂直方向の範囲
static BOOL bKeisen, bKanji;                    // 罫線及び漢字フラグ
static BOOL bPreviewed = FALSE;                 // T:プレビュー済み
// 罫線コード表
static LPTSTR szKeisen = {"│┃││┃┃┃┃─┘┛┐┤┨┓┨┨━┛┛┓"
                          "┥┫┓┫┫─└┗┌├┠┏┠┠─┴┸┬┼╂┰"
                          "╂╂━┷┻┯┿╋┳╋╋━┗┗┏┝┣┏┣┣━"
                          "┷┻┯┿╋┳╋╋━┷┻┯┿╋┳╋╋"};

/* -------------------------------------------------------------------
 * を解像度nDpiの時の長さvalueをピクセル数へ変換する。valueの単位はflg
 * で指定する。CX_PT(ポイント), CX_CM(センチメートル), CX_MM(ミリメー
 * トル)を指定出来る。
 * *-----------------------------------------------------------------*/
int ConvX2Dt(double value, int nDpi, int flg)
{
    switch (flg) {
    case CX_PT:
        return (int)(value * nDpi / 72.0);
    case CX_CM:
        return (int)(value * nDpi / 2.54);
    case CX_MM:
        return (int)(value * nDpi / 25.4);
    default:
        MessageBox(NULL, "変換フラグが不正です", "ConvX2Dt()",
                   MB_ICONSTOP | MB_SETFOREGROUND);
        return 0;
    }
}

/* -------------------------------------------------------------------
 * ConvdDtの逆を行う。
 * *-----------------------------------------------------------------*/
double ConvDt2X(int value, int nDpi, int flg)
{
    switch (flg) {
    case CX_PT:
        return value * 72.0 / nDpi;
    case CX_CM:
        return value * 2.54 / nDpi;
    case CX_MM:
        return value * 25.4 / nDpi;
    default:
        MessageBox(NULL, "変換フラグが不正です", "ConvDt2X()",
                   MB_ICONSTOP | MB_SETFOREGROUND);
        return 0;
    }
}

int GetPrtDpiW(void)
{
    return nDPIW;
}

int GetPrtDpiH(void)
{
    return nDPIH;
}

int GetPrtWd(void)
{
    return nPaperWidth;
}

int GetPrtHt(void)
{
    return nPaperHeight;
}

int GetPrtMgW(void)
{
    return nPaperMarginW;
}

int GetPrtMgH(void)
{
    return nPaperMarginH;
}

int GetPrtBasePoint(void)
{
    return nBasePoint;
}

int GetPrtCenter(int flg)
{
    int nCenter;

    if (flg & GPC_W) {
        nCenter = nPaperWidth / 2;
        if (!(flg & GPC_IM)) {
            nCenter -= nPaperMarginW;
        }
    }
    else {
        nCenter = nPaperHeight / 2;
        if (!(flg & GPC_IM)) {
            nCenter -= nPaperMarginH;
        }
    }
    return nCenter;
}

BOOL
SetupPrinter(
    HWND hWnd,                                  // ハンドル
    HGLOBAL *lphDevNames,                       // デバイスネーム
    HGLOBAL *lphDevMode                         // デバイスモード
    )
{
    PRINTDLG pd;                                // 印刷ダイアログ
    BOOL bRet = FALSE;

    EnterCriticalSection(&g_csCriticalSection);// ロック
    DbgPrint(NULL, 'I', "★ロック");

    DbgPrint(NULL, 'I', "プリンタ設定開始");

    // [印刷]ダイアログ ボックスを初期化
    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hWnd;
    pd.hDevMode = (HANDLE)*lphDevMode;
    pd.hDevNames = (HANDLE)*lphDevNames;
    pd.hDC = (HDC)NULL;
    pd.Flags = PD_PRINTSETUP;
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

    /* 印刷ダイアログボックスを表示する */
    if (!PrintDlg(&pd)) {                       // Cancelを押した場合
        DbgPrint(NULL, 'W', "キャンセルボタンが押下されました");
        goto Exit;
    }

    *lphDevNames = pd.hDevNames;
    *lphDevMode = pd.hDevMode;

    SetDefaultPrtInfo();                        // デバイス情報書き込み

    DbgPrint(NULL, 'I', "プリンタ設定終了");

    bRet = TRUE;
 Exit:
    DbgPrint(NULL, 'I', "★ロック解除");
    LeaveCriticalSection(&g_csCriticalSection); // ロック解除

    return bRet;
}

/* -------------------------------------------------------------------
 * プリンタの初期設定を行う
 * *-----------------------------------------------------------------*/
BOOL
BeginDocument(void)
{
    static DOCINFO di;
    TCHAR szBuf[512];

    DbgPrint(NULL, 'B', "プリンタ初期化開始");

    wsprintf(szBuf, "(^_^)%s T.Furukawa",
             !g_MailBox.PrtInfo.szFileName[0] ? "テスト印字" :
             (g_MailBox.PrtInfo.szTitle[0] ? g_MailBox.PrtInfo.szTitle :
              "e-mail?"));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = szBuf;
    di.lpszOutput = (LPTSTR)NULL;

    if (!g_MailBox.PrtInfo.bPreView) {
        // プリンタ出力の場合
        if (SP_ERROR == StartDoc(g_MailBox.hDC, &di)) {
            int nErr = GetLastError();
            DbgPrint(NULL, 'E', "%s",
                     GetLastErrorMessage(__FILE__ " StartDoc()", nErr));
            return FALSE;
        }
        nPaperWidth = GetDeviceCaps(g_MailBox.hDC, PHYSICALWIDTH);
        nPaperHeight = GetDeviceCaps(g_MailBox.hDC, PHYSICALHEIGHT);
        nPaperMarginW = GetDeviceCaps(g_MailBox.hDC, PHYSICALOFFSETX);
        nPaperMarginH = GetDeviceCaps(g_MailBox.hDC, PHYSICALOFFSETY);

        nDPIW = GetDeviceCaps(g_MailBox.hDC, LOGPIXELSX);
        nDPIH = GetDeviceCaps(g_MailBox.hDC, LOGPIXELSY);

    }
    else {
        // プレビューの場合
        nPaperWidth = g_MailBox.PrevInfo.wd;
        nPaperHeight = g_MailBox.PrevInfo.ht;
        nPaperMarginW = g_MailBox.PrevInfo.xoff;
        nPaperMarginH = g_MailBox.PrevInfo.yoff;

        nDPIW = g_MailBox.PrevInfo.dpiW;
        nDPIH = g_MailBox.PrevInfo.dpiH;

        bPreviewed = FALSE;                     // プレビュー未に設定
    }

    DbgPrint(NULL, 'I', "デバイス情報\n"
             "サイズ  : %d×%ddot→%f×%fcm\n"
             "マージン: %ddot(Left), %ddot(Top)→%f, %fcm\n"
             "解像度  : %d×%ddpi",
             nPaperWidth, nPaperHeight,
             nPaperWidth * 2.54 / nDPIW, nPaperHeight * 2.54 / nDPIH,
             nPaperMarginW, nPaperMarginH,
             nPaperMarginW * 2.54 / nDPIW, nPaperMarginH * 2.54 / nDPIH,
             nDPIW, nDPIH);

    nCurrentX = nCurrentY = 0;
    nPageNo = 1;
    bKanji = FALSE;
    bKeisen = FALSE;

    // ベースラインスキップ等の初期設定を行う
    nBasePoint = ConvX2Dt(g_MailBox.PrtInfo.fFontSize, nDPIH, CX_PT);
    nBaseLineK = ConvX2Dt(g_MailBox.PrtInfo.fFontSize * 1.5, nDPIH, CX_PT);
    nBaseLine = ConvX2Dt(g_MailBox.PrtInfo.fFontSize * 1.2, nDPIH, CX_PT);

    // テキストのバックモード(透過)
    SetBkMode(g_MailBox.hDC, TRANSPARENT);

    DbgPrint(NULL, 'B', "プリンタ初期化完了");
    return TRUE;
}

HPEN
CreatePrtPen(
    UINT nStyle,                                // ペンのスタイル
    LONG nWidth,                                // 太さ
    COLORREF crColor                            // 色
    )
{
    LOGPEN lgpen;                               // 論理ペン

    lgpen.lopnStyle = nStyle;
    lgpen.lopnWidth.x = nWidth;
    lgpen.lopnColor = crColor;

    return CreatePenIndirect(&lgpen);
}

HFONT
CreatePrtFont(
    LPTSTR lpszFontName,                        // フォント名
    int nHeight,                                // フォントの高さ
    int nWeight,                                // フォントの太さ
    BOOL bItalic,                               // T: イタリック
    BOOL bUnderline,                            // T: 下線
    BOOL bStrikeOut,                            // T: 打ち消し線
    BOOL bSjis                                  // T: 日本語
    )
{
    LOGFONT lf;

    memcpy(&lf, &lft, sizeof(LOGFONT));
    lf.lfHeight = nHeight;
    lf.lfWeight = nWeight;
    lf.lfItalic = bItalic;
    lf.lfUnderline = bUnderline;
    lf.lfStrikeOut = bStrikeOut;
    lf.lfCharSet = bSjis ? SHIFTJIS_CHARSET : ANSI_CHARSET;
    strcpy(&lf.lfFaceName[0], lpszFontName);

    return CreateFontIndirect(&lf);
}

/* -------------------------------------------------------------------
 * ページ出力を開始する
 * *-----------------------------------------------------------------*/
BOOL
BeginPage(void)
{
    static int nLeft, nRight;
    time_t t;
    HPEN hPen, hOldPen;
    HBRUSH hBrush, hOldBrush;
    HFONT hFont, hOldFont;
    POINT pt[3];
    RECT rc;
    int nError, mod, i, nCenter;
    LPTSTR szErr = NULL;
    TCHAR szBuf[512];

    SetTextColor(g_MailBox.hDC, RGB(0, 0, 0));

    // n段組みの最初の段以外(即ち一番左側の段以外)の時は座標の更新だけ行う
    if (0 != (mod = ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp))) {
        double fWidth = (double)(nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp;
        nStartX = (int)(nLeft + fWidth * mod);
        nEndX = (int)(nStartX + fWidth);
        nStartX += ConvX2Dt(5, nDPIW, CX_PT);
        nEndX -= ConvX2Dt(5, nDPIW, CX_PT);
        nPageNo++;
        goto Exit;
    }
    // ----------------------------------------------
    // 以下は一番最初の段の印刷開始時のみ実行されます
    // ----------------------------------------------
    if (!g_MailBox.PrtInfo.bPreView) {
        // プレビューの場合はこのブロック内の処理は行わない
        nError = StartPage(g_MailBox.hDC);
        if (nError <= 0) {
            DbgPrint(NULL, 'E', "%s", 
                     GetLastErrorMessage("StartPage()", GetLastError()));

            return FALSE;
        }
    }

    rc.top = rc.left = 0;
    rc.right = nPaperWidth;
    rc.bottom = nPaperHeight;
    FillRect(g_MailBox.hDC, &rc, GetStockObject(WHITE_BRUSH));

    // 外枠の描画
    hPen = CreatePrtPen(PS_SOLID, (int)(nDPIW / 72.0 + 1), RGB(0, 0, 0));
    hBrush = CreateSolidBrush(RGB(255, 255, 255));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    hOldBrush = SelectObject(g_MailBox.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        RoundRect(g_MailBox.hDC,
                  nStartX = nDPIW / 2 - nPaperMarginW,
                  nStartY = nDPIH - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }
    else {                                      // 縦置きの場合
        RoundRect(g_MailBox.hDC,
                  nStartX = nDPIW - nPaperMarginW,
                  nStartY = nDPIH / 2 - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }

    // タイトルと本文の仕切り線
    nStartY += ConvX2Dt(20, nDPIH, CX_PT);
    pt[0].x = nLeft = nStartX;
    pt[0].y = pt[1].y = nStartY;
    pt[1].x = nRight = nEndX;
    Polyline(g_MailBox.hDC, &pt[0], 2);

    // ページの仕切り線を描画する
    for (i = 1; i < g_MailBox.PrtInfo.nNumOfUp; i++) {
        double fWidth = (nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp * i;
        pt[0].x = pt[1].x = (int)(fWidth + nLeft);
        pt[0].y = nStartY;
        pt[1].y = nEndY;
        Polyline(g_MailBox.hDC, &pt[0], 2);
    }
    nEndX = (int)(nStartX + (nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp
                  - ConvX2Dt(5, nDPIW, CX_PT));
    nStartX += ConvX2Dt(5, nDPIW, CX_PT);
    nStartY += ConvX2Dt(5, nDPIH, CX_PT);
    nEndY -= ConvX2Dt(5, nDPIH, CX_PT);

    SelectObject(g_MailBox.hDC, hOldBrush);
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // 綴じ位置を描画する
    hPen = CreatePrtPen(PS_SOLID, 1, RGB(0, 0, 0));
    hBrush = CreateSolidBrush(RGB(0, 0, 0));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    hOldBrush = SelectObject(g_MailBox.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        pt[0].x = nCenter = GetPrtCenter(GPC_W);
        pt[1].y = pt[2].y = ConvX2Dt(10, nDPIH, CX_PT);
        pt[0].y = 0;
        pt[1].x = pt[0].x + ConvX2Dt(5, nDPIW, CX_PT);
        pt[2].x = pt[0].x - ConvX2Dt(5, nDPIW, CX_PT);
    }
    else {                                      // 縦置きの場合
        pt[0].x = 0;
        pt[1].x = pt[2].x = ConvX2Dt(10, nDPIW, CX_PT);
        pt[0].y = nCenter = GetPrtCenter(GPC_H);
        pt[1].y = pt[0].y + ConvX2Dt(5, nDPIH, CX_PT);
        pt[2].y = pt[0].y - ConvX2Dt(5, nDPIH, CX_PT);
    }
    Polygon(g_MailBox.hDC, &pt[0], 3);

    // 2 HOLE PUNCHI を描画する。但し実際の正しい規格(JIS?)を私は知り
    // ません。物差しで計って, 穴と穴の距離を8cm, 穴の直径を5.5mm, 用
    // 紙の端から穴の頂点迄の距離を1cmとして描画しています。
    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        double fbtHole = ConvX2Dt(8, nDPIW, CX_CM);
        double f2rHoleW = ConvX2Dt(.55, nDPIW, CX_CM);
        double f2rHoleH = ConvX2Dt(.55, nDPIH, CX_CM);
        double fTopMg = ConvX2Dt(1, nDPIH, CX_CM) - nPaperMarginH;
        Arc(g_MailBox.hDC,
            (int)(nCenter - fbtHole / 2 - f2rHoleW / 2),
            (int)(fTopMg),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2));
        Arc(g_MailBox.hDC,
            (int)(nCenter + fbtHole / 2 - f2rHoleW / 2),
            (int)(fTopMg),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg  + f2rHoleH),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg  + f2rHoleH / 2),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2));
    }
    else {
        double fbtHole = ConvX2Dt(8, nDPIH, CX_CM);
        double f2rHoleW = ConvX2Dt(.55, nDPIW, CX_CM);
        double f2rHoleH = ConvX2Dt(.55, nDPIH, CX_CM);
        double fLeftMg = ConvX2Dt(1, nDPIW, CX_CM) - nPaperMarginW;
        Arc(g_MailBox.hDC,
            (int)(fLeftMg),
            (int)(nCenter - fbtHole / 2 - f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2 + f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2));
        Arc(g_MailBox.hDC,
            (int)(fLeftMg),
            (int)(nCenter + fbtHole / 2 - f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2 + f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2));
    }

    SelectObject(g_MailBox.hDC, hOldBrush);
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // ページ番号の出力
    hFont = CreatePrtFont(FN_ARIAL, ConvX2Dt(8, nDPIH, CX_PT),
                          400, FALSE, FALSE, FALSE, FALSE);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);

    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        rc.left = nDPIW / 2 - nPaperMarginW;
    }
    else {                                      // 縦置きの場合
        rc.left = nDPIW - nPaperMarginW;
    }
    rc.top = nPaperHeight - nDPIH / 2 - nPaperMarginH
        + ConvX2Dt(2, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(10, nDPIH, CX_PT);

    t = 64;
    if (GetUserName(szBuf, &t)) {
        strcat(szBuf, " - ");
    }
    else {
        szBuf[0] = '\0';
    }

    t = time(NULL);
    sprintf(szBuf + strlen(szBuf), "Printout time: %s", ctime(&t));
    szBuf[strlen(szBuf) - 1] = '\0';
    strcat(szBuf, " - ");

    sprintf(szBuf + strlen(szBuf), "Page: %d",
            (nPageNo - 1) / g_MailBox.PrtInfo.nNumOfUp + 1);
    nPageNo++;

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    // Copyright表示
    if (!g_PrtInfo.bNoCopyright) {
        hFont = CreatePrtFont(FN_ARIAL, ConvX2Dt(3, nDPIH, CX_PT),
                              400, FALSE, FALSE, FALSE, FALSE);
        hOldFont = SelectObject(g_MailBox.hDC, hFont);

        DrawText(g_MailBox.hDC, COPYRIGHT, -1, &rc,
                 DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_WORDBREAK);

        SelectObject(g_MailBox.hDC, hOldFont);
        DeleteObject(hFont);
    }

 Exit:
    // デバッグモードの場合は、印刷エリアを塗り潰す
    if (g_PrtInfo.bDebug) {
        RECT rc;
        rc.left = nStartX;
        rc.right = nEndX;
        rc.top = nStartY;
        rc.bottom = nEndY;
        DrawRect(g_MailBox.hDC, &rc, RGB(0, 0, 255), PS_SOLID);
    }

    return TRUE;
}

/* -------------------------------------------------------------------
 * 用紙を排紙する
 * *-----------------------------------------------------------------*/
BOOL EndPageDocument(void)
{
    HFONT hFont, hOldFont;
    RECT rc, rc2, rcBase;
    TCHAR szBuf[768];
    int nHt;

    // タイトルの出力
    hFont = CreatePrtFont(FN_MSPG, ConvX2Dt(18, nDPIH, CX_PT),
                          700, FALSE, FALSE, FALSE, TRUE);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);
    if (nPaperWidth > nPaperHeight) {           // 横置きの場合
        rc.left = nDPIW / 2 - nPaperMarginW;
        rc.top = nDPIH - nPaperMarginH;
    }
    else {                                      // 縦置きの場合
        rc.left = nDPIW - nPaperMarginW;
        rc.top = nDPIH / 2 - nPaperMarginH;
    }
    rc.bottom = rc.top + ConvX2Dt(20, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;

    memcpy(&rcBase, &rc, sizeof(RECT));

    rc.left += ConvX2Dt(10, nDPIW, CX_PT);
    rc.right -= ConvX2Dt(10, nDPIW, CX_PT);
    DrawText(g_MailBox.hDC, g_MailBox.PrtInfo.szTitle, -1, &rc,
             DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    // 印刷対象がメールの場合のみ
    if (PT_MAIL == g_MailBox.PrtInfo.nType) {
        // Fromと受信日時の表示
        sprintf(szBuf, "%s\r\n%s", g_MailBox.szFrom, g_MailBox.szDate);
        hFont = CreatePrtFont(FN_MSPG, ConvX2Dt(8, nDPIH, CX_PT),
                              800, FALSE, FALSE, FALSE, TRUE);
        hOldFont = SelectObject(g_MailBox.hDC, hFont);

        memcpy(&rc2, &rc, sizeof(RECT));
        nHt = DrawText(g_MailBox.hDC, szBuf, -1, &rc2,
                       DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK | DT_CALCRECT);
        rc.top += (ConvX2Dt(20, nDPIH, CX_PT) - nHt) / 2;
        rc.bottom = rc.top + nHt;
        DrawText(g_MailBox.hDC, szBuf, -1, &rc,
                 DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

        // リファレンスの表示とメッセージIDの表示
        memcpy(&rc, &rcBase, sizeof(RECT));
        rc.top -= ConvX2Dt(18, nDPIH, CX_PT);
        rc.bottom = rc.top + ConvX2Dt(16, nDPIH, CX_PT);
        sprintf(szBuf, "%s\r\n%s",
                g_MailBox.szReference, g_MailBox.szMessageID);
        DrawText(g_MailBox.hDC, szBuf, -1, &rc,
                 DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

        SelectObject(g_MailBox.hDC, hOldFont);
        DeleteObject(hFont);
    }

    // プレビューの場合ここで、プレビュー画面を表示する
    if (g_MailBox.PrtInfo.bPreView) {
        if (!bPreviewed) {
            PrintPreview(g_MailBox.hWnd, &g_MailBox.PrevInfo);
        }
        bPreviewed = TRUE;                      // プレビュー済みに設定
        return FALSE;
    }
    if (0 >= EndPage(g_MailBox.hDC)) {
        int nErr = GetLastError();
        MessageBox(g_MailBox.hWnd, GetLastErrorMessage("EndPage()", nErr),
                   "EndPageDocument()", MB_ICONSTOP | MB_SETFOREGROUND);
        DbgPrint(NULL, 'E', "EndPageDocument(): %s",
                 GetLastErrorMessage("EndPage()", nErr));
        return FALSE;
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * ドキュメントの印刷を完了する
 * *-----------------------------------------------------------------*/
BOOL
EndDocument(void)
{
    int nErr;

    EndPageDocument();
    if (g_MailBox.PrtInfo.bPreView) {
        return FALSE;
    }

    nErr = EndDoc(g_MailBox.hDC);
    if (nErr <= 0) {
        MessageBox(g_MailBox.hWnd, GetLastErrorMessage("EndDoc()", nErr),
                   TEXT("FinePrinter"), MB_ICONSTOP);
        DbgPrint(NULL, 'E', "FilePrinter(): %s",
                 GetLastErrorMessage("EndDoc()", nErr));
        return FALSE;
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * 一文字プリンタへ出力する。この関数は外部から呼び出されることは無い。
 * フォントは既に選択されている必要がある。
 * *-----------------------------------------------------------------*/
static BOOL
PutcPrinter(
    LPTSTR szBuf,                               // 出力文字
    int cbString                                // 出力文字数
    )
{
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

    // 印字した結果、幅を超えないか調べる
    GetTextExtentPoint32(g_MailBox.hDC, szBuf, cbString, &Size);
    if ((nCurrentX + Size.cx) > nEndX) {        // 幅を超えている
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;

        // 改行した結果、最後の行を超える場合は、改ページする
        if ((nCurrentY + Size.cy) > nEndY) {
            // 最後の段の場合は排紙する
            if (0 == ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp)) {
                if (!EndPageDocument()) {
                    return FALSE;
                }
            }
            if (!BeginPage()) {
                return FALSE;
            }
            nCurrentX = nStartX;
            nCurrentY = nStartY;
            bKanji = bKeisen = FALSE;
        }
    }

    // メールの印刷時のみ
    if (PT_MAIL == g_MailBox.PrtInfo.nType) {
        if (IsKanjiSJIS(*szBuf)) {
            TCHAR szTmp[4];
            memcpy(szTmp, szBuf, 2);
            szTmp[2] = '\0';
            if (strstr(szKeisen, szTmp)) {
                bKeisen = TRUE;
            }
            bKanji = TRUE;
        }
    }

    // デバッグモードの場合は、文字の回りを矩形で囲む
    if (g_PrtInfo.bDebug) {
        RECT rc;
        rc.left = nCurrentX;
        rc.right = nCurrentX + Size.cx;
        rc.top = nCurrentY;
        rc.bottom = nCurrentY + Size.cy;
        DrawRect(g_MailBox.hDC, &rc, RGB(255, 0, 0), PS_SOLID);
    }
    TextOut(g_MailBox.hDC, nCurrentX, nCurrentY, szBuf, cbString);
    nCurrentX += Size.cx;

    return TRUE;
}

/* -------------------------------------------------------------------
 * プリンタへ出力する。フォントは既に選択されている必要がある。
 * *-----------------------------------------------------------------*/
BOOL
PutsPrinter(LPTSTR szBuf)
{
    SIZE Size;
    TCHAR szTmp[4];
    int cbString = 1, i;

    // 一番最初又の印字
    if (!nCurrentY) {
        if (!BeginPage()) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = FALSE;
        bKeisen = FALSE;
    }

    // 最後の文字がCR or LFの場合は待避しておく
    szTmp[0] = '\0';
    if ('\n' == *(szBuf + strlen(szBuf) - 1) ||
        '\r' == *(szBuf + strlen(szBuf) - 1)) {
        szTmp[0] = '\n';
        *(szBuf + strlen(szBuf) - 1) = '\0';
    }

    // 纏めて出力できるかどうか確かめる
    GetTextExtentPoint32(g_MailBox.hDC, szBuf, strlen(szBuf), &Size);
    if ((nCurrentY + Size.cy) > nEndY) {        // 最終ラインを超えている
        // 最後の段の場合は排紙する
        if (0 == ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp)) {
            if (!EndPageDocument()) {
                return FALSE;
            }
        }
        if (!BeginPage()) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = bKeisen = FALSE;
    }

    if ((nCurrentX + Size.cx) < nEndX) {
        // 纏めて出力可能
        // デバッグモードの場合は、文字の回りを矩形で囲む
        if (g_PrtInfo.bDebug) {
            RECT rc;
            rc.left = nCurrentX;
            rc.right = nCurrentX + Size.cx;
            rc.top = nCurrentY;
            rc.bottom = nCurrentY + Size.cy;
            DrawRect(g_MailBox.hDC, &rc, RGB(0, 255, 0), PS_SOLID);
        }
        TextOut(g_MailBox.hDC, nCurrentX, nCurrentY, szBuf, strlen(szBuf));
        nCurrentX += Size.cx;

        if (PT_MAIL == g_MailBox.PrtInfo.nType) {
            for (i = 0; i < (int)strlen(szBuf); i++) {
                if (IsKanjiSJIS(*(LPBYTE)(szBuf + i))) {
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
        }
        
        if (szTmp[0]) {                         // 改行文字が存在する
            PutcPrinter(szTmp, 1);
        }
        return TRUE;
    }

    // 以下は纏めて出力できなかったのでちまちまと出力する処理
    if (szTmp[0]) {
        strcat(szBuf, "\n");
    }

    for (; *szBuf; szBuf++) {
        szTmp[0] = *szBuf;
        cbString = 1;
        if (IsKanjiSJIS((BYTE)szTmp[0])) {
            szTmp[1] = *(szBuf + 1);
            szBuf++;
            cbString = 2;
        }
        if (!PutcPrinter(szTmp, cbString)) {
            return FALSE;
        }
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * lpszStrプリンタへ出力する。フォントは指定されたパラメータを元に作成
 * する。
 * *-----------------------------------------------------------------*/
BOOL
SetFontAndPrint(
    LPTSTR lpszStr,                             // 出力文字列
    LPTSTR lfFaceName,                          // フォント名
    UINT lfHeight,                              // フォントの高さ
    UINT lfWeight,                              // フォントのウエイト
    BOOL lfItalic,                              // T:イタリック
    BOOL lfUnderline,                           // T:下線
    BOOL lfStrikeout,                           // T:打ち消し線
    BOOL bJapan                                 // T:日本語フォント
    )
{
    HFONT hFont, hOldFont;
    BOOL bResult;

    hFont = CreatePrtFont(lfFaceName, lfHeight, lfWeight, lfItalic,
                          lfUnderline, lfStrikeout, bJapan);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);

    bResult = PutsPrinter(lpszStr);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    return bResult;
}

/* -------------------------------------------------------------------
 * 矩形を描画する
 * *-----------------------------------------------------------------*/
void WINAPI
DrawRect(
    HDC hDC,                                    // デバイスコンテキスト
    LPRECT lprc,                                // 矩形座標
    COLORREF rgb,                               // 色
    int pnStyle                                 // 線のスタイル
    )
{
    HPEN hPen, hPenOld;                         // ペン
	LOGPEN lgpen;                               // 論理ペン
    POINT pt[5];

    // ペンの作成
    lgpen.lopnStyle = pnStyle;                  // 実線
    lgpen.lopnWidth.x = 1;                      // 太さ1
    lgpen.lopnColor = rgb;                      // 色は黒
    hPen = CreatePenIndirect(&lgpen);           // ペンの作成
    hPenOld = SelectObject(hDC, hPen);          // 現在のペンに設定

    pt[0].x = lprc->left, pt[0].y = lprc->top;
    pt[1].x = lprc->right - 1, pt[1].y = lprc->top;
    pt[2].x = lprc->right - 1, pt[2].y = lprc->bottom - 1;
    pt[3].x = lprc->left, pt[3].y = lprc->bottom - 1;
    pt[4].x = lprc->left, pt[4].y = lprc->top;

    Polyline(hDC, pt, 5);

    SelectObject(hDC, hPenOld);                 // ペンを元に戻す
    DeleteObject(hPen);                         // ペンの削除
}
