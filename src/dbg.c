/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:32:45 tfuruka1>
 * $Id: dbg.c,v 1.3 2004/06/18 00:57:57 tfuruka1 Exp $
 * $Log: dbg.c,v $
 * Revision 1.3  2004/06/18 00:57:57  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.2  2001/08/19 04:55:59  tfuruka1
 * ログファイルの格納パスをフルパスで持つようにした。
 *
 * Revision 1.1  2001/02/05 17:47:18  tfuruka1
 * Initial revision
 *
 * *-------------------------------------------------------------------
 * このファイルでは主にデバッグ支援に関する関数を定義している。
 * *-------------------------------------------------------------------*/

// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h>

LPCSTR WINAPI GetLastErrorMessage(LPCSTR lpsz, DWORD dwErr);
LPCTSTR WINAPI GetMyDir(VOID);
// -----------------------------------------------------------------------
static BOOL bDumpIsNoDisplay = TRUE;            // ダンプはファイルだけ
// -----------------------------------------------------------------------

#define MAX_LOGFILE  (512 * 1024)               // 最大ログファイルサイズ
#define MAX_LINE     1000                       // 最大表示行
#define MEM_SIZE     (8192 * 2)                 // 書式文字列格納バッファ
#define IDC_LIST     200                        // LISTBOXのID
#define FONT_H       13                         // フォントの高さ

#define COLOR_GRAY   RGB(128, 128, 128)         // グレイ
#define COLOR_LTGRAY RGB(192, 192, 192)         // ライトグレイ
#define COLOR_WHITE  RGB(255, 255, 255)         // 白
#define COLOR_BLACK  RGB(0, 0, 0)               // 黒

static WNDPROC lpWndProcOwn;                    // 親ウインドウのプロシジャ
static HWND hWndList = NULL;                    // Listのハンドル
static HFONT hFontN, hFontB, hFontI;            // フォント
static CHAR szLogName[MAX_PATH] = "\0";         // ログファイル名
static CHAR szLogNameSave[MAX_PATH];            // ログファイル名(古)

enum {IDM_CLEAR = 1100, IDM_TITLE, IDM_ZOOM, IDM_MINIMIZE,
      IDM_MOVE, IDM_SIZE, IDM_VIEWLOG, IDM_DELETELOG, IDM_NODUMP,
      DM_NUM_OF_MENU};

/*--------------------------------------------------------------------
 * 親ウインドウのサブクラスのWM_DRAWITEMの処理を行う。
 * *-------------------------------------------------------------------*/
