/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-14 22:09:25 tfuruka1>
 *
 * ak2ps のようなものの共通 DLL
 *
 * $Id: dllmain.c,v 1.13 2003/03/14 15:06:19 tfuruka1 Exp $
 * $Log: dllmain.c,v $
 * Revision 1.13  2003/03/14 15:06:19  tfuruka1
 * ● Syslog関数の仕様変更に伴い、Syslog関数を使用している部分の修正を行っ
 *    た。
 * ● 作業ファイルの作成ルールを一部変更した(.を_に変更)。Distillerが作成
 *    するPDFファイルが、拡張子以降を無視するので、出力ファイル名が重複す
 *    るのを避けるのが目的である。
 * ● サーバの起動関数に、サーバの起動オプションを追加した。
 * ● Syslog関数の仕様を変更した。第一パラメータに標準出力オプションを追
 *    加した。
 * ● PostScriptファイルからタイトルを取得する関数を追加した。
 *
 * Revision 1.12  2003/03/01 09:01:25  tfuruka1
 * ● 作業ファイルの作成処理において、ファイル名に使用出来ない文字が指定去
 *    れた場合に適切な文字に変更するように修正を行った。
 *
 * Revision 1.11  2003/02/25 15:28:35  tfuruka1
 * 行番号印刷の処理追加による対応を行った。
 *
 * Revision 1.10  2001/12/23 10:23:33  tfuruka1
 * ●作業ディレクトリ名取得の関数を新規追加（作業ファイル作成関数から切り出
 *   し）
 *
 * Revision 1.9  2001/12/17 14:33:17  tfuruka1
 * syslog内部で標準出力へ出力していた処理を削除した（やっぱりうっとうしい
 * ので）。
 *
 * Revision 1.8  2001/12/14 17:04:43  tfuruka1
 * プレビュー対応
 *
 * Revision 1.7  2001/12/08 15:19:06  tfuruka1
 * 用紙サイズの指定対応の対応漏れの対応。
 *
 * Revision 1.6  2001/12/07 18:25:11  tfuruka1
 * 用紙サイズの指定を出来るようにした。
 *
 * Revision 1.5  2001/10/01 13:20:47  tfuruka1
 * 用紙の向きを指定出来るように修正。
 *
 * Revision 1.4  2001/08/19 04:35:57  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
 * Revision 1.3  2001/08/18 16:30:30  tfuruka1
 * ●作業ファイルの作成に_mktemp関数を使用していたが、よく考えたらこの関
 *   数は、最大で 27 個しか一意なファイル名を作成できない事に気が付いた
 *   （というか、エラーが出た）ので、_mktempのbaseが極力一意になるように
 *   して、27個以上の一意なファイルを作成できるようにした。
 * ●MessageBoxを使用している部分をSyslogを使用するように変更した(Meadow
 *   から呼ばれた時にMessageBoxを使用すると、Windowが見えないのでダイアロ
 *   グを閉じる事ができなくなるので)。
 * ●SendPrintFromFile関数を廃止し、SendPrintFromFileCopy関数に統合した。
 * ●Syslog関数で、標準出力にも出力するようにした（syslogdがなくてもデ
 *   バッグできる）。
 *
 * Revision 1.2  2001/02/05 17:38:02  tfuruka1
 * RCSがおかしくなったので、復旧した。過去の修正内容は以下の通り。
 * ●標準入力から読み込んだバイト数がゼロの場合は処理を行わないように修正
 *   した。
 * ●Syslog関数を追加した。主な目的はデバッグ用である。
 * ●GetLongBaseName関数を追加した。
 *
 * Revision 1.1  2001/02/05 17:35:32  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "[ \t]+$" "")
#include "ak2prs.h"
#include "ak2pr.h"

