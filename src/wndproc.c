/* -*- mode: c++; coding: sjis-dos; -*-
 *
 * 「ak2psのようなもの」のウインドウプロシジャ
 *
 * $Id: wndproc.c,v 1.1 2001/02/05 16:52:53 tfuruka1 Exp $
 * $Log: wndproc.c,v $
 * Revision 1.1  2001/02/05 16:52:53  tfuruka1
 * Initial revision
 *
 * Revision 1.2  2000/11/05 03:48:57  tfuruka1
 * 詳細ウィンドウを表示した時に「印刷停止」メニューにチェックマークを付け
 * ていたバグを修正。また、「詳細ウインドウ」メニューのチェックマークが逆
 * （メニューが表示されていないときにチェックマークが付いていた）だったバ
 * グを修正。
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#define TIME_STAMP "Time-stamp: <2001-02-06 01:52:52 tfuruka1>"

#include "ak2prs.h"

#define SZ_URL "http://www.vector.co.jp/authors/tfuruka1/"
#define SZ_MAIL "mailto:tfuruka1@nifty.com"
#define TMR_ID 100
#define TMR_TO 1000

#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
     (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

enum{IDC_TASK = 100, IDC_LIST};

static NOTIFYICONDATA s_ndi;                    // タスクトレイ
static HWND hWndOwn;                            // オーナウインドウハンドル
static HWND hWndList = NULL;                    // リストボックス
static UINT idTmr = 0;                          // タイマ識別子

/*--------------------------------------------------------------------
 * WM_CREATE の処理部。 
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoCreate(
    HWND hWnd,                                  // ウインドウハンドル
    LPCREATESTRUCT pcs                          // 作成データ格納する構造体
    )
{
    HMENU hMenu;

    hWndOwn = pcs->hwndParent;                  // オーナウインドウハンドル
    DbgPrint(pcs->hwndParent, 'I', "AK2PR Server(%s) Start.", TIME_STAMP);

    // タスクトレイにアイコンを表示する
    s_ndi.cbSize = sizeof(NOTIFYICONDATA);
    s_ndi.hWnd = hWnd; 
    s_ndi.uID = IDC_TASK; 
    s_ndi.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
    s_ndi.uCallbackMessage = WM_TASKMENU; 
    s_ndi.hIcon = LoadIcon(GetWindowInstance(hWnd),
                           MAKEINTRESOURCE(IDI_TIP));
    wsprintf(s_ndi.szTip, TEXT("%s"), SV_CAPTION);
    Shell_NotifyIcon(NIM_ADD, &s_ndi);

    // リストボックスを生々する(実はこれが印刷Queueの実態である)
    if (NULL == (hWndList = CreateWindowEx(
        0, "LISTBOX", "(^_^)",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        LBS_DISABLENOSCROLL | LBS_HASSTRINGS | LBS_USETABSTOPS	|
        LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL,
        0, 0, 10, 10,
        hWnd, (HMENU)IDC_LIST, GetWindowInstance(hWnd), NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() ListBox", MB_ERROR);
        return FALSE;
    }

    // メールボックスの初期化
    g_MailBox.bRun = TRUE;                      // スレッド実行中
    g_MailBox.bStop = TRUE;                     // 印刷停止
    g_MailBox.hWnd = hWnd;

    // 印刷スレッドの生成
    g_MailBox.hThread = CreateThread(NULL, 0,
                                    (LPTHREAD_START_ROUTINE)PrintThread,
                                    NULL, 0, &g_MailBox.idThread);
    if (NULL == g_MailBox.hThread) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateThread", nErr),
                   __FILE__ "/DoCreate()", MB_ERROR);
        return FALSE;
    }

    // インターバルタイマ設定
    idTmr = SetTimer(hWnd, TMR_ID, TMR_TO, NULL);

    // メニューのチェックボタンの更新
    hMenu = GetMenu(hWnd);
    hMenu = GetSubMenu(hMenu, 0);
    CheckMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND |
                  g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);

    return TRUE;
}
/*--------------------------------------------------------------------
 * WM_SIZE
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoSize(HWND hWnd,                               // ハンドル
       UINT state,                              // サイズ変更フラグ
       int cx,                                  // 幅
       int cy                                   // 高さ
    )
{
    if (SIZE_MINIMIZED == state) {              // アイコン化された場
        if (IsShellTray()) {                    // シェルトレイが有る場合は
            ShowWindow(hWnd, SW_HIDE);          // ウインドウを消去する
        }
        return;
    }
    if (hWndList) {
        SetWindowPos(hWndList, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    }
}
/*--------------------------------------------------------------------
 * WM_REALCLOSE 
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoRealClose(HWND hWnd)
{
    if (IDYES == MessageBox(hWnd, "終了しても宜しいですか", SV_CAPTION,
                            MB_SETFOREGROUND | MB_ICONQUESTION | MB_YESNO)) {
        SendMessage(hWndOwn, WM_REALCLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------
 * WM_CLOSE
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoClose(HWND hWnd)
{
    //ShowWindow(hWnd, SW_HIDE);
    DoRealClose(hWnd);
}

/*--------------------------------------------------------------------
 * WM_DESTROY メッセージの処理部
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoDestroy(HWND hWnd)
{
    Shell_NotifyIcon(NIM_DELETE, &s_ndi);       // タスクトレイから削除
}
/*--------------------------------------------------------------------
 * WM_COPYDATA メッセージの処理部
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoCopyData(
    HWND hWnd,                                  // ハンドル
    HWND hWndFrom,                              // 送り手側の HWND
    PCOPYDATASTRUCT pcds                        // データを格納する構造体
    )
{
    PPRT_INFO pPrtInfo;

    DbgDump(hWnd, 'C', "印刷要求受付", pcds->lpData, pcds->cbData);
    UpdateWindow(hWnd);

    if (NULL == (pPrtInfo = (PPRT_INFO)malloc(sizeof(PRT_INFO)))) {
        DbgPrint(hWnd, 'E', "メモリ不足の為、スプールできません");
        return FALSE;
    }

    memcpy(pPrtInfo, pcds->lpData, sizeof(PRT_INFO));

    // 未設定オプションはデフォルトで置き換える
    if (!pPrtInfo->nNumOfUp) {
        pPrtInfo->nNumOfUp = g_PrtInfo.nNumOfUp;
    }
    if (!pPrtInfo->nTab) {
        pPrtInfo->nTab = g_PrtInfo.nTab;
    }
    if (0.0 == pPrtInfo->fFontSize) {
        pPrtInfo->fFontSize = g_PrtInfo.fFontSize;
    }

    // パラメータで設定出来ない値を設定する
    pPrtInfo->bColor = g_PrtInfo.bColor;
    pPrtInfo->bNoRcvHeader = g_PrtInfo.bNoRcvHeader;

    EnQueue(hWndList, pPrtInfo);                // EnQueue

    return TRUE;
}

/*--------------------------------------------------------------------
 * WM_COMMAND メッセージの処理部
 * *-------------------------------------------------------------------*/