static BOOL
DoDrawItem(LPDRAWITEMSTRUCT lpdis)
{
    LPTSTR lpszStr;                             // 文字列取得用
    size_t nSize;                               // サイズ取得用
    COLORREF crColor;                           // 色
    HPEN hPen, hOldPen;                         // ペン
    LOGPEN lgpn;                                // 論理ペン
    POINT Point[2];                             // ポイント
    HFONT hFont, hOldFont;                      // フォント

    if (-1 == lpdis->itemID)                    // 項目が空なので
        return TRUE;                            // 何もしない

    switch (lpdis->itemAction) {                // アイテムアクションは
    case ODA_DRAWENTIRE:                        // 全体の書き直し
        // 文字列長を得る
        nSize = SendMessage(lpdis->hwndItem, LB_GETTEXTLEN, lpdis->itemID, 0);
        // 必要なメモリを確保する
        if (NULL == (lpszStr = malloc(nSize + 1))) {
            MessageBox(lpdis->hwndItem, "メモリ確保に失敗しました",
                       "Com - Dbgprt - DrawItem", MB_ICONSTOP);
            break;
        }
        // 表示する文字列を得る
        SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID,
                    (LPARAM)lpszStr);
        // 文字列の色を設定する
        switch (*lpszStr) {
        case 'F':                               // WM_COPYDATAのフォワード
            hFont = hFontB, crColor = RGB(0, 0, 128); break;
        case 'C':                               // WM_COPYDATA受信
            hFont = hFontB, crColor = RGB(0, 128, 64); break;
        case 'R':                               // Receive関係
            hFont = hFontN, crColor = RGB(0, 128, 0); break;
        case 'r':                               // Receive関係(応答)
            hFont = hFontI, crColor = RGB(0, 128, 0); break;
        case 'S':                               // Send関係
            hFont = hFontN, crColor = RGB(0, 0, 255); break;
        case 's':                               // Send関係(応答)
            hFont = hFontI, crColor = RGB(0, 0, 255); break;
        case 'W':                               // ウォーニング
            hFont = hFontB, crColor = RGB(255, 0, 255); break;
        case 'E':                               // エラー
            hFont = hFontB, crColor = RGB(255, 0, 0); break;
        case 'B':                               // 強調
            hFont = hFontB, crColor = RGB(0, 0, 0); break;
        case 'I': default:                      // 通常又はデフォルト
            hFont = hFontN; crColor = RGB(0, 0, 0); break;
        }
        SetTextColor(lpdis->hDC, crColor);
        SetBkMode(lpdis->hDC, TRANSPARENT);
        // 文字列を描画する
        hOldFont = SelectObject(lpdis->hDC, hFont);
        DrawText(lpdis->hDC, lpszStr + 1, -1, &lpdis->rcItem,
                 DT_VCENTER	| DT_SINGLELINE | DT_EXPANDTABS | DT_NOPREFIX);
        SelectObject(lpdis->hDC, hOldFont);

        free(lpszStr);                          // メモリの開放

        /* break を忘れているわけではありません。選択状態を復元する為
           に, 以下の処理を実行させています*/
    case ODA_SELECT:                            // 選択

        // 使用する線の設定
        lgpn.lopnStyle = PS_SOLID;              // 線種
        lgpn.lopnWidth.x = 1;                   // 太さ

        // アンダーラインの座標を設定する
        Point[0].x = lpdis->rcItem.left;
        Point[1].x = lpdis->rcItem.right;
        Point[0].y = lpdis->rcItem.bottom - 1;
        Point[1].y = lpdis->rcItem.bottom - 1;
        if (ODS_SELECTED & lpdis->itemState) {  // 選択状態の場合は
            lgpn.lopnColor = COLOR_WHITE;       // 白
        }
        else {                                  // 非選択状態の場合は
            lgpn.lopnColor = COLOR_LTGRAY;      // 背景色
        }
        hPen = CreatePenIndirect(&lgpn);        // ペンの作成
        hOldPen = SelectObject(lpdis->hDC, hPen); // ペンの選択

        Polyline(lpdis->hDC, &Point[0], 2);     // アンダーラインを描画

        SelectObject(lpdis->hDC, hOldPen);      // ペンを元に戻す
        DeleteObject(hPen);                     // ペンを削除

        // オーバーラインの座標を設定する
        Point[0].x = lpdis->rcItem.left;
        Point[1].x = lpdis->rcItem.right;
        Point[0].y = lpdis->rcItem.top;
        Point[1].y = lpdis->rcItem.top;
        if (ODS_SELECTED & lpdis->itemState) {  // 選択状態の場合は
            lgpn.lopnColor = COLOR_GRAY;        // 黒
        }
        else {                                  // 非選択状態の場合は
            lgpn.lopnColor = COLOR_LTGRAY;      // 背景色
        }
        hPen = CreatePenIndirect(&lgpn);        // ペンの作成
        hOldPen = SelectObject(lpdis->hDC, hPen); // ペンの選択

        Polyline(lpdis->hDC, &Point[0], 2);     // オーバーラインを描画

        SelectObject(lpdis->hDC, hOldPen);      // ペンを元に戻す
        DeleteObject(hPen);                     // ペンを削除
        return FALSE;
    }
    return TRUE;
}
/*--------------------------------------------------------------------
 * 親ウインドウのサブクラス
 * *-------------------------------------------------------------------*/