/* 用紙サイズの一覧 */
static struct {
    LPTSTR cmdOpt;                              // コマンドパラメータ等
    short dmPaperSize;                          // DEVMODEのパラメータ
    LPTSTR sComment;                            // 用紙の説明
} devModePs[] = {
    {"A3", DMPAPER_A3, "A3シート, 297×420ミリメートル"}, 
    {"A4", DMPAPER_A4, "A4シート, 210×297ミリメートル"}, 
    {"A4SMALL", DMPAPER_A4SMALL, "A4 small シート, 210×297ミリメートル"}, 
    {"A5", DMPAPER_A5, "A5シート, 148×210ミリメートル"}, 
    {"B4", DMPAPER_B4, "B4シート, 250×354ミリメートル"}, 
    {"B5", DMPAPER_B5, "B5シート, 182×257ミリメートル"}, 
    {"LETTER", DMPAPER_LETTER, "Letter, 8 1/2×11インチ"}, 
    {"LEGAL", DMPAPER_LEGAL, "Legal, 8 1/2×14インチ"}, 
    {"CSHEET", DMPAPER_CSHEET, "C シート, 17×22インチ"}, 
    {"DSHEET", DMPAPER_DSHEET, "D シート, 22×34インチ"}, 
    {"ESHEET", DMPAPER_ESHEET, "E シート, 34×44インチ"}, 
    {"LETTERSMALL", DMPAPER_LETTERSMALL, "Letter Small, 8 1/2×11インチ"}, 
    {"TABLOID", DMPAPER_TABLOID, "Tabloid, 11×17インチ"}, 
    {"LEDGER", DMPAPER_LEDGER, "Ledger, 17×11インチ"}, 
    {"STATEMENT", DMPAPER_STATEMENT, "Statement, 5 1/2×8 1/2インチ"}, 
    {"EXECUTIVE", DMPAPER_EXECUTIVE, "Executive, 7 1/4×10 1/2インチ"}, 
    {"FOLIO", DMPAPER_FOLIO, "Folio, 8 1/2×13インチ"}, 
    {"QUARTO", DMPAPER_QUARTO, "Quarto, 215×275ミリメートル"}, 
    {"10X14", DMPAPER_10X14, "10×14インチシート"}, 
    {"11X17", DMPAPER_11X17, "11×17インチシート"}, 
    {"NOTE", DMPAPER_NOTE, "Note, 8 1/2×11インチ"}, 
    {"ENV_9", DMPAPER_ENV_9, "#9 Envelope, 3 7/8×8 7/8インチ"}, 
    {"ENV_10", DMPAPER_ENV_10, "#10 Envelope, 4 1/8×9 1/2インチ"}, 
    {"ENV_11", DMPAPER_ENV_11, "#11 Envelope, 4 1/2×10 3/8インチ"}, 
    {"ENV_12", DMPAPER_ENV_12, "#12 Envelope, 4 3/4×11インチ"}, 
    {"ENV_14", DMPAPER_ENV_14, "#14 Envelope, 5×11 1/2インチ"}, 
    {"ENV_DL", DMPAPER_ENV_DL, "DL Envelope, 110×220ミリメートル"}, 
    {"ENV_C5", DMPAPER_ENV_C5, "C5 Envelope, 162×229ミリメートル"}, 
    {"ENV_C3", DMPAPER_ENV_C3, "C3 Envelope, 324×458ミリメートル"}, 
    {"ENV_C4", DMPAPER_ENV_C4, "C4 Envelope, 229×324ミリメートル"}, 
    {"ENV_C6", DMPAPER_ENV_C6, "C6 Envelope, 114×162ミリメートル"}, 
    {"ENV_C65", DMPAPER_ENV_C65, "C65 Envelope, 114×229ミリメートル"}, 
    {"ENV_B4", DMPAPER_ENV_B4, "B4 Envelope, 250×353ミリメートル"}, 
    {"ENV_B5", DMPAPER_ENV_B5, "B5 Envelope, 176×250ミリメートル"}, 
    {"ENV_B6", DMPAPER_ENV_B6, "B6 Envelope, 176×125ミリメートル"}, 
    {"ENV_ITALY", DMPAPER_ENV_ITALY, "Italy Envelope, 110×230ミリメートル"}, 
    {"ENV_MONARCH", DMPAPER_ENV_MONARCH, 
     "Monarch Envelope, 3 7/8×7 1/2インチ"}, 
    {"ENV_PERSONAL", DMPAPER_ENV_PERSONAL, 
     "6 3/4 Envelope, 3 5/8×6 1/2インチ"}, 
    {"FANFOLD_US", DMPAPER_FANFOLD_US, "US Std Fanfold, 14 7/8×11インチ"}, 
    {"FANFOLD_STD_GERMAN", DMPAPER_FANFOLD_STD_GERMAN, 
     "German Std Fanfold, 8 1/2×12インチ"}, 
    {"FANFOLD_LGL_GERMAN", DMPAPER_FANFOLD_LGL_GERMAN, 
     "German Legal Fanfold, 8 1/2×13インチ"}, 
    {NULL, 0, NULL}
};

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
 * 作業用ディレクトリ名を得る。作業ディレクトリ名は書き換えてはいけま
 * せん。
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetTempDirectoryName()
{
    static TCHAR szTempDirName[MAX_PATH] = "\0";
    char *p1;
    DWORD dwFA;
    int i;

    if (NULL == (p1 = getenv("TEMP"))) {
        Syslog(TRUE, "%s#%d: 環境変数TEMPが見つかりません",
               __FILE__, __LINE__);
        return NULL;
    }

    for (i = 0; i < 100; i++) {
        sprintf(szTempDirName, "%s\\ak2prTempDir%d", p1, i);
        if (0xFFFFFFFF == (dwFA = GetFileAttributes(szTempDirName))) {
            if (0 != _mkdir(szTempDirName)) {
                continue;
            }
            break;
        }
        if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
            break;
        }
    }
    if (100 <= i) {
        Syslog(TRUE, "%s#%d: 作業ディレクトリが作成できません",
               __FILE__, __LINE__);
        szTempDirName[0] ='\0';
        return NULL;
    }
    return (LPCTSTR)szTempDirName;
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
    IN OUT LPTSTR lpszFileName                  // 作成した作業ファイル名
    )
{
    static int iCnt = 0;
    FILE *fp;
    time_t t;
    TCHAR szFileName[MAX_PATH];
    LPCTSTR lpszDir;
    int i;

    // ファイル名に * や \ 等が含まれていると、正しいファイルを作成で
    // きないので、適切な文字に置き換える
    for (i = 0; *(lpszFileName + i); i++) {
        // 漢字の一バイト目の場合は次の文字もスキップする
        if (IsKanjiSJIS(*((LPBYTE)lpszFileName + i))) {
            i++;
            continue;
        }
        switch (*(lpszFileName + i)) {
        case '*':
        case '/':
        case ':':
        case '\\':
            *(lpszFileName + i) = '#';
            break;
        case ' ':
            *(lpszFileName + i) = '-';
            break;
        case '.':                               // Distillerが拡張子と
                                                // 判断するので
            *(lpszFileName + i) = '_';
            break;
        }
    }

    if (!(lpszDir = GetTempDirectoryName())) {
        return NULL;
    }

    iCnt++;
    iCnt %= 256;

    t = time(NULL);

    strncpy(szFileName, lpszFileName, MAX_PATH);
    sprintf(lpszFileName, "%s\\%s~%08x_%d_XXXXXX",
            lpszDir, szFileName, t, iCnt);
    if (!_mktemp(lpszFileName)) {
        Syslog(TRUE, "%s#%d: 作業ファイルが作成できません",
               __FILE__, __LINE__);
        return NULL;
    }
    if (NULL == (fp = fopen(lpszFileName, mode))) {
        Syslog(TRUE, "%s#%d: 作業ファイル[%s]をオープンできません(%s)",
               __FILE__, __LINE__, lpszFileName, strerror(0));
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
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("GetModuleHandle", nErr));
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
BOOL WINAPI
ExecutePrtServer(LPCTSTR lpszOption)
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
    sprintf(szImageName, "%s/%s %s", lpszMyDir, SV_EXE_NAME, lpszOption);

    // プロセス情報と起動情報の初期化
    memset(&sui, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    sui.cb = sizeof(STARTUPINFO);
    sui.dwFlags = STARTF_USESHOWWINDOW;
    sui.wShowWindow = SW_SHOWNORMAL;

    if (!CreateProcess(NULL, szImageName, NULL, NULL, FALSE,
                       0, NULL, NULL, &sui, &pi )) {
        int nErr = GetLastError();
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("CreateProcess", nErr));
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
        Syslog(TRUE, "%s#%d: サーバの起動に失敗したようです",
               __FILE__, __LINE__);
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

    if (!ExecutePrtServer(NULL)) {             // プリンタサーバを起動
        return FALSE;                           // サーバが居ない
    }

    // DLLのVarsion不一致を防ぐ為, タイムスタンプを埋める
    strcpy(pPrtInfo->szTimeStamp, TIMESTAMP);
    strcpy(pPrtInfo->szTimeStamp1, TIMESTAMP);

    // サーバのハンドルを得る
    if (!(hWndTo = FindWindow(SV_CLASS, SV_CAPTION))) {
        int nErr = GetLastError();
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("FindWindow", nErr));
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
    int nType,                                  // 印刷データタイプ
    int nOrientation,                           // 用紙の向き
    short dmPaperSize,                          // 用紙サイズ
    int bNum                                    // 行番号の印刷
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
    PrtInfo.nType = nType;
    PrtInfo.fFontSize = fFontSize;
    PrtInfo.nOrientation = nOrientation;
    PrtInfo.dmPaperSize = dmPaperSize;
    PrtInfo.bPreView = FALSE;
    PrtInfo.bNum = bNum;                        // 行番号の印刷(-1:サーバ)

    // 作業ファイルを作成する
    strcpy(PrtInfo.szFileName, PrtInfo.szTitle); 
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
        Syslog(TRUE, "%s", "ファイルサイズが0なのでキャンセルします");
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
    int nType,                                  // 印刷データタイプ
    int nOrientation,                           // 用紙の向き
    short dmPaperSize,                          // 用紙サイズ
    int bNum                                    // 行番号印刷
    )
{
    PRT_INFO PrtInfo;                           // プリントファイル情報
    FILE *fp;                                   // ファイルポインタ
    DWORD dwFA;                                 // ファイル情報

    // ファイルの存在チェックを行う
    if (0xFFFFFFFF == (dwFA = GetFileAttributes(lpszFileName))) {
        LPCTSTR p;
        dwFA = GetLastError();
        p = GetLastErrorMessage("GetFileAttributes()", dwFA);
        Syslog(TRUE, "%s#%d: %s %s", __FILE__, __LINE__, p, lpszFileName);
        return FALSE;
    }

    // ディレクトリの場合はエラー
    if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
        Syslog(TRUE, "%s#%d: %sはディレクトリです",
               __FILE__, __LINE__, lpszFileName);
        return FALSE;
    }

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
    PrtInfo.nType = nType;
    PrtInfo.fFontSize = fFontSize;
    PrtInfo.nOrientation = nOrientation;
    PrtInfo.dmPaperSize = dmPaperSize;
    PrtInfo.bPreView = FALSE;
    PrtInfo.bNum = bNum;                        // 行番号印刷(Booleanではない)

    // 作業ファイルを作成する
    strcpy(PrtInfo.szFileName, PrtInfo.szTitle); 
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }
    fclose(fp);

    // ファイルを複写する
    if (!CopyFile(lpszFileName, PrtInfo.szFileName, FALSE)) {
        Syslog(TRUE, "%s#%d: %s %s", __FILE__, __LINE__,
               GetLastErrorMessage("GetFileAttributes()",
                                   GetLastError()),
               PrtInfo.szFileName);
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * UNIXのSyslogの簡易版。常にdebug.local7しか出力しません。
 * *-------------------------------------------------------------------*/
VOID WINAPI
    Syslog(BOOL bStdOut,                        // T:stdoutにも出力
           LPCSTR lpstr,                        // 書式printfと同じ
           ...                                  // 引数
    )
{
    WSADATA wsaData;
    SOCKET finet;
    PSERVENT sp;
    SOCKADDR_IN sin;
    LPHOSTENT lpHost;
    va_list args;                               // 引数展開用
    char szLine[1024 * 64], szBuf[1024], *p;

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

    if (bStdOut) {
        printf("%s\n", szLine);
    }

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

/*-------------------------------------------------------------------- 
 * 指定可能な用紙サイズの一覧を返却する。
 * 
 * *-------------------------------------------------------------------*/
LPTSTR WINAPI
GetPaperSizeUsageMessage()
{
    int i;
    static TCHAR buf[4096];

    strcpy(buf, "用紙サイズは以下の何れかを指定して下さい。"
           "但し, 全てのオプションが全てのプリンタで"
           "有効とは限りません。\n\n"
           "オプション          説明\n"
           "------------------- ------------------------------\n");
    for (i = 0; devModePs[i].cmdOpt; i++) {
        sprintf(buf + strlen(buf), "%-20s%s\n", 
                devModePs[i].cmdOpt, devModePs[i].sComment);
        if (strlen(buf) > (4096 - 80)) {
            break;
        }
    }
    return buf;
}

/*-------------------------------------------------------------------- 
 * コマンドオプションで指定した用紙サイズから、DEVMODEで使用する用紙サ
 * イズの定数を得る。用紙サイズが不正の場合は0を返却する。
 * *-------------------------------------------------------------------*/
short WINAPI
GetPaperSizeDevMode(LPTSTR cmdOpt)
{
    int i;
    for (i = 0; devModePs[i].cmdOpt; i++) {
        if (0 == stricmp(cmdOpt, devModePs[i].cmdOpt)) {
            return devModePs[i].dmPaperSize;
        }
    }
    return 0;
}
/*--------------------------------------------------------------------
 * DEVMODEで使用する用紙サイズの定数から用紙サイズの説明を得る。
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetPaperSizeComment(short dmPaperSize)
{
    int i;

    if (0 == dmPaperSize) {
        return (LPCTSTR)"デフォルト";
    }
    for (i = 0; devModePs[i].cmdOpt; i++) {
        if (dmPaperSize == devModePs[i].dmPaperSize) {
            return (LPCTSTR)devModePs[i].sComment;
        }
    }
    return (LPCTSTR)"不明";
}
/*--------------------------------------------------------------------
 * PostScript ファイルから、Titleを得る。
 * *-------------------------------------------------------------------*/
#define PS_TITLE_STR "%%Title:"

LPCTSTR WINAPI
    GetPSTitle(LPCTSTR lpszFile,                // PSファイル名
               LPTSTR lpszTitle,                // タイトル格納エリア
               int cbMax                        // 格納最大文字数
               )
{
    FILE *fp;
    TCHAR szBuf[1024], *p;

    memset(lpszTitle, 0, cbMax);

    if (NULL == (fp = fopen(lpszFile, "rt"))) {
        Syslog(TRUE, "%s#%d: PSファイル[%s]をオープンできません(%s)",
               __FILE__, __LINE__, lpszFile, strerror(0));
        return NULL;        
    }

    while (fgets(szBuf, 1024, fp)) {
        if (p = strstr(szBuf, PS_TITLE_STR)) {
            break;
        }
    }
    fclose(fp);
    if (p) {
        strncpy(lpszTitle, p + strlen(PS_TITLE_STR), cbMax);
    }
    else {
        strncpy(lpszTitle, "タイトル不明", cbMax);
    }
    TrimString(lpszTitle);
    Syslog(FALSE, "DBG:GetPSTitle[%s]", lpszTitle);
    return (LPCTSTR)lpszTitle;
}