static VOID
DoCommand(
    HWND hWnd,                                  // ウインドウハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールハンドル
    UINT codeNotify                             // 通知コード
    )
{
    int nErr, cnt, i;
    TCHAR szMsg[1024];
    COPYDATASTRUCT cds;                         // 送信データ
    PRT_INFO PrtInfo;
    LPVOID pv;
    HMENU hMenu;

    switch (id) {
    case IDM_HTTP: case IDM_MAIL:
        if (32 >= (nErr = (int)ShellExecute(hWnd, NULL,
                                       (IDM_HTTP == id) ? SZ_URL : SZ_MAIL,
                                       NULL, NULL, SW_SHOW))) {
            sprintf(szMsg, "%dのエラーが発生しました", nErr);
            MessageBox(hWnd, szMsg, "DoCommand() " __FILE__, MB_ERROR);
        }
        break;
    case IDM_EXIT:
        SendMessage(hWnd, WM_REALCLOSE, 0, 0);
        break;
    case IDM_SHOW:
        ShowWindow(hWndOwn, IsWindowVisible(hWndOwn) ? SW_HIDE : SW_SHOW);
        // メニューのチェックボタンの更新
        hMenu = GetMenu(hWnd);
        hMenu = GetSubMenu(hMenu, 0);
        CheckMenuItem(hMenu, IDM_SHOW, MF_BYCOMMAND |
                      IsWindowVisible(hWndOwn) ? MF_CHECKED : MF_UNCHECKED);
        break;
    case IDM_SETUP:
        SetupPrtStyle(hWnd);
        break;
    case IDM_PRINTER:
        SetupPrinter(hWnd, &g_MailBox.hDevNames, &g_MailBox.hDevMode);
        break;
    case IDM_TESTPRT:                           // テスト印字
        PrtInfo.szFileName[0] = '\0';
        strcpy(PrtInfo.szTitle, "テスト印字");
        PrtInfo.nNumOfUp = 0;
        PrtInfo.nTab = 0;
        PrtInfo.fFontSize = 0;
        // DLLのVarsion不一致を防ぐ為, タイムスタンプを埋める
        strcpy(PrtInfo.szTimeStamp, TIMESTAMP);
        strcpy(PrtInfo.szTimeStamp1, TIMESTAMP);

        cds.dwData = 0;
        cds.cbData = sizeof(PRT_INFO);
        cds.lpData = (LPVOID)&PrtInfo;
        SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        break;
    case IDM_DELETE:
        cnt = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
        if (0 > cnt) {
            break;
        }
        for (i = 0, cnt--; cnt >= 0; cnt--) {
            if (SendMessage(hWndList, LB_GETSEL, cnt, 0)) {
                pv = (LPVOID)SendMessage(hWndList, LB_GETITEMDATA, cnt, 0);
                SendMessage(hWndList, LB_DELETESTRING, cnt, 0);
                if (LB_ERR == (int)pv || !pv) {
                    MessageBox(hWnd, "印刷データの削除に失敗しました",
                               "LB_GETITEMDATA",
                               MB_SETFOREGROUND | MB_ICONSTOP);
                    continue;
                }
                free(pv);
                i++;
            }
        }
        if (i) {
            TCHAR szBuf[256];
            sprintf(szBuf, "%d個の印刷データを削除しました", i);
            MessageBox(hWnd, szBuf, "消しちゃった",
                       MB_SETFOREGROUND | MB_ICONINFORMATION);
        }
        break;
    case IDM_STOP:                              // 印刷停止要求
        g_MailBox.bStop = g_MailBox.bStop ? FALSE : TRUE;
        DbgPrint(NULL, 'I', "印刷Queueのスケジュールを%sしました",
                 g_MailBox.bStop ? "停止" : "再開");
        // メニューのチェックボタンの更新
        hMenu = GetMenu(hWnd);
        hMenu = GetSubMenu(hMenu, 0);
        CheckMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND |
                      g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);
        break;
    }
}