static  LRESULT CALLBACK
SubWindProcOwn(
    HWND hWnd,                                  // ウィンドウハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    )
{
    long l;
    HMENU hMenu = GetSubMenu(GetMenu(hWnd), 0);

    switch (uMsg) {
    case WM_COMMAND:
        switch((int)(LOWORD(wParam))) {
        case IDM_DELETELOG:                     // ログ削除
            if (IDYES == MessageBox(hWnd, "ログファイルを削除しても"
                                    "宜しいですか", "本当?",
                                    MB_YESNO | MB_ICONQUESTION)) {
                unlink(szLogNameSave);
                unlink(szLogName);
            }
            return 0;
        case IDM_NODUMP:                        // 画面にダンプ表示しない
            bDumpIsNoDisplay = !bDumpIsNoDisplay;
            CheckMenuItem(hMenu, IDM_NODUMP,
                          bDumpIsNoDisplay ? MF_CHECKED : MF_UNCHECKED);
            return 0;
        case IDM_VIEWLOG:                       // ログを見る
            ShellExecute(hWnd, NULL, szLogName, NULL, NULL, SW_SHOW);
            return 0;
        case IDM_SIZE:                          // サイズ変更
            SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE, 0);
            return 0;
        case IDM_MOVE:                          // 移動
            SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, 0);
            return 0;
        case IDM_ZOOM:                          // 最大化
            SendMessage(hWnd, WM_SYSCOMMAND, SC_ZOOM, 0);
            return 0;
        case IDM_MINIMIZE:                      // 最小化
            SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            return 0;
        case IDM_TITLE:                         // タイトルバーの表示/消去
            if ((l = GetWindowLong(hWnd, GWL_STYLE)) & WS_CAPTION) {
                SetWindowLong(hWnd, GWL_STYLE, l & (~WS_CAPTION));
            }
            else {
                SetWindowLong(hWnd, GWL_STYLE, l | WS_CAPTION);
            }
            SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE |
                         SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
            return 0;
        case IDM_CLEAR:                         // 画面のクリア
            if (hWndList) {
                SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
            }
            return 0;
        default:
            break;
        }
    case WM_CTLCOLORLISTBOX:                    // リストボックスの背景色
        if ((HWND)lParam != hWndList) {         // 自分ではない
            break;
        }
        return (LRESULT)GetStockObject(LTGRAY_BRUSH);
    case WM_DRAWITEM:                           // オーナー描画
        if (IDC_LIST != (UINT)wParam) {         // LIST BOX 以外は
            break;                              // 処理しない
        }
        DoDrawItem((LPDRAWITEMSTRUCT)lParam);
        return TRUE;                            // [注意] See help
    case WM_MEASUREITEM:                        // オーナー描画を行うため
        if (IDC_LIST != (UINT)wParam) {         // LIST BOX 以外は
            break;                              // 処理しない
        }
        ((LPMEASUREITEMSTRUCT)lParam)->itemHeight = FONT_H + 2;
        return TRUE;                            // [注意] See help
    case WM_SIZE:                               // リサイズ
        if (hWndList) {
            SetWindowPos(hWndList, 0, 0, 0,
                         LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER);
        }
        return 0;
    }
    // 親のウインドウプロシジャをコールする
    return CallWindowProc(lpWndProcOwn, hWnd, uMsg, wParam, lParam);
}

/*--------------------------------------------------------------------
 * LISTBOXを作成する
 * *-------------------------------------------------------------------*/
static HWND
CreateListBox(HWND hWnd)
{
    HWND hWndList;                              // ウィンドウハンドル
    RECT rc;
    LOGFONT lf = {-FONT_H, 0, 0, 0, 400, 0, 0, 0, SHIFTJIS_CHARSET,
                  3, 2, 1, 49, TEXT("ＭＳ ゴシック")};
    double dummy;
    char *cdummy;

    dummy = strtod("10.1", &cdummy);

    // 親ウインドウのウインドウプロシジャをフックする
    lpWndProcOwn = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    if (!lpWndProcOwn) {
        int nErr = GetLastError();
        MessageBox(hWnd,
                   GetLastErrorMessage("GetWindowLong(GWL_WNDPROC)", nErr),
                   "ウインドウプロシジャのフックに失敗", MB_ICONSTOP);
    }
    SetWindowLong(hWnd, GWL_WNDPROC, (long)SubWindProcOwn);

    GetClientRect(hWnd, &rc);                   // クライアントサイズを得る

    hWndList = CreateWindowEx(
        0, "LISTBOX", "(^_^)",
        WS_CHILD | WS_VISIBLE | LBS_DISABLENOSCROLL | LBS_HASSTRINGS |
        LBS_USETABSTOPS	| LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL |
        WS_HSCROLL | LBS_OWNERDRAWFIXED,
        0, 0, rc.right, rc.bottom,
        hWnd, (HMENU)IDC_LIST, GetWindowInstance(hWnd), NULL);

    if (!hWndList) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindow", nErr),
                   "DbgPrint()->CreateListBox()", MB_ICONSTOP);
        return NULL;
    }

    /* デフォルトのフォントだとダンプが見にくくなる為，等幅フォントを
       設定する*/
    hFontN = CreateFontIndirect(&lf);           // フォントを作成(等幅)
    lf.lfItalic = TRUE;
    hFontI = CreateFontIndirect(&lf);           // フォントを作成(It)
    lf.lfItalic = FALSE, lf.lfWeight = 700;
    hFontB = CreateFontIndirect(&lf);           // フォントを作成(Bf)
    SendMessage(hWndList, WM_SETFONT, (WPARAM)hFontN, MAKELPARAM(TRUE, 0));

    return hWndList;
}

