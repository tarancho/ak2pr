/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2000-11-05 15:05:35 tfuruka1>
 *
 * ak2ps のようなものの共通 DLL
 *
 * $Id: dllmain.c,v 1.1 2001/02/05 17:35:32 tfuruka1 Exp $
 * $Log: dllmain.c,v $
 * Revision 1.1  2001/02/05 17:35:32  tfuruka1
 * Initial revision
 *
 * Revision 1.3  2000/11/05 06:07:03  tfuruka1
 * 標準入力から読み込んだバイト数がゼロの場合は処理を行わないように修正し
 * た。
 *
 * Revision 1.2  2000/11/04 13:52:09  tfuruka1
 * Syslog関数を追加した。主な目的はデバッグ用である。
 *
 * Revision 1.1  2000/11/04 13:50:26  tfuruka1
 * Initial revision
 *
 * Revision 1.3  2000/10/22 11:21:55  tfuruka1
 * RCSがおかしくなったので復旧した。
 *
 * Revision 1.2  2000/10/22 09:37:57  tfuruka1
 * GetLongBaseName関数を追加した。
 *
 * Revision 1.1  1999/12/16 04:25:57  tfuruka1
 * Initial revision
 *
 */
#include "ak2prs.h"
#include "ak2pr.h"

/*--------------------------------------------------------------------
 * dwErr で指定されたエラーコードに対応するエラーメッセージを関数にシ
 * ステムメッセージテーブルリソースから検索して一時的な文字列へのポイ
 * ンタを返却する。dwErr は GetLastError から得た値を指定する事。lpsz 
 * はエラーメッセージへ追加する文字列を指定する。API 名等を指定する。
 * *-------------------------------------------------------------------*/
LPCSTR WINAPI
GetLastErrorMessage(LPCSTR lpsz, DWORD dwErr)
{
#ifdef _WIN32_WCE
    static LPTSTR sz = "WindowsCE では GetLastErrorMessage はみサポートです";
    return sz;
#else
    static char sz[1024];
    char szTmp[256];
    int i;
    
    if (!(i = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                            FORMAT_MESSAGE_IGNORE_INSERTS, 
                            NULL, dwErr, 0, szTmp, sizeof(szTmp), NULL))) {
        strcpy(szTmp, "---");
    }
    else {
        szTmp[i] = '\0';
        for (i--; 0 <= i; i--) {
            if ('\n' == szTmp[i] || '\r' == szTmp[i]) {
                szTmp[i] = '\0';
            }
        }
    }
    wsprintf(sz, "[WIN32] %s: Error Code = %d(%#02x): %s", 
             lpsz, dwErr, dwErr, szTmp);
    return (LPCTSTR)sz;
#endif
}

/*--------------------------------------------------------------------
 * 作業ファイルを作成する。正常に作成出来た場合はファイルポインタを返
 * 却する。作成できなかった場合は NULL を返却する。mode は fopen の 
 * mode と全く同じである。また, ファイルを使用し終った後は fclose でファ
 * イルをクローズ後, lpszFileName で返却したファイルを削除しなければな
 * らない。
 * *-------------------------------------------------------------------*/
FILE * WINAPI
MakeTempFile(
    IN const char *mode,                        // モード
    OUT LPTSTR lpszFileName                     // 作成した作業ファイル名
    )
{
    FILE *fp;
    char *p1;

    if (NULL == (p1 = getenv("TEMP"))) {
        MessageBox(NULL, "環境変数TEMPが見つかりません",
                   "MakeTempFile()", MB_ERROR);
        return NULL;
    }
    sprintf(lpszFileName, "%s/ak2prXXXXXX", p1);
    if (!_mktemp(lpszFileName)) {
        MessageBox(NULL, "作業ファイルが作成できませ",
                   "MakeTempFile()", MB_ERROR);
        return NULL;
    }
    if (NULL == (fp = fopen(lpszFileName, mode))) {
        MessageBox(NULL,
                   "作業ファイルをオープン作成できません MakeTempFile()",
                   strerror(0), MB_ERROR);
        return NULL;
    }
    return fp;
}

