/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-12-23 16:12:46 tfuruka1>
 * $Id: init.c,v 1.13 2004/12/23 08:11:56 tfuruka1 Exp $
 * $Name:  $
 *
 * 「ak2psのようなもの」のサーバの初期化処理
 *
 * 備忘録
 *
 *   印刷オプション等を追加した場合、init.c, setup.c に修正が必要なの
 *   はファイル名から判断するのは容易ですが、その他にも以下のファイル
 *   に修正が必要になります。
 *
 *     wndproc.c - WM_COPYDATA の処理部
 *
 * $Log: init.c,v $
 * Revision 1.13  2004/12/23 08:11:56  tfuruka1
 * シングルライン印刷(食ミ出した部分を印刷しない)に対応しました。とりあえ
 * ず、サーバ側の設定のみです。
 *
 * Revision 1.12  2004/08/21 01:01:01  tfuruka1
 * テキスト印刷に於いて「行間」と「罫線連結」が有効になるようにしました。
 *
 * Revision 1.11  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.10  2004/01/19 05:34:26  tfuruka1
 * フォントに関連する情報をプロファイルから取得, 格納できるようにしました。
 *
 * Revision 1.9  2004/01/12 09:56:58  tfuruka1
 * 長辺綴じと短辺綴じに対応しました。プロファイルに短辺綴じの情報を保持す
 * るようにしました。
 *
 * Revision 1.8  2003/03/14 15:21:28  tfuruka1
 * ● PostScript関連のパス情報も*将来*コマンドラインから指定出来るように、
 *    パス情報をMAILBOXからPRT_INFOへ移動した。
 *
 * Revision 1.7  2001/12/23 10:24:34  tfuruka1
 * Copyrightを印字しないモードをサポートした
 *
 * Revision 1.6  2001/12/18 12:59:22  tfuruka1
 * デバッグ印刷のオプションを追加。
 *
 * Revision 1.5  2001/12/18 04:10:46  tfuruka1
 * プレビューウインドウの保存・復帰処理を追加
 *
 * Revision 1.4  2001/12/14 17:00:24  tfuruka1
 * プレビュー対応
 *
 * Revision 1.3  2001/08/19 08:55:39  tfuruka1
 * PostScriptファイルを印刷するときにGhostScriptを呼び出せるようにした。
 *
 * Revision 1.2  2001/08/19 04:36:41  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
 * Revision 1.1  2001/02/05 17:41:51  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

#define PROFILE_NAME "ak2pr.ini"
#define PROFILE_SEC  "Print setup"
#define KEY_NUMOFUP  "UP"
#define KEY_TAB      "TabStop"
#define KEY_FONT     "FontSize"
#define KEY_BNORCVH  "bNoRcvHeader"
#define KEY_BCOLOR   "bColor"
#define KEY_BKEISEN  "bKeisen"
#define KEY_BNUM     "bNum"
#define KEY_BASELINE "nBaseLine"
#define KEY_BPREVIEW "bPreview"
#define KEY_BDEBUG   "bDebug"
#define KEY_BSINGLE_LINE "bSingleLine"
#define KEY_BNOCOPYRIGHT "bNoCopyrightPrint"
#define KEY_BSHORT_BINDING "bShortBinding"

#define SEC_FONT     "FONT"
#define KEY_THF      "same-width"
#define KEY_PPF      "Proportional"
#define KEY_OF       "Other.same-width"
#define KEY_OPPF     "Other.Proportional"

#define SEC_DEVICE   "DEVICE SETUP"
#define KEY_DEVNAME  "DeviceName"
#define KEY_DEVMODE  "DeviceMode"

#define SEC_PS       "PostScript"
#define KEY_ACRIN    "AcrobatDistillerWatchdogFolderIN"
#define KEY_GS       "GhostScript.Path"
#define KEY_GSOP     "GhostScript.Option"

#define SEC_PREVIEW  "PreView"
#define KEY_TOP      "top"
#define KEY_BOTTOM   "bottom"
#define KEY_LEFT     "left"
#define KEY_RIGHT    "right"

#define BAD_STR      "(^_^;"
#define GET_PROFILE(sec, key) GetPrivateProfileString((sec), (key),\
                             BAD_STR, szBuf, 1023, szProfile)