/*--------------------------------------------------------------------
 * ログファイルのサイズのチェックを行い，ファイルサイズを超えていた場
 * 合は，ログファイルをリネームする。
 * *-------------------------------------------------------------------*/
static VOID
CheckLogFile(
    LPCSTR CurrentLog,                          // 現在のログファイル名
    LPCSTR SaveLog                              // 保存ファイル名
    )
{
    struct _stat buffer;

    if (-1 == _stat(CurrentLog, &buffer)) {     // ログファイルの情報
        return;                                 // 戻る
    }

    if (MAX_LOGFILE < buffer.st_size) {
        unlink(SaveLog);
        rename(CurrentLog, SaveLog);
    }
}

/*--------------------------------------------------------------------
 * lpstrで指定された書式文字列を書式に従って編集し表示ウィンドウへ表示
 * する。書式はprintfと同様である。表示ウィンドウが存在しない場合は，
 * hWndへ表示ウィンドウをCreateした後に表示する。nFlagはメッセージを識
 * 別する為のインジケータとして表示される。nFlagは 'N' を除く0x20～
 * 0x7e迄の何れかのコードを指定出来る。戻り値は常に表示ウインドウのハ
 * ンドルである。但し，書式文字列に従って作成された文字列は16384文字以
 * 下でなければならない。nFlagは 'I', 'C', 'F', 'R', 'W', 'E', 'S', '
 * s', 'r' を認識します。それ以外は'I'として処理します。
 *
 * 注意: 表示ウインドウのコントロールIDは200を使用します。従ってオーナ
 * ウインドウではID=200のコントロールは使用できません(通常はIDは100か
 * ら使用されますので，ぶつかる可能性は少ない筈です)。ID=200の
 * WM_CTLCOLORLISTBOX, WM_DRAWITEM, WM_MEASUREITEM及び, WM_SIZEをフッ
 * クしますので，これらのイベントはオーナーウインドウへ通知されません。
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // ウインドウハンドル
    int nFlag,                                  // フラグ
    LPCSTR lpstr,                               // 書式printfと同じ
    ...                                         // 引数
    )
{
    static DWORD OldTickCount = 0;              // 前回出力の TickCount
    FILE *fp = NULL;                            // ログファイルポインタ
    va_list args;                               // 引数展開用
    char *szLine, *p1, *p2;                     // 文字列操作用
    SYSTEMTIME t;                               // 暦時間
    DWORD NewTickCount, TickInterval;           // 前回出力からの時間
    SIZE Size;                                  // サイズ取得用
    HDC hDC;                                    // デバイスコンテキスト
    int i, j, nMaxStrWd;                        // 汎用
    BOOL bNoDisplay = FALSE;                    // 画面出力無し
    TCHAR szBuf[1024];                          // 汎用

    if (!szLogName[0]) {
        // ログファイル名を取得する
        if (!GetWindowText(hWnd, szBuf, MAX_PATH - 5)) {
            int nErr = GetLastError();
            TCHAR szHwnd[256];
            sprintf(szHwnd, "[hWnd=%d]ウインドウテキストの取得に"
                    "失敗しました", hWnd);
            MessageBox(hWnd, GetLastErrorMessage("GetWindowText()", nErr),
                       szHwnd, MB_ICONSTOP);
            strcpy(szBuf, "hogehoge");
        }

        sprintf(szLogName, "%s/%d%s.log", GetMyDir(), 1, szBuf);
        sprintf(szLogNameSave, "%s/%d%s.log", GetMyDir(), 0, szBuf);

    }
    if (!hWndList) {
        HMENU hMenu = GetMenu(hWnd), hSubMenu;
        if (NULL == hMenu) {
            hMenu = CreateMenu();
            SetMenu(hWnd, hMenu);
        }
        hSubMenu = CreateMenu();

        AppendMenu(hSubMenu, 0, IDM_NODUMP, "ダンプを画面に表示しない");
        AppendMenu(hSubMenu, 0, IDM_TITLE, "タイトルバー");
        AppendMenu(hSubMenu, 0, IDM_CLEAR, "画面消去");
        AppendMenu(hSubMenu, MF_SEPARATOR, IDM_CLEAR, NULL);
        AppendMenu(hSubMenu, 0, IDM_VIEWLOG, "ログを見る");
        AppendMenu(hSubMenu, 0, IDM_DELETELOG, "ログを削除する");
        AppendMenu(hSubMenu, MF_SEPARATOR, IDM_CLEAR, NULL);
        AppendMenu(hSubMenu, 0, IDM_ZOOM, "最大化");
        AppendMenu(hSubMenu, 0, IDM_MINIMIZE, "最小化");
        AppendMenu(hSubMenu, 0, IDM_MOVE, "移動");
        AppendMenu(hSubMenu, 0, IDM_SIZE, "サイズ変更");

        CheckMenuItem(hSubMenu, IDM_NODUMP,
                      bDumpIsNoDisplay ? MF_CHECKED : MF_UNCHECKED);

        AppendMenu(hMenu,  MF_POPUP | MF_BYPOSITION, (UINT)hSubMenu,
                   "&Special");

        DrawMenuBar(hWnd);
        if(!(hWndList = CreateListBox(hWnd))){  // リストボックスの作成
            return NULL;                        // 失敗した時は諦める
        }
    }

    if (!bNoDisplay) {
        // LISTBOXの最大行数を超えている場合は半分削除する
        if (MAX_LINE < SendMessage(hWndList, LB_GETCOUNT, 0, 0)) {
            ShowWindow(hWndList, SW_HIDE);
            for (i = 0; i < (MAX_LINE / 2); i++) {
                SendMessage(hWndList, LB_DELETESTRING, 0, 0);
            }
            ShowWindow(hWndList, SW_SHOW);
        }
    }

    if (NULL == (szLine = malloc(MEM_SIZE))) {  // メモリの確保
        MessageBox(hWnd, "メモリ確保に失敗しました",
                   "DbgPrint()", MB_SETFOREGROUND);
        return NULL;
    }

    // 表示メッセージを編集する
    if ((nFlag & 0xff) == 'N') {                // ヘッダ無しの場合
        nFlag >>= 8;
        *szLine = (BYTE)nFlag;
        strcpy(szLine + 1, "\t");
        if (bDumpIsNoDisplay) {
            bNoDisplay = TRUE;                  // ディスプレイ出力無し
        }
    }
    else {                                      // ヘッダ有りの場合
        ULONG hh, mm, ss, ms;                   // 前回との時間
        NewTickCount = GetTickCount();          // 起動してからの時間
        if (!OldTickCount) {                    // 初めてのログ出力
            TickInterval = 0;                   // 前回との時間を0
        }
        else {
            TickInterval = NewTickCount - OldTickCount;
        }
        OldTickCount = NewTickCount;            // 前回との時間を得る
        hh = TickInterval / (3600 * 1000);
        mm = (TickInterval %= (3600 * 1000)) / (60 * 1000);
        ss = (TickInterval %= (60 * 1000)) / 1000;
        ms = TickInterval % 1000;
        GetLocalTime(&t);                       // 現在日時の読み込み
        sprintf(szLine, "%c[%d]%02d/%02d/%02d %02d:%02d:%02d.%03d"
                "(+%02d:%02d:%02d.%03d)[%c]: ",
                nFlag, GetCurrentThreadId(),
                t.wYear % 100, t.wMonth, t.wDay,
                t.wHour, t.wMinute, t.wSecond, t.wMilliseconds,
                hh, mm, ss, ms, nFlag);
    }

    // 文字列を書式に合せて整形する
    va_start(args, lpstr);
    vsprintf(szLine + strlen(szLine), lpstr, args);
    va_end(args);

    CheckLogFile(szLogName, szLogNameSave);     // ログファイルのサイズ
    if (NULL == (fp = fopen(szLogName, "at"))) {
        MessageBox(hWnd, "ログファイルのオープンに失敗しました",
                   "DbgPrint()", MB_ICONSTOP);
    }
    // LISTBOXへ表示
    p1 = szLine;
    while (p1) {
        p2 = strchr(p1, '\n');
        if (p2) {
            *p2 = '\0';
        }

        if (!bNoDisplay) {
            if (0 > SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)p1)) {
                /*
                MessageBox(hWnd, "ListBoxへの出力でエラーが発生しました",
                           "DbgPrint()", MB_SETFOREGROUND);
                           */
                break;
            }
        }
        if (fp) {                               // ファイルポインタが存在
            fprintf(fp, "%s\n", p1 + 1);        // ログ出力
        }
        if (p2) {
            *p2-- = '\t';
            *p2 = (BYTE)nFlag;
        }
        p1 = p2;
    }

    if (fp) {                                   // ファイルポインタが存在
        fclose(fp);                             // ファイルクローズ
    }

    if (!bNoDisplay) {
        // 常に最終行が見えるようにする
        SendMessage(hWndList, LB_SETTOPINDEX,
                    SendMessage(hWndList, LB_GETCOUNT, 0, 0) - 1, 0);


        // 最も長い行のドット数を得る
        nMaxStrWd = 0;
        hDC = GetDC(hWndList);                  // ListBoxのDC
        j = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
        for (i = 0; i < j; i++) {
            SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szLine);
            GetTextExtentPoint32(hDC, szLine, strlen(szLine), &Size);
            if (nMaxStrWd < Size.cx) {
                nMaxStrWd = Size.cx;
            }
        }
        ReleaseDC(hWndList, hDC);               // DCの開放
        // リストボックスの幅を調整する
        SendMessage(hWndList, LB_SETHORIZONTALEXTENT,
                    (WPARAM)nMaxStrWd + 16, 0);
    }

    free(szLine);                               // メモリ開放
    return hWndList;
}