/*--------------------------------------------------------------------
 * WM_TASKMENU メッセージ処理部
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
DoTaskMenu(
    HWND hWnd,                                  // ハンドル
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    )
{
    HMENU hMenu;                                // メニューハンドル
    HMENU hMenuTrackPopup;                      // メニューハンドル
    POINT pt;                                   // 座標

    switch (lParam) {
    case WM_RBUTTONDOWN:                        // 右ボタン押下
        // 先ずはお呪い
        SendMessage(hWnd, WM_NULL, 0, 0);       // 完全なお呪い
        SetForegroundWindow(hWnd);              // ウインドウを前面に

        GetCursorPos(&pt);                      // 現在のマウス座標を得る

        // リソースからメニューを得る
        if (!(hMenu = LoadMenu(GetWindowInstance(hWnd),
                               MAKEINTRESOURCE(IDR_MENU)))) {
            return 0;                           // 失敗したときは無視
        }
        // サブメニューを得る
        hMenuTrackPopup = GetSubMenu(hMenu, 0);

        // 詳細ウインドウの表示状態によってチェックを設定する
        CheckMenuItem(hMenuTrackPopup, IDM_SHOW, MF_BYCOMMAND | 
                      IsWindowVisible(hWndOwn) ? MF_CHECKED : MF_UNCHECKED);

        // 印刷停止状態によってチェックを設定する
        CheckMenuItem(hMenuTrackPopup, IDM_STOP, MF_BYCOMMAND |
                      g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);

        // メニューを表示する(メニューが選択されるまでは、この関数から
        // 戻って来ない)
        TrackPopupMenu(hMenuTrackPopup, 0, pt.x, pt.y, 0, hWnd, NULL);
        // メニューを削除する
        DestroyMenu(hMenu);
        break;
    case WM_LBUTTONDBLCLK:                      // 左ボタンのダブルクリック
        ShowWindow(hWnd, SW_SHOW);              // メインウインドウを表示
        SetForegroundWindow(hWnd);              // ウインドウを前面に
        if (IsIconic(hWnd)) {                   // アイコン状態の場合は
            OpenIcon(hWnd);                     // アイコンを開く
        }
        break;
    }
    return 0;
}

static VOID CALLBACK
DoTimer(
    HWND hWnd,
    UINT id
    )
{
    if (g_MailBox.bRun) {                       // 印刷スレッドが動作中の場合
        return;                                 // は何もしない
    }

    if (g_MailBox.bStop) {                      // 印刷停止中は
        return;                                 // 何もしない
    }

    // Queueが空の場合は何もしない
    if (!DeQueue(hWndList, &g_MailBox.PrtInfo)) {
        return;
    }
    
    // 印刷スレッドを起動する
    g_MailBox.bRun = TRUE;
    ResumeThread(g_MailBox.hThread);
}

/*--------------------------------------------------------------------
 * サスペンド要求
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
DoSuspend(
    HWND hWnd,
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    )
{
    DbgPrint(NULL, 'I', "印刷Threadを停止しました");
    g_MailBox.bRun = FALSE;
    SuspendThread((HANDLE)wParam);
    return 0;
}
    
/*--------------------------------------------------------------------
 * メインウインドウのウインドウプロシジャ
 * *-------------------------------------------------------------------*/