#define WRT_PROFILE(sec, key, val) WritePrivateProfileString((sec), (key),\
                                   val, szProfile)
#define IsBadStr(str) (0 == strcmp((str), BAD_STR))

/*--------------------------------------------------------------------
 * GetPrivateProfileStringと同様であるが、lphGlobalへデータを格納する。
 * メモリは必要な分だけ、内部で確保する。
 * *-------------------------------------------------------------------*/
static BOOL GetPrivateProfileData(
    LPTSTR lpszSec,                             // セクション名
    LPTSTR lpszKey,                             // キー名
    HGLOBAL *lphGlobal,                         // データ格納ハンドル
    LPTSTR lpszProfile                          // プロファイル名
    )
{
    LPBYTE lpByte, lpSave, lpszData;
    DWORD cbData;
    BYTE szWk[16], szKey[256];

    sprintf(szKey, "%s.ByteSize", lpszKey);
    GetPrivateProfileString(SEC_DEVICE, szKey, BAD_STR, szWk, 15, lpszProfile);
    if (IsBadStr(szWk)) {
        return FALSE;
    }
    if (!(cbData = strtoul(szWk, NULL, 10))) {
        return FALSE;
    }

    if (NULL == (lpszData = malloc(cbData * 4))) {
        return FALSE;
    }

    sprintf(szKey, "%s.Data", lpszKey);
    GetPrivateProfileString(SEC_DEVICE, szKey, BAD_STR,
                            lpszData, cbData * 4 - 1, lpszProfile);
    if (IsBadStr(lpszData)) {
        free(lpszData);
        return FALSE;
    }
    lpSave = lpszData;

    if (NULL == (*lphGlobal = GlobalAlloc(GMEM_MOVEABLE, cbData))) {
        free(lpszData);
        return FALSE;
    }
    lpByte = GlobalLock(*lphGlobal);

    for (; *lpszData; lpByte++) {
        if ('^' == *lpszData) {
            *lpByte = *(lpszData + 1) - '@';
            lpszData += 2;
            continue;
        }
        if ('%' == *lpszData) {
            szWk[0] = *(lpszData + 1);
            szWk[1] = *(lpszData + 2);
            szWk[2] = '\0';
            *lpByte = (BYTE)(strtoul(szWk, NULL, 16) % 256);
            lpszData += 3;
            continue;
        }
        *lpByte = *lpszData;
        lpszData++;
    }

    GlobalUnlock(*lphGlobal);
    free(lpSave);
    return TRUE;
}

/*--------------------------------------------------------------------
 * WritePrivateProfileStringと同様であるが、lpDataから始まるデータを
 * cbDataバイトを文字列へ変換してから書き込む。
 * *-------------------------------------------------------------------*/
static BOOL WritePrivateProfileData(
    LPTSTR lpszSec,                             // セクション名
    LPTSTR lpszKey,                             // キー名
    LPVOID lpData,                              // データ先頭アドレス
    DWORD cbData,                               // バイト数
    LPTSTR szFileName                           // プロファイル名
    )
{
    int i;
    LPBYTE lpszStore;
    LPBYTE lpByte = lpData;
    TCHAR szBuf[16], szKey[256];

    if (NULL == (lpszStore = malloc(cbData * 4))) {
        return FALSE;
    }

    *lpszStore = '\0';
    for (i = 0; (ULONG)i < cbData; i++) {
        if (0x20 >= *(lpByte + i)) {
            sprintf(lpszStore + strlen(lpszStore), "^%c",
                    '@' + (int)*(lpByte + i));
            continue;
        }
        if (('^' == *(lpByte + i))
            || ('%' == *(lpByte + i))
            || (0x7f <= *(lpByte + i))) {
            sprintf(lpszStore + strlen(lpszStore), "%%%02x", *(lpByte + i));
            continue;
        }
        sprintf(lpszStore + strlen(lpszStore), "%c", *(lpByte + i));
    }

    sprintf(szBuf, "%d", cbData);
    sprintf(szKey, "%s.ByteSize", lpszKey);
    WritePrivateProfileString(lpszSec, szKey, szBuf, szFileName);
    sprintf(szKey, "%s.Data", lpszKey);
    WritePrivateProfileString(lpszSec, szKey, lpszStore, szFileName);

    free(lpszStore);

    return TRUE;
}

