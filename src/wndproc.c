/* -*- mode: c++; coding: sjis-dos; -*-
 *
 * 「ak2psのようなもの」のウインドウプロシジャ
 *
 * $Id: wndproc.c,v 1.17 2004/01/19 05:39:32 tfuruka1 Exp $
 * $Log: wndproc.c,v $
 * Revision 1.17  2004/01/19 05:39:32  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.16  2004/01/12 10:00:54  tfuruka1
 * 長辺綴じと短辺綴じに対応しました。
 *
 * Revision 1.15  2003/03/29 12:52:08  tfuruka1
 * ● リストビューのカラムの幅の初期値を少し変更した。
 * ● ツールボタンに「クリップボード印刷」を追加した。なので、当然、クリッ
 *    プボード印刷の処理を追加した。
 *
 * Revision 1.14  2003/03/16 00:21:56  tfuruka1
 * ● プレビューから印刷設定を行うと、設定が有効にならない項目があったの
 *    で、修正した。
 *
 * Revision 1.13  2003/03/15 14:47:04  tfuruka1
 * ● 印刷設定を行う関数の仕様変更に伴う修正。
 *
 * Revision 1.12  2003/03/14 16:55:34  tfuruka1
 * ツールバーにツールチップを付けました。
 *
 * Revision 1.11  2003/03/14 15:57:51  tfuruka1
 * ● 「印刷停止」の状態が、ツールボタンとメニューで異なる場合があったの
 *    で、修正した。
 * ● 起動時に常に「印刷停止」状態にしていたが、そのコマンドオプションで
 *    指定できるようにしたので、関連する部分の修正を行った。
 * ● PostScrip関連のオプションをak2prのコマンドラインから*将来*指定でき
 *    るようにする為の下地を作成した。
 *
 * Revision 1.10  2003/02/25 15:30:53  tfuruka1
 * 行番号出力の制御追加による修正
 *
 * Revision 1.9  2001/12/23 10:20:47  tfuruka1
 * ●ツールバーを新規追加
 * ●ドラッグ＆ドロップを追加（作業ディレクトリ内のファイルは印刷対象外と
 *   するようにしています）
 * ●デバッグウインドウの表示・非表示のメニューへの反映を、デバッグウイン
 *   ドウのウインドウプロシジャWM_SHOWWINDOWで行うように修正した。
 *
 * Revision 1.8  2001/12/14 17:03:59  tfuruka1
 * プレビュー対応
 *
 * Revision 1.7  2001/12/10 14:06:59  tfuruka1
 * 削除データが選択されていない状態で、印刷Queueの削除を実行した時にエラー
 * が表示されるように修正（というか、元々そうなっていたのに、LISTBOXから
 * ListViewへ変更した時の対応が漏れていた）。
 *
 * Revision 1.6  2001/12/08 15:21:40  tfuruka1
 * メイン画面のLISTBOX→ListView変更。
 *
 * Revision 1.5  2001/08/18 16:38:47  tfuruka1
 * ●終了する前にDeleteQueue関数を呼び出して、作業ファイルを削除するよう
 *   にした。
 * ●IDM_DELETEを受信した時に印刷Queueを削除する処理をコーディングしてい
 *   たが、複数の場所でこの処理が必要になったので、queue.cにDeleteQueue関
 *   数として抜き出し、DeleteQueue関数を呼び出すようにした。
 *
 * Revision 1.4  2001/02/05 17:13:37  tfuruka1
 * ホームページのURLをtfuruka1からVA001687へ変更した。
 *
 * Revision 1.3  2001/02/05 17:11:31  tfuruka1
 * プリンタの設定ダイアログをドロップダウンメニューからプロパティシートの
 * 共通タブへ移動した。
 *
 * Revision 1.2  2001/02/05 16:55:33  tfuruka1
 * 詳細ウィンドウを表示した時に「印刷停止」メニューにチェックマークを付け
 * ていたバグを修正。また、「詳細ウインドウ」メニューのチェックマークが逆
 * （メニューが表示されていないときにチェックマークが付いていた）だったバ
 * グを修正。
 *
 * Revision 1.1  2001/02/05 16:52:53  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#define TIME_STAMP "Time-stamp: <2004-01-19 12:23:01 tfuruka1>"

#include "ak2prs.h"

#define SZ_URL "http://www.vector.co.jp/authors/VA001687/"
#define SZ_MAIL "mailto:tfuruka1@nifty.com"
#define TMR_ID 100
#define TMR_TO 1000

#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
     (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

enum{IDC_TASK = 100, IDC_LIST, IDC_STS, IDC_TOOL};

static NOTIFYICONDATA s_ndi;                    // タスクトレイ
static HWND hWndOwn;                            // オーナウインドウハンドル
static HWND hWndMain = NULL;                    // メインウインドウ
static HWND hWndList = NULL;                    // リストボックス
static HWND hWndSts = NULL;                     // ステータスバー
static HWND hWndTool = NULL;                    // ツールバー
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
    int i;
    HINSTANCE hInst;
    LV_COLUMN lvcol[] = {
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 150,"要求受付日時", 0},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 20,"段組数", 1},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 50,"フォントサイズ", 2},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 20,"タブストップ", 3},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"用紙方向", 4},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"用紙サイズ", 5},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"印刷フォーマット", 7},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 180,"タイトル", 6},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 40,"行番号", 7},
        {0, 0, 100, NULL, 0}};
    TBBUTTON tbb[] = {
        {0, IDM_SETUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {1, IDM_TESTPRT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {2, IDM_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {3, IDM_STOP, TBSTATE_ENABLED /*| TBSTATE_CHECKED | TBSTATE_ELLIPSES*/,
         TBSTYLE_CHECK, 0, 0},
        {4, IDM_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {5, IDM_MAIL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {6, IDM_HTTP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {7, IDM_SHOW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {8, IDM_EXIT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0}
    };

    hWndOwn = pcs->hwndParent;                  // オーナウインドウハンドル
    hWndMain = hWnd;                            // 自分のウインドウ
    DbgPrint(pcs->hwndParent, 'I', "AK2PR Server(%s) Start.", TIME_STAMP);

    // タスクトレイにアイコンを表示する
    s_ndi.cbSize = sizeof(NOTIFYICONDATA);
    s_ndi.hWnd = hWnd; 
    s_ndi.uID = IDC_TASK; 
    s_ndi.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
    s_ndi.uCallbackMessage = WM_TASKMENU; 
    s_ndi.hIcon = LoadIcon(hInst = GetWindowInstance(hWnd),
                           MAKEINTRESOURCE(IDI_TIP));
    wsprintf(s_ndi.szTip, TEXT("%s"), SV_CAPTION);
    Shell_NotifyIcon(NIM_ADD, &s_ndi);

    // リストビューを生々する(実はこれが印刷Queueの実態である)
    InitCommonControls();
    if (NULL == (hWndList = CreateWindowEx(
        0,  WC_LISTVIEW, "(^_^)",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        LVS_REPORT, 0, 0, 10, 10,
        hWnd, (HMENU)IDC_LIST, hInst, NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() ListViwe32", MB_ERROR);
        return FALSE;
    }

    ListView_SetExtendedListViewStyle(hWndList,
                                      LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
                                      | LVS_EX_HEADERDRAGDROP);

    for (i = 0; lvcol[i].pszText; i++) {
        lvcol[i].iSubItem = i;
        ListView_InsertColumn(hWndList, i, &lvcol[i]);
    }

    if (!(hWndSts = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                                   WS_CHILD | SBARS_SIZEGRIP | CCS_BOTTOM
                                   | WS_VISIBLE, 0, 0, 0, 0, 
                                   hWnd, (HMENU)IDC_STS, hInst, NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() Status", MB_ERROR);
        return FALSE;
    }

    // ツールバーの作成
    if (!(hWndTool = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE
                                     | TBSTYLE_TOOLTIPS, IDC_TOOL,
                                     sizeof(tbb) / sizeof(TBBUTTON),
                                     hInst, IDR_TOOLBAR, tbb,
                                     sizeof(tbb) / sizeof(TBBUTTON),
                                     0, 0, 40, 40, sizeof(TBBUTTON)))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateToolbarEx", nErr),
                   __FILE__ "/DoCreate() Status", MB_ERROR);
        return FALSE;
    }

    // メールボックスの初期化
    g_MailBox.bRun = TRUE;                      // スレッド実行中
    //g_MailBox.bStop = TRUE;                     // 印刷停止
    // ↑コマンドオプションで設定するように変更
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

    // ツールバーのチェックボタンの状態設定
    SendMessage(hWndTool, TB_CHECKBUTTON, IDM_STOP,
                MAKELONG(g_MailBox.bStop, 0));

    // ドラッグアンドドロップを許可
    DragAcceptFiles(hWnd, TRUE);
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
    if (SIZE_MINIMIZED == state) {              // アイコン化された場合
        if (IsShellTray()) {                    // シェルトレイが有る場合は
            ShowWindow(hWnd, SW_HIDE);          // ウインドウを消去する
        }
        return;
    }

    /* ステータスバーのリサイズ */
    if (hWndSts) {
        SendMessage(hWndSts, WM_SIZE, state, MAKELONG(cx, cy));
    }

    /* ツールバーのリサイズ */
    if (hWndTool) {
        SendMessage(hWndTool, WM_SIZE, state, MAKELONG(cx, cy));
    }

    if (hWndList) {
        RECT rc;
        RECT rct;

        GetClientRect(hWndSts, &rc);
        GetClientRect(hWndTool, &rct);
        // ツールバーのウインドウサイズは何故か、ClientRectで得られた
        // 高さより、2ドット高いので、注意
        DbgPrint(NULL, 'D', "ToolBarSize: %dx%d", rct.right, rct.bottom);
        MoveWindow(hWndList, 0, rct.bottom + 2,
                   cx, cy - rc.bottom - rct.bottom - 2, TRUE);
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
        DeleteQueue(hWndList, TRUE);
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
    if (!pPrtInfo->bPreView) {
        pPrtInfo->bPreView = g_PrtInfo.bPreView;
    }
    if (-1 == pPrtInfo->bNum) {
        pPrtInfo->bNum = g_PrtInfo.bNum;
    }
    if (-1 == pPrtInfo->bShortBinding) {
        pPrtInfo->bShortBinding = g_PrtInfo.bShortBinding;
    }

    // PostScript関連は現状、コマンドラインから入力できないので、全て
    // デフォルトの値を使用する。
    strcpy(pPrtInfo->szAcrobat, g_PrtInfo.szAcrobat);
    strcpy(pPrtInfo->szGsPath, g_PrtInfo.szGsPath);
    strcpy(pPrtInfo->szGsOpt, g_PrtInfo.szGsOpt);

    // パラメータで設定出来ない値を設定する(将来は全てパラメータで設定
    // する予定)
    pPrtInfo->bColor = g_PrtInfo.bColor;
    pPrtInfo->bNoRcvHeader = g_PrtInfo.bNoRcvHeader;
    pPrtInfo->bNoCopyright = g_PrtInfo.bNoCopyright;
    pPrtInfo->bDebug = g_PrtInfo.bDebug;

    memcpy(&pPrtInfo->lfTHF, &g_PrtInfo.lfTHF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfPPF, &g_PrtInfo.lfPPF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfOF, &g_PrtInfo.lfOF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfOPPF, &g_PrtInfo.lfOPPF, sizeof(LOGFONT));

    // 構造体を有効にする
    pPrtInfo->valid = TRUE;

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
        break;
    case IDM_SETUP:
        SetupPrtStyle(hWnd, &g_PrtInfo);
        break;
    case IDM_TESTPRT:                           // テスト印字
        PrtInfo.szFileName[0] = '\0';
        strcpy(PrtInfo.szTitle, "テスト印字");
        PrtInfo.nNumOfUp = 0;
        PrtInfo.nTab = 0;
        PrtInfo.fFontSize = 0;
        PrtInfo.dmPaperSize = 0;
        PrtInfo.nOrientation = 0;
        PrtInfo.nType = PT_TEXT;
        // DLLのVarsion不一致を防ぐ為, タイムスタンプを埋める
        strcpy(PrtInfo.szTimeStamp, TIMESTAMP);
        strcpy(PrtInfo.szTimeStamp1, TIMESTAMP);

        cds.dwData = 0;
        cds.cbData = sizeof(PRT_INFO);
        cds.lpData = (LPVOID)&PrtInfo;
        SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        break;
    case IDM_DELETE:
        cnt = ListView_GetSelectedCount(hWndList);
        if (0 >= cnt) {
            MessageBox(hWnd, "削除する印刷データが選択されていません",
                       "削除できません", MB_SETFOREGROUND | MB_ICONSTOP);
            break;
        }
        i = DeleteQueue(hWndList, FALSE);

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

        // ツールバーのチェックボタンの状態設定。メニューから変更され
        // た場合にツールバーの状態も変更する。ツールバーで変更された
        // 場合は意味が無いが、悪さは与えない筈なので特にチェックは行
        // わない
        SendMessage(hWndTool, TB_CHECKBUTTON, IDM_STOP,
                    MAKELONG(g_MailBox.bStop, 0));
        break;
    case IDM_PASTE:
        // クリップボードの内容を印刷する
        SendPrintFromStdin(TRUE, NULL, NULL, 0, 8, 0, PT_TEXT, 0, 0, -1, -1);
        break;
    }
}

/*--------------------------------------------------------------------
 * WM_NOTIFY メッセージ処理部
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
    DoNotify(HWND hWnd,                         // ハンドル
             int idCtrl,                        // コントロール
             LPNMHDR pnmh                       // Notify情報
             )
{
    LPTOOLTIPTEXT lptip;

    switch (pnmh->code) {
    case TTN_NEEDTEXT:
        lptip = (LPTOOLTIPTEXT)pnmh;
        lptip->hinst = GetWindowInstance(hWnd);
        // ストリングリソースと、コントロールを同じ番号にしているので、
        // 以下のようにしている。今後、異なる番号にした時は注意が必要！
        lptip->lpszText = MAKEINTRESOURCE(lptip->hdr.idFrom);
        return 1;
    default:
        break;
    }
    return 0;
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
 * ディレクトリ内の全てのファイルを印刷対象ファイルとしてリクエストす
 * る。WM_DROPFILEの処理からのみ呼び出される
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
SendPrintDirectory(LPTSTR lpszFile)
{
    TCHAR szFind[MAX_PATH * 2];
    TCHAR szFile[MAX_PATH * 2];
    WIN32_FIND_DATA FindFileData;
    HANDLE hFile;

    sprintf(szFind, "%s/*.*", lpszFile);
    hFile = FindFirstFile(szFind, &FindFileData);
    while (INVALID_HANDLE_VALUE != hFile) {
        sprintf(szFile, "%s/%s", lpszFile, FindFileData.cFileName);
        // ディレクトリの場合
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 自分自身又は、自分の親ディレクトリ以外はファイル検索を
            // 行う
            if (0 != strcmp(FindFileData.cFileName, ".")
                && 0 != strcmp(FindFileData.cFileName, "..")) {

                if (0 != lstrcmpi(szFile, GetTempDirectoryName())) {
                    SendPrintDirectory(szFile);
                }
                else {
                    DbgPrint(NULL, 'W', "%sは作業ディレクトリです", szFile);
                }
            }
        }
        else {
            // ディレクトリ以外の場合は、印刷要求を行う。但し、作業ディ
            // レクトリ内のファイルは印字しない。無限ループしちゃうの
            // で。
            if (0 != _strnicmp(szFile, GetTempDirectoryName(),
                               strlen(GetTempDirectoryName()))) {
                SendPrintFromFileCopy(NULL, NULL, szFile, 0, 0, 0,
                                      PT_TEXT, 0, 0, -1, -1);
            }
            else {
                DbgPrint(NULL, 'W', "%sは作業ディレクトリ内のファイルです",
                         szFile);
            }
        }

        // 次のファイルを検索する
        if (!FindNextFile(hFile, &FindFileData)) {
            break;
        }
    }
    FindClose(hFile);
}

/*--------------------------------------------------------------------
 * WM_DROPFILEの処理を行う
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoDropFiles(
    HWND hWnd,
    HDROP hDrop
    )
{
    TCHAR szFile[MAX_PATH];
    UINT numOfFile;
    UINT i;
    DWORD dwFa;
 
    numOfFile = DragQueryFile(hDrop, 0xFFFFFFFF, szFile, MAX_PATH);
    for (i = 0; i < numOfFile; i++) {
        DragQueryFile(hDrop, i, szFile, MAX_PATH);
        dwFa = GetFileAttributes(szFile);
        if (0xFFFFFFFF == dwFa) {
            DbgPrint(NULL, 'E', "%s",
                     GetLastErrorMessage("DropFiles()", GetLastError()));
            continue;
        }

        // ディレクトリの場合は、その下のファイルを検索する
        if (FILE_ATTRIBUTE_DIRECTORY & dwFa) {
            SendPrintDirectory(szFile);
        }
        else {
            // 作業ファイル内のファイル以外の場合は、印刷要求を行う
            if (0 != _strnicmp(szFile, GetTempDirectoryName(),
                               strlen(GetTempDirectoryName()))) {
                SendPrintFromFileCopy(NULL, NULL, szFile, 0, 0, 0,
                                      PT_TEXT, 0, 0, -1, -1);
            }
            else {
                DbgPrint(NULL, 'W', "%sは作業ディレクトリ内のファイルです",
                         szFile);
            }
        }
    }
    DragFinish(hDrop);
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
        HANDLE_MSG(hWnd, WM_DROPFILES, DoDropFiles);
        HANDLE_MSG(hWnd, WM_NOTIFY, DoNotify);
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
    case WM_SHOWWINDOW:
        if (hWndMain) {
            HMENU hMenu;
            // メニューのチェックボタンの更新
            hMenu = GetMenu(hWndMain);
            hMenu = GetSubMenu(hMenu, 0);
            CheckMenuItem(hMenu, IDM_SHOW, MF_BYCOMMAND |
                          wParam ? MF_CHECKED : MF_UNCHECKED);
        }
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