/*--------------------------------------------------------------------
 * 自分自身が格納されているディレクトリ名への一時的なポインタを得る。
 * 返却するポインタは一時的なものなので, 必要な場合は複写して使用する
 * 事。
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetMyDir(VOID)
{
    static TCHAR szBuf[1024];
    char *p1, *p2;

    // ファイルのフルパスを得る
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("GetModuleHandle", nErr), 
                   "GetMyDir()", MB_ERROR);
        return NULL;
    }
    
    p1 = strrchr(szBuf, '\\');
    p2 = strrchr(szBuf, '/');
    p1 = ((ULONG)p1 > (ULONG)p2) ? p1 : p2;
    if (!p1) {
        strcpy(szBuf, "./");
    }
    else {
        *(p1 + 1) = '\0';
    }
    return (LPCTSTR)szBuf;
}

/*--------------------------------------------------------------------
 * Unixのbasenameと同じ。返却するポインタは一時的なポインタなので、必
 * ずコピーして使用する必要がある。
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
BaseName(LPCTSTR lpszPath)
{
    LPCTSTR a, b;

    a = _mbsrchr(lpszPath, '\\');
    b = _mbsrchr(lpszPath, '/');

    if (a == b) {                               // 等しい時は NULLしかない
        return lpszPath;
    }

    return ((ULONG)a > (ULONG)b ? a : b) + 1;
}
LPCTSTR WINAPI
GetLongBaseName(LPCTSTR lpszPath)
{
    HANDLE hFile;
    WIN32_FIND_DATA wfd;

    hFile = FindFirstFile(lpszPath, &wfd);
    if (INVALID_HANDLE_VALUE == hFile) {
        return BaseName(lpszPath);
    }
    FindClose(hFile);
    return wfd.cFileName;
}

/*--------------------------------------------------------------------
 * プリントサーバが起動しているかチェックする。起動していた場合は HWND
 * を返却する。未起動の場合は NULL を返す。
 * *-------------------------------------------------------------------*/
BOOL WINAPI
IsPrtServerEnable(VOID)
{
    return (BOOL)FindWindow(SV_CLASS, SV_CAPTION);
}

/*--------------------------------------------------------------------
 * プリントサーバを起動する。正常に起動出来た場合は TRUE を返却する。起
 * 動できなかった場合は FALSE を返す。
 * *-------------------------------------------------------------------*/
static BOOL WINAPI
ExecutePrtServer(VOID)
{
    PROCESS_INFORMATION  pi;                    // プロセス情報
    STARTUPINFO sui;                            // 起動情報
    TCHAR szImageName[1024];                    // モジュール名
    LPCTSTR lpszMyDir;                          // 自分のディレクトリ
    int i;                                      // 汎用
    
    if (IsPrtServerEnable())                    // 既に起動していた場合は
        return TRUE;                            // 正常に起動したものとする

    if (!(lpszMyDir = GetMyDir())) {            // 自分自身のディレクトリを得る
        return FALSE;
    }

    // 実行ファイルを作成する
    sprintf(szImageName, "%s/%s", lpszMyDir, SV_EXE_NAME);

    // プロセス情報と起動情報の初期化
    memset(&sui, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    sui.cb = sizeof(STARTUPINFO);
    sui.dwFlags = STARTF_USESHOWWINDOW;
    sui.wShowWindow = SW_SHOWNORMAL;

    if (!CreateProcess(NULL, szImageName, NULL, NULL, FALSE,
                       0, NULL, NULL, &sui, &pi )) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateProcess", nErr),
                   "ExecutePrtServer()", MB_ERROR);
        return FALSE;
    }

    // WaitForInputIdle(pi.hProcess, INFINITE);
    // ウィンドウが生々されるまで待つ
    for (i = 0; i < 10; i++) {
        if (IsPrtServerEnable()) {
            break;
        }
        Sleep(1000);
    }
    // プロセスハンドルは必要無いので閉じる
    CloseHandle(pi.hProcess);

    if (pi.hThread) {                           // スレッドハンドルも閉じる
        CloseHandle(pi.hThread);
    }

    // もう一度サーバーをチェックする
    if (!IsPrtServerEnable()) {
        MessageBox(NULL, "サーバの起動に失敗したようです",
                   "ExecutePrtServer()", MB_ERROR);
        return FALSE;
    }
    return TRUE;                                // 正常起動
}