/*--------------------------------------------------------------------
 * デフォルトの印刷情報を得る
 * *-------------------------------------------------------------------*/
void
GetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    GET_PROFILE(PROFILE_SEC, KEY_NUMOFUP);
    g_PrtInfo.nNumOfUp = IsBadStr(szBuf) ? 2 : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_TAB);
    g_PrtInfo.nTab = IsBadStr(szBuf) ? 8 : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_FONT);
    g_PrtInfo.fFontSize = IsBadStr(szBuf) ? 10.0 : atof(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BPREVIEW);
    g_PrtInfo.bPreView = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BNOCOPYRIGHT);
    g_PrtInfo.bNoCopyright = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BDEBUG);
    g_PrtInfo.bDebug = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BNORCVH);
    g_PrtInfo.bNoRcvHeader = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BCOLOR);
    g_PrtInfo.bColor = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BKEISEN);
    g_PrtInfo.bKeisen = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BASELINE);
    g_PrtInfo.nBaseLine = IsBadStr(szBuf) ? IDC_R_ENGLISH : atoi(szBuf);
    switch (g_PrtInfo.nBaseLine) {
    case IDC_R_NOINTERLINE:
    case IDC_R_ENGLISH:
    case IDC_R_JAPAN:
    case IDC_R_AUTO:
        break;
    default:
        g_PrtInfo.nBaseLine = IDC_R_ENGLISH;
    }

    GET_PROFILE(PROFILE_SEC, KEY_BNUM);
    g_PrtInfo.bNum = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BSHORT_BINDING);
    g_PrtInfo.bShortBinding = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BSINGLE_LINE);
    g_PrtInfo.nSingleLine = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    // フォント情報を得る
    GET_PROFILE(SEC_FONT, KEY_THF);
    strncpy(g_PrtInfo.lfTHF.lfFaceName, IsBadStr(szBuf) ? FN_MSM : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_PPF);
    strncpy(g_PrtInfo.lfPPF.lfFaceName, IsBadStr(szBuf) ? FN_MSPG : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_OF);
    strncpy(g_PrtInfo.lfOF.lfFaceName, IsBadStr(szBuf) ? FN_COU : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_OPPF);
    strncpy(g_PrtInfo.lfOPPF.lfFaceName, IsBadStr(szBuf) ? FN_ARIAL : szBuf,
            LF_FACESIZE);

    // PostScript関連情報を得る
    GET_PROFILE(SEC_PS, KEY_ACRIN);
    strncpy(g_PrtInfo.szAcrobat, szBuf, MAX_PATH);

    GET_PROFILE(SEC_PS, KEY_GS);
    strncpy(g_PrtInfo.szGsPath, szBuf, MAX_PATH);

    GET_PROFILE(SEC_PS, KEY_GSOP);
    strncpy(g_PrtInfo.szGsOpt, szBuf, 512);

    // プリンタのデバイス情報を得る
    if (!GetPrivateProfileData(SEC_DEVICE, KEY_DEVNAME,
                               &g_MailBox.hDevNames, szProfile)) {
        g_MailBox.hDevNames = NULL;
        g_MailBox.hDevMode = NULL;
        return;
    }
    if (!GetPrivateProfileData(SEC_DEVICE, KEY_DEVMODE,
                               &g_MailBox.hDevMode, szProfile)) {
        GlobalFree(g_MailBox.hDevNames);
        g_MailBox.hDevNames = NULL;
        g_MailBox.hDevMode = NULL;
    }
}

/*--------------------------------------------------------------------
 * 印刷設定情報を書き戻す
 * *-------------------------------------------------------------------*/