/*--------------------------------------------------------------------
 * lpStartからuLenバイトの16進ダンプを表示する。表示ウィンドウが存在し
 * ない場合は，hWndへ表示ウィンドウをCreateした後に表示する。lpszHead
 * はダンプの前に表示する文字列を指定する。nFlagの意味はDbgPrintを
 * 参照の事。
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgDump(
    HWND hWnd,                                  // ハンドル
    int nFlag,                                  // フラグ
    LPCSTR lpszHead,                            // タイトル
    LPBYTE lpStart,                             // 開始アドレス
    ULONG uLen                                  // ダンプ長
    )
{
    TCHAR szBuf[128], szBufc[20];               // 作業用
    HWND hWndRet;                               // 戻り値
    ULONG i;                                    // 汎用
    UCHAR uc;                                   // 汎用
    int nSameLine = 0;                          // 前16バイトと同じデータ

    // タイトルを表示する
    hWndRet = DbgPrint(hWnd, nFlag, "%s (Length=%d)", lpszHead, uLen);

    for (i = 0; i < ((uLen + 15) & ~15); i++) {
        if (0 == (i % 16)) {
            if (i) {
                // メモリの内容が前16バイトと同じ場合は出力しない
                if (i > 16 && 0 == memcmp(lpStart + i - 16,
                                          lpStart + i - 32, 16)) {
                    nSameLine++;
                }
                else {
                    if (nSameLine) {            // 出力されていない行が有る
                        DbgPrint(hWnd, 'N' + nFlag * 256,
                                     "\t\t==== SAME %d BYTE ====",
                                     nSameLine * 16);
                        nSameLine = 0;
                    }
                    DbgPrint(hWnd, 'N' + nFlag * 256, "%s",
                                 strcat(strcat(szBuf, szBufc), " |"));
                }
            }
            sprintf(szBuf, "%08x |", i & ~15);
            strcpy(szBufc, " | ");
        }
        if (i < uLen) {
            uc = *(lpStart + i);
            sprintf(szBuf + strlen(szBuf), " %02x", uc);
            if (0x20 > uc || 0x7e < uc) {
                uc = '.';
            }
            sprintf(szBufc + strlen(szBufc), "%c", uc);
        }
        else {
            strcat(szBuf, "   ");
            strcat(szBufc, " ");
        }
    }
    if (nSameLine) {                            // 出力されていない行が有る
        DbgPrint(hWnd, 'N' + nFlag * 256,
                     "\t\t==== SAME %d BYTE ====", nSameLine * 16);
    }
    DbgPrint(hWnd, 'N' + nFlag * 256,
                 "%s", strcat(strcat(szBuf, szBufc), " |"));

    return hWndRet;
}