/*--------------------------------------------------------------------
 * プリントサーバへ印刷情報を送信する。
 * *-------------------------------------------------------------------*/
static BOOL WINAPI
SendPrintData(
    HWND hWnd,                                  // 送信元のハンドル
    PPRT_INFO pPrtInfo                          // 印刷情報
    )
{
    HWND hWndTo;                                // 送信先のハンドル
    COPYDATASTRUCT cds;                         // 送信データ

    if (!ExecutePrtServer()) {                  // プリンタサーバを起動
        return FALSE;                           // サーバが居ない
    }

    // DLLのVarsion不一致を防ぐ為, タイムスタンプを埋める
    strcpy(pPrtInfo->szTimeStamp, TIMESTAMP);
    strcpy(pPrtInfo->szTimeStamp1, TIMESTAMP);

    // サーバのハンドルを得る
    if (!(hWndTo = FindWindow(SV_CLASS, SV_CAPTION))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("FindWindow", nErr),
                   "SendPrintData()", MB_ERROR);
        return FALSE;
    }

    // 送信データを設定する
    cds.dwData = 0;
    cds.cbData = sizeof(PRT_INFO);
    cds.lpData = (LPVOID)pPrtInfo;

    // 印刷情報を送信する
    return SendMessage(hWndTo, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
}