void
SetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];
    LPVOID lpData;
    DWORD cbData;

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    sprintf(szBuf, "%d", g_PrtInfo.nNumOfUp);
    WRT_PROFILE(PROFILE_SEC, KEY_NUMOFUP, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nTab);
    WRT_PROFILE(PROFILE_SEC, KEY_TAB, szBuf);

    sprintf(szBuf, "%f", g_PrtInfo.fFontSize);
    WRT_PROFILE(PROFILE_SEC, KEY_FONT, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bPreView);
    WRT_PROFILE(PROFILE_SEC, KEY_BPREVIEW, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNoCopyright);
    WRT_PROFILE(PROFILE_SEC, KEY_BNOCOPYRIGHT, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bDebug);
    WRT_PROFILE(PROFILE_SEC, KEY_BDEBUG, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNoRcvHeader);
    WRT_PROFILE(PROFILE_SEC, KEY_BNORCVH, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bColor);
    WRT_PROFILE(PROFILE_SEC, KEY_BCOLOR, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bKeisen);
    WRT_PROFILE(PROFILE_SEC, KEY_BKEISEN, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nBaseLine);
    WRT_PROFILE(PROFILE_SEC, KEY_BASELINE, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNum);
    WRT_PROFILE(PROFILE_SEC, KEY_BNUM, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bShortBinding);
    WRT_PROFILE(PROFILE_SEC, KEY_BSHORT_BINDING, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nSingleLine);
    WRT_PROFILE(PROFILE_SEC, KEY_BSINGLE_LINE, szBuf);

    // フォント情報の書き込み
    WRT_PROFILE(SEC_FONT, KEY_THF, g_PrtInfo.lfTHF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_PPF, g_PrtInfo.lfPPF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_OF, g_PrtInfo.lfOF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_OPPF, g_PrtInfo.lfOPPF.lfFaceName);

    // PostScript情報関連の書き込み
    WRT_PROFILE(SEC_PS, KEY_ACRIN, g_PrtInfo.szAcrobat);
    WRT_PROFILE(SEC_PS, KEY_GS, g_PrtInfo.szGsPath);
    WRT_PROFILE(SEC_PS, KEY_GSOP, g_PrtInfo.szGsOpt);

    // デバイス情報の書き込み
    if (g_MailBox.hDevNames && g_MailBox.hDevMode) {

        lpData = GlobalLock(g_MailBox.hDevNames);
        cbData = GlobalSize(g_MailBox.hDevNames);
        WritePrivateProfileData(SEC_DEVICE, KEY_DEVNAME,
                                lpData, cbData, szProfile);
        GlobalUnlock(g_MailBox.hDevNames);

        lpData = GlobalLock(g_MailBox.hDevMode);
        cbData = GlobalSize(g_MailBox.hDevMode);
        WritePrivateProfileData(SEC_DEVICE, KEY_DEVMODE,
                                lpData, cbData, szProfile);
        GlobalUnlock(g_MailBox.hDevNames);
    }
}

/*--------------------------------------------------------------------
 * プレビューウインドウのウインドウ位置をプロファイルへ書き込む
 * *-------------------------------------------------------------------*/
void
SetPreViewPos(LPRECT lprc)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    sprintf(szBuf, "%d", lprc->top);
    WRT_PROFILE(SEC_PREVIEW, KEY_TOP, szBuf);

    sprintf(szBuf, "%d", lprc->bottom);
    WRT_PROFILE(SEC_PREVIEW, KEY_BOTTOM, szBuf);

    sprintf(szBuf, "%d", lprc->left);
    WRT_PROFILE(SEC_PREVIEW, KEY_LEFT, szBuf);

    sprintf(szBuf, "%d", lprc->right);
    WRT_PROFILE(SEC_PREVIEW, KEY_RIGHT, szBuf);
}

/*--------------------------------------------------------------------
 * プレビューウインドウのウインドウ位置をプロファイルから読み込む
 * *-------------------------------------------------------------------*/
void
GetPreViewPos(LPRECT lprc)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];
    int wd, ht;

    wd = GetSystemMetrics(SM_CXSCREEN);
    ht = GetSystemMetrics(SM_CYSCREEN);

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    GET_PROFILE(SEC_PREVIEW, KEY_TOP);
    lprc->top = IsBadStr(szBuf) ? (ht / 2 - 300) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_BOTTOM);
    lprc->bottom = IsBadStr(szBuf) ? (ht / 2 + 300) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_LEFT);
    lprc->left = IsBadStr(szBuf) ? (wd / 2 - 400) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_RIGHT);
    lprc->right = IsBadStr(szBuf) ? (wd / 2 + 400) : atoi(szBuf);

    if (lprc->top > ht) {
        lprc->top = 0;
    }
    if (lprc->bottom < 0) {
        lprc->bottom = ht;
    }
    if (lprc->left > wd) {
        lprc->left = 0;
    }
    if (lprc->right < 0) {
        lprc->right = wd;
    }
}