LRESULT CALLBACK
MainWndProc(
    HWND hWnd,                                  // ウインドウハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_CREATE, DoCreate);
        HANDLE_MSG(hWnd, WM_CLOSE, DoClose);
        HANDLE_MSG(hWnd, WM_DESTROY, DoDestroy);
        HANDLE_MSG(hWnd, WM_COPYDATA, DoCopyData);
        HANDLE_MSG(hWnd, WM_TASKMENU, DoTaskMenu);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommand);
        HANDLE_MSG(hWnd, WM_REALCLOSE, DoRealClose);
        HANDLE_MSG(hWnd, WM_SIZE, DoSize);
        HANDLE_MSG(hWnd, WM_TIMER, DoTimer);
        HANDLE_MSG(hWnd, WM_SUSPEND, DoSuspend);
    default:
        // 上記の何れのメッセージ以外の場合はデフォルトのプロシジャ
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/*--------------------------------------------------------------------
 * 詳細ウインドウのウインドウプロシジャ(実は殆ど何もしない)
 * *-------------------------------------------------------------------*/
LRESULT CALLBACK
MainWndProcDetail(
    HWND hWnd,                                  // ウインドウハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    )
{
    switch (uMsg) {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    case WM_REALCLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);                     // メッセージループ終了
        return 0;
    default:
        // 上記の何れのメッセージ以外の場合はデフォルトのプロシジャ
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