/*--------------------------------------------------------------------
 * 標準入力の内容を読み込みプリントサーバへ印刷情報を送信する。
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromStdin(
    HWND hWnd,                                  // ハンドル
    LPCTSTR lpszTitle,                          // タイトル未指定時は"stdin"
    int nNumOfUp,                               // 段組数
    int nTab,                                   // タブ幅
    double fFontSize,                           // フォントサイズ
    BOOL bMail                                  // T: 印刷データはメール[mail]
    )
{
    PRT_INFO PrtInfo;                           // プリントファイル情報
    FILE *fp;                                   // ファイルポインタ
    TCHAR szBuf[1024];                          // 標準入力用バッファ
    BOOL bNotZero = FALSE;                      // ファイルサイズゼロ

    // プリントファイル情報の初期化
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    if (lpszTitle) {
        strncpy(PrtInfo.szTitle, lpszTitle, 255);
    }
    else {
        strcpy(PrtInfo.szTitle, "stdin");
    }

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = TRUE;
    PrtInfo.fFontSize = fFontSize;

    // 作業ファイルを作成する
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }

    // 標準入力から読み込む
    while (fgets(szBuf, 1024, stdin)) {
        fprintf(fp, "%s", szBuf);
        bNotZero++;
    }
    fclose(fp);

    if (!bNotZero) {
        unlink(PrtInfo.szFileName);
        Syslog("%s", "ファイルサイズが0なのでキャンセルします");
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*-------------------------------------------------------------------- 
 * 指定されたファイルを一時ファイルへ複写し，プリントサーバへ印刷情報
 * を送信する。
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromFileCopy(
    HWND hWnd,                                  // ハンドル
    LPCTSTR lpszTitle,                          // タイトル未指定時は"stdin"
    LPCTSTR lpszFileName,                       // コピー元ファイル名
    int nNumOfUp,                               // 段組数
    int nTab,                                   // タブ幅
    double fFontSize,                           // フォントサイズ
    BOOL bMail                                  // T: 印刷データはメール[mail]
    )
{
    PRT_INFO PrtInfo;                           // プリントファイル情報
    FILE *fp;                                   // ファイルポインタ

    // プリントファイル情報の初期化
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    if (lpszTitle) {
        strncpy(PrtInfo.szTitle, lpszTitle, 255);
    }
    else {
        strncpy(PrtInfo.szTitle, GetLongBaseName(lpszFileName), 255);
    }

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = TRUE;
    PrtInfo.fFontSize = fFontSize;

    // 作業ファイルを作成する
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }
    fclose(fp);

    // ファイルを複写する
    if (!CopyFile(lpszFileName, PrtInfo.szFileName, FALSE)) {
        MessageBox(hWnd, GetLastErrorMessage("GetFileAttributes()",
                                             GetLastError()),
                   PrtInfo.szFileName, MB_ERROR);
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * ファイルの存在チェックを行いプリントサーバへ印刷情報を送信する。
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromFile(
    HWND hWnd,                                  // ハンドル
    LPCTSTR lpszFileName,                       // ファイル名
    int nNumOfUp,                               // 段組数
    int nTab,                                   // タブ幅
    double fFontSize,                           // フォントサイズ
    BOOL bMail                                  // T: 印刷データはメール[mail]
    )
{
    PRT_INFO PrtInfo;                           // プリントファイル情報
    TCHAR szBuf[1024];                          // 標準入力用バッファ
    DWORD dwFA;                                 // ファイル情報

    // ファイルの存在チェックを行う
    if (0xFFFFFFFF == (dwFA = GetFileAttributes(lpszFileName))) {
        LPCTSTR p;
        dwFA = GetLastError();
        p = GetLastErrorMessage("GetFileAttributes()", dwFA);
        if (hWnd) {
            MessageBox(hWnd, p, __FILE__, MB_ICONSTOP);
        }
        else {
            fprintf(stderr, "%s\n", p);
        }
        return FALSE;
    }

    // ディレクトリの場合はエラー
    if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
        sprintf(szBuf, "%sはディレクトリです", lpszFileName);
        if (hWnd) {
            MessageBox(hWnd, lpszFileName, __FILE__, MB_ICONSTOP);
        }
        else {
            fprintf(stderr, "%s\n", lpszFileName);
        }
        return FALSE;
    }

    // プリントファイル情報の初期化
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    strncpy(PrtInfo.szTitle, GetLongBaseName(lpszFileName), 255);

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = FALSE;                    // ★ファイルは削除しない
    PrtInfo.fFontSize = fFontSize;
    strcpy(PrtInfo.szFileName, lpszFileName);

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * UNIXのSyslogの簡易版。常にdebug.local7しか出力しません。
 * *-------------------------------------------------------------------*/
VOID WINAPI
Syslog(
    LPCSTR lpstr,                               // 書式printfと同じ
    ...                                         // 引数
    )
{
    WSADATA wsaData;
    SOCKET finet;
    PSERVENT sp;
    SOCKADDR_IN sin;
    LPHOSTENT lpHost;
    va_list args;                               // 引数展開用
    char szLine[8192], szBuf[1024], *p;

    // モジュール名のフルパスを得る
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        return;
    }
    // ファイル名部分だけ切り出す
    if (NULL == (p = strrchr(szBuf, '\\'))) {
        return;
    }
    // FACILITY = 23, LEVEL = 7
    sprintf(szLine, "<%d>[%s] ", 7 + 23 * 8, p + 1);

    // 文字列を書式に合せて整形する
    va_start(args, lpstr);
    vsprintf(szLine + strlen(szLine), lpstr, args);
    va_end(args);

    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return;
	}

    if (0 > (finet = socket(AF_INET, SOCK_DGRAM, 0))) {
        return;
    }

    lpHost = gethostbyname("localhost");

    if (NULL == (sp = getservbyname("syslog", "udp"))) {
        return;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(sp->s_port);
    sin.sin_addr = *((LPIN_ADDR)*lpHost->h_addr_list);

    sendto(finet, szLine, strlen(szLine), 0, (PSOCKADDR)&sin,
           sizeof(SOCKADDR));
    closesocket(finet);
    WSACleanup();
}