/*--------------------------------------------------------------------
 * クラス登録及びウインドウの生成を行う
 * *-------------------------------------------------------------------*/
HWND
CreateApplication(HANDLE hInstance)
{
    HWND hWnd;                                  // ウィンドウハンドル
    WNDCLASS wc;                                // ウインドウクラス

    // 印字の初期情報を読み込む
    GetDefaultPrtInfo();
    SetDefaultPrtInfo();                        // ファイルが存在しない場合

    // プレビューウインドウの位置を復元する
    GetPreViewPos(&g_MailBox.PrevInfo.rc);

    // デバイス情報が有効か否か調べる
    if (g_MailBox.hDevNames) {
        HDC hDC;
        LPDEVNAMES lpDevNames;
        LPDEVMODE lpDevMode;

        lpDevNames = (LPDEVNAMES)GlobalLock(g_MailBox.hDevNames);
        lpDevMode = (LPDEVMODE)GlobalLock(g_MailBox.hDevMode);

        hDC = CreateDC((PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                       (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                       NULL, lpDevMode);
        GlobalUnlock(g_MailBox.hDevNames);
        GlobalUnlock(g_MailBox.hDevMode);

        if (hDC) {
            DeleteDC(hDC);
        }
        else {
            GlobalFree(g_MailBox.hDevNames);
            GlobalFree(g_MailBox.hDevMode);

            g_MailBox.hDevNames = NULL;
            g_MailBox.hDevMode = NULL;
        }
    }

    // 詳細ウインドウ
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)MainWndProcDetail;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FURUKAWA));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;                    // 背景は描画しない
    wc.lpszMenuName = NULL;                     // メニューは無し
    wc.lpszClassName = SV_DBGCLASS;             // クラス名

    // レジストする
    if (!RegisterClass(&wc)) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("RegisterClass", nErr),
                   "CreateApplication()", MB_ERROR);
        return NULL;
    }

    // 詳細ウインドウを作成する
    hWnd = CreateWindowEx(0, SV_DBGCLASS, "詳細(ak2pr)",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL);

    // ウインドウの生成に失敗
    if (!hWnd) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateWindowEx", nErr),
                   "CreateApplication()", MB_ERROR);
        return FALSE;
    }

    // 詳細ウインドウを表示する
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    // メインウインドウ
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FURUKAWA));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;                    // 背景は描画しない
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = SV_CLASS;                // クラス名

    // レジストする
    if (!RegisterClass(&wc)) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("RegisterClass", nErr),
                   "CreateApplication()", MB_ERROR);
        return NULL;
    }

    // メインウインドウを作成する(実はメインといいつつ詳細ウインドウの
    // 子供である)
    hWnd = CreateWindowEx(0, SV_CLASS, SV_CAPTION,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          hWnd, NULL, hInstance, NULL);

    // ウインドウの生成に失敗
    if (!hWnd) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateWindowEx", nErr),
                   "CreateApplication()", MB_ERROR);
        return FALSE;
    }

    // シェルトレイが存在する場合はウインドウを非表示にする。シェルト
    // レイが存在しない場合は、ウインドウを表示する
    if (!IsShellTray()) {
        DbgPrint(NULL, 'W', "多分, 「タスクバー」が存在しません。"
                 "\nデスクトップ上のウインドウとしてアイコン状態で"
                 "起動します。");
        CloseWindow(hWnd);
    }
    else {
        DbgPrint(NULL, 'I', "「タスクバー」が見つかりました。"
                 "\nタスクバーの時計の隣にアイコンとして"
                 "表示します。");
    }
    ShowWindow(hWnd, IsShellTray() ? SW_HIDE : SW_SHOW);
    UpdateWindow(hWnd);

    return hWnd;
}
