/* -*- mode: c++ -*-
 * $Id: xbm.c,v 1.3 2005/05/08 13:03:27 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm, uncompface ファイルの展開等を行ないます。本当は, 元々
 * uncompface を展開するのが目的で, uncompface の -X オプションで生成
 * した xbm ファイルを展開できるのを確認したところで… uncompface の
 * -X オプションって, version によって, サポートされない事が判明してし
 * まい, 急遽, umcompface に対応したのであった。
 *
 * リンク時は、gdi32.lib, user32.lib が必要です。単体デバッグ時は
 * XBM_DEBUG をコンパイルオプションに指定すると、このファイルだけで動
 * 作します。
 *
 *    cl /W3 /Zi /DXBM_DEBUG xbm.c /link gdi32.lib user32.lib
 *
 * $Log: xbm.c,v $
 * Revision 1.3  2005/05/08 13:03:27  tfuruka1
 * X-Face関連の追加
 *
 * Revision 1.2  2005/05/07 12:14:33  tfuruka1
 * printfで行っていたデバッグをak2pr用のものに変更しました。
 *
 * Revision 1.1  2005/04/30 17:15:08  tfuruka1
 * 新規追加
 *
 */

#include "xbm.h"

#define XFACE_STR "X-Face:"
#define PROCESS_TIMEOUT 100                     // プロセスタイムアウト

#if defined(XBM_DEBUG)
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // ウインドウハンドル
    int nFlag,                                  // フラグ
    LPCSTR lpstr,                               // 書式printfと同じ
    ...                                         // 引数
    )
{
    va_list args;                               // 引数展開用
    // 文字列を書式に合せて整形する
    va_start(args, lpstr);
    vprintf(lpstr, args);
    va_end(args);
    printf("\n");
    return NULL;
}

BOOL WINAPI
MakeTempFileAndClose(
    IN const char *mode,                        // モード
    IN OUT LPTSTR lpszFileName                  // 作成した作業ファイル名
    )
{
    FILE *fp;

    sprintf(lpszFileName, "XBM_XXXXXX");
    if (!_mktemp(lpszFileName)) {
        return FALSE;
    }
    fp = fopen(lpszFileName, mode);
    fclose(fp);
    return TRUE;
}

#else
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // ウインドウハンドル
    int nFlag,                                  // フラグ
    LPCSTR lpstr,                               // 書式printfと同じ
    ...                                         // 引数
    );

BOOL WINAPI
MakeTempFileAndClose(
    IN const char *mode,                        // モード
    IN OUT LPTSTR lpszFileName                  // 作成した作業ファイル名
    );

#endif

/*
 * 子プロセスの実行
 */
static int WINAPI
ExecuteProcess(LPTSTR lpCmd)
{
    DWORD exitCode;
    HANDLE hProcess;
    long ret;
    STARTUPINFO stInfo;
    PROCESS_INFORMATION procInfo;
    int i;

    stInfo.cb = sizeof(STARTUPINFO);
    stInfo.lpReserved = NULL;
    stInfo.lpDesktop = NULL;
    stInfo.lpTitle = NULL;
    stInfo.dwFlags = STARTF_USESHOWWINDOW;
    stInfo.cbReserved2 = 0;
    stInfo.lpReserved2 = NULL;
    stInfo.wShowWindow = SW_HIDE;               // 画面表示無し
    ret = CreateProcess(NULL, (LPTSTR)lpCmd, NULL, NULL, FALSE, 0, NULL,
                        NULL, &stInfo, &procInfo);

    if (!ret) {
        int nErr = GetLastError();
        DbgPrint(0, 'E', "%s(%d) コマンド実行失敗： %s", __FILE__, __LINE__,
                 lpCmd);
        return 255;
    }

    // ハンドルは使用しないので閉じる
    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);

    // プロセスの終了を待ち合わせる
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 1, procInfo.dwProcessId);
    for (i = 0; i < PROCESS_TIMEOUT; i++) {
        GetExitCodeProcess(hProcess, &exitCode);
        if (STILL_ACTIVE != exitCode) {
            DbgPrint(0, 'I', "コマンド実行(%s)終了: ExitCode=%d",
                     lpCmd, exitCode);
            CloseHandle(hProcess);
            return exitCode;
        }
        Sleep(100);
    }
    // タイムアウト
    DbgPrint(0, 'W', "プロセスを強制終了します %s", lpCmd);
    TerminateProcess(hProcess, 255);
    CloseHandle(hProcess);
    return 255;
}

/*
 * XBMファイルから、幅、又は高さを得る
 */
static int
GetXbmValue(FILE *fp, char *lpszItem)
{
    char szBuf[1024];
    char *p;
    char *p1;

    // ファイルの先頭へシーク
    if (0 != fseek(fp, 0, SEEK_SET)) {
        perror("SEEK");
        return 0;
    }

    while (fgets(szBuf, 1024, fp)) {
        if ('#' != szBuf[0]) {
            continue;
        }
        if (NULL == (p = strstr(szBuf, lpszItem))) {
            continue;
        }
        if (NULL == (p1 = strrchr(p, ' '))) {
            continue;
        }
        return atoi(p1);
    }
    return 0;
}

/*
 * XBMファイルから、幅を得る
 */
static int
GetXbmWidth(FILE *fp)
{
    return GetXbmValue(fp, "_width ");
}

/*
 * XBMファイルから、高さを得る
 */
static int
GetXbmHeight(FILE *fp)
{
    return GetXbmValue(fp, "_height ");
}

/*
 * XBMファイルのデータ部分の先頭にシークする
 */
static BOOL
SkipToFirstByte(FILE *fp)
{
    int c;

    while (EOF != (c = getc(fp))) {
        if ('{' == c) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * XBMファイルから1バイト分読み込むか、uncompressが出力したデータファ
 * イルから2バイト分読み込む。
 */
static long
GetByte(FILE *fp, int kind)
{
    int c;
    int idx;
    int max = (SETXBM_XBM == kind) ? 4 : 6;
    char szBuf[12];

    // 最初の空白を消去
    while (EOF != (c = getc(fp))) {
        if (!isspace(c)) {
            break;
        }
    }
    idx = 0;
    if (!isdigit(c)) {
        DbgPrint(0, 'D', "%s#(%d)想定外の文字: %c[%02x]",
                 __FILE__, __LINE__, c, c);
        return -1;
    }
    szBuf[idx] = c;
    idx++;
    szBuf[idx] = '\0';
    while (EOF != (c = getc(fp))) {
        if (isspace(c)) {
            continue;
        }
        if (('X' != c) && ('x' != c) && (!isxdigit(c))) {
            return (int)strtol(szBuf, NULL, 16);
        }
        szBuf[idx] = c;
        idx++;
        szBuf[idx] = '\0';
        if (max < idx) {
            DbgPrint(0, 'D', "%s(%d)文字数が長すぎます: %s\n",
                     __FILE__, __LINE__, szBuf);
            return -1;
        }
    }
    DbgPrint(0, 'D', "%s(%d)EOF検出", __FILE__, __LINE__);
    return -1;
}

/*
 * XBM または, uncompface ファイルの情報を展開する。LPXBM_INFOのlpData
 * に展開領域と、cbData にlpDataのバイト数が設定されている必要がありま
 * す。この値を越えるサイズを展開する事はできません。
 */
BOOL WINAPI
SetXBM(LPXBM_INFO lpXbm, LPCTSTR lpszFileName, int kind)
{
    int i;
    long c;
    size_t size;
    FILE *fp;

    if (!(fp = fopen(lpszFileName, "rt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszFileName));
        return FALSE;
    }

    lpXbm->kind = kind;

    if (SETXBM_XBM == kind) {                   // xbm
        lpXbm->nWidth = GetXbmWidth(fp);
        lpXbm->nHeight = GetXbmHeight(fp);
    } else {                                    // uncompface
        lpXbm->nWidth = 48;
        lpXbm->nHeight = 48;
    }
    lpXbm->cbWidth = ((lpXbm->nWidth / 8)
                      + (0 == (lpXbm->nWidth % 8) ? 0 : 1));
    size = lpXbm->cbWidth * lpXbm->nHeight;
    if (size > lpXbm->cbData) {
        DbgPrint(NULL, 'E', "%s(%d) XBMをメモリに展開できません。"
                 "%dバイトの領域が必要ですが、%dバイトしかありません。",
                 __FILE__, __LINE__, size, lpXbm->cbData);
        return FALSE;
    }
    lpXbm->cbData = size;

    if (SETXBM_XBM == kind) {                   // xbm
        SkipToFirstByte(fp);

        for (i = 0; i < (int)lpXbm->cbData; i++) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = c & 0xff;
        }
    } else {                                    // uncompface
        for (i = 0; i < (int)lpXbm->cbData; i += 2) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = (c >> 8)& 0xff;
            *(lpXbm->lpData + i + 1) = c & 0xff;
        }
    }
    fclose(fp);

    return TRUE;
}

/*
 * XBM情報の座標の内容を返却します。
 */
int
PeekXBM(LPXBM_INFO lpXbm, int x, int y)
{
    int idx = y * (lpXbm->cbWidth) + (x / 8);
    int shift = (x % 8);
    int c = *(lpXbm->lpData + idx);

    if (SETXBM_UFACE == lpXbm->kind) {
        shift = 7 - shift;
    }
    return (c >> shift) & 1;
}

/*
 * XBM情報の内容を描画します。
 */
BOOL WINAPI
DrawXBM(
    LPXBM_INFO lpXbm,                           // xbm情報
    HDC hDC,                                    // 描画するDC
    int x,                                      // 描画座標
    int y,                                      // 描画座標
    int nWidth,                                 // 描画幅
    int nHeight,                                // 描画高
    COLORREF crFg,                              // 全景色
    COLORREF crBg,                              // 背景色
    DWORD dwRop                              // ラスターオペレーション
    )
{
    HDC hMemDC;                        // デバイスコンテキスト（仮想）
    HBITMAP hBitMap;                            // ビットマップ
    HBITMAP hOldBitmap;                         // 古いビットマップ
    int xx, yy;

    // 仮想デバイスコンテキストを作成する
    if (!(hMemDC = CreateCompatibleDC(hDC))) {
        DbgPrint(0, 'D', "%s(%d)仮想デバイスコンテキスト作成失敗",
                 __FILE__, __LINE__);
        return FALSE;
    }

    // ビットマップの作成
    if (!(hBitMap = CreateBitmap(lpXbm->nWidth,
                                 lpXbm->nHeight, 1, 1, NULL))) {
        DbgPrint(0, 'D', "%s(%d)ビットマップの作成に失敗しました",
                 __FILE__, __LINE__);
        DeleteDC(hMemDC);                  // デバイスコンテキスト削除
        return FALSE;
    }

    // ビットマップの選択
    if (!(hOldBitmap = SelectObject(hMemDC, hBitMap))) {
        DbgPrint(0, 'D', "%s(%d)ビットマップの選択に失敗しました。",
                 __FILE__, __LINE__);
        DeleteDC(hMemDC);                  // デバイスコンテキスト削除
        DeleteObject(hBitMap);                  // ビットマップ削除
        return FALSE;
    }

    // XBMの描画
    for (yy = 0; yy < lpXbm->nWidth; yy++) {
        for (xx = 0; xx < lpXbm->nHeight; xx++) {
            SetPixel(hMemDC, xx, yy,
                     PeekXBM(lpXbm, xx, yy) ? crFg : crBg);
        }
    }

    // XBMの転送
    StretchBlt(hDC, x, y, nWidth, nHeight,
               hMemDC, 0, 0, lpXbm->nWidth, lpXbm->nHeight, dwRop);

    // 後始末
    SelectObject(hMemDC, hOldBitmap);           // ビットマップを戻す
    DeleteObject(hBitMap);                      // ビットマップ削除
    DeleteDC(hMemDC);                           // 仮想DC削除

    return TRUE;
}

/*
 * メールファイルからX-FACE部分を切り出し、ファイルに出力する
 */
static BOOL WINAPI
CutXFACEToFile(
    LPTSTR lpszInFile,                          // メールファイル
    LPTSTR lpszOutFile                          // X-FACE出力先
    )
{
    FILE *fpIn;
    FILE *fpOut;
    int cbXFace = strlen(XFACE_STR);
    BOOL bFind = FALSE;
    TCHAR szBuf[1024];

    if (!(fpIn = fopen(lpszInFile, "rt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszInFile));
        return FALSE;
    }
    if (!(fpOut = fopen(lpszOutFile, "wt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszOutFile));
        fclose(fpIn);
        return FALSE;
    }

    // X-Face:が見付かる迄、読み飛ばす
    while (fgets(szBuf, 1024, fpIn)) {
        if (0 == memicmp(szBuf, XFACE_STR, cbXFace)) {
            bFind = TRUE;
            break;
        }
    }
    if (!bFind) {
        DbgPrint(0, 'I', "%s は存在しません", XFACE_STR);
        fclose(fpIn);
        fclose(fpOut);
        return FALSE;
    }
    fprintf(fpOut, "%s", szBuf + cbXFace);

    while (fgets(szBuf, 1024, fpIn)) {
        // 継続行でなければ終了
        if (' ' != szBuf[0]) {
            break;
        }
        fprintf(fpOut, "%s", szBuf);
    }
    fclose(fpIn);
    fclose(fpOut);
    return TRUE;
}

/*
 * uncompfaceを実行する
 */
BOOL WINAPI
ExecuteUncompface(
    LPTSTR lpszCmdPath,                         // uncompfaceパス
    LPTSTR lpszInFile,                          // メールファイル
    LPTSTR lpszOutFile                          // 出力ファイル
    )
{
    TCHAR szCmd[1024];
    TCHAR szTempFile[MAX_PATH];
    int exitCode;

    strcpy(szTempFile, "XFACE");
    if (!MakeTempFileAndClose("wt", szTempFile)) {
        return FALSE;
    }

    if (!CutXFACEToFile(lpszInFile, szTempFile)) {
        unlink(szTempFile);
        return FALSE;
    }

    sprintf(szCmd, "%s %s %s", lpszCmdPath, szTempFile, lpszOutFile);
    exitCode = ExecuteProcess(szCmd);
    unlink(szTempFile);
    return 0 == exitCode ? TRUE : FALSE;
}

#if defined(XBM_DEBUG) // ━━━━━━━━━━━━━━━━━━━━━━
/*
 * SDK32:コンソールウィンドウのハンドル取得
 *
 * 本関数は以下の記事を参考にして作成しました。
 *
 * 最終更新日: 1999/02/09
 * 文書番号: J046738
 *
 * この資料は以下について記述したものです。
 *
 * Microsoft(R) Win32(R) Software Development Kit (SDK)
 *
 * この資料は、米国 Microsoft Corporation から提供されている Knowledge
 * Base の Article ID Q124103 (最終更新日 1988-12-23) をもとに作成した
 * ものです。
 */
#define MY_BUFSIZE 1024                      // コンソールのタイトル用
HWND GetConsoleHwnd(VOID)
{
    HWND hwndFound;
    char pszNewWindowTitle[MY_BUFSIZE];
    char pszOldWindowTitle[MY_BUFSIZE];
    int i;

    // コンソールタイトルの取得
    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // 独自に、ウィンドウの新規タイトルをフォーマットします
    wsprintf(pszNewWindowTitle,"%d/%d-%s",
             GetTickCount(),
             GetCurrentProcessId(),
             pszOldWindowTitle);

    // 現在のウィンドウタイトルを変更します
    SetConsoleTitle(pszNewWindowTitle);

    for (i = 0; i < 100; i++) {
        // ウィンドウの新規タイトルを探しにいきます
        hwndFound = FindWindow(NULL, pszNewWindowTitle);
        if (hwndFound) {
            DbgPrint(0, 'D', "%s(%d)GetConsoleHwnd(): 回数=%d, T=%s\n",
                     __FILE__, __LINE__, i, pszNewWindowTitle);
            break;                              // 見つかった
        }
        Sleep(10);                              // 10m Wait
    }

    // 元のウィンドウタイトルへ戻します
    SetConsoleTitle(pszOldWindowTitle);

    return hwndFound;
}

int main(int argc, char *argv[])
{
    int x, y;
    int kind;
    LPCTSTR lpszFileName;
    TCHAR szFileName[MAX_PATH];
    HWND hWnd = GetConsoleHwnd();
    HDC hDC = GetDC(hWnd);
    unsigned char xbmBuf[6 * 48];
    XBM_INFO xbmInfo;

    xbmInfo.lpData = xbmBuf;
    xbmInfo.cbData = sizeof(xbmBuf);

    if (3 != argc) {
        printf("Usage: xbm [u | x | m<uncompfacePath>] FileName\n");
        printf(" u - uncompfaceで処理されたファイル\n"
               " x - xbmファイル\n"
               " m - X-FACEが含まれたメールファイル\n");
        return 1;
    }

    kind = (*argv[1] != 'x') ? SETXBM_UFACE : SETXBM_XBM;
    lpszFileName = argv[2];
    if (*argv[1] == 'm') {
        strcpy(szFileName, "X-FACE");
        if (!MakeTempFileAndClose("wt", szFileName)) {
            return 1;
        }
        if (!ExecuteUncompface(argv[1] + 1, argv[2], szFileName)) {
            unlink(szFileName);
            return 1;
        }
        lpszFileName = szFileName;
    }
    if (!SetXBM(&xbmInfo, lpszFileName, kind)) {
        if (*argv[1] == 'm') {
            unlink(lpszFileName);
        }
        return 1;
    }
    if (*argv[1] == 'm') {
        unlink(lpszFileName);
    }

    // テキストで描画
    for (y = 0; y < xbmInfo.nHeight; y++) {
        for (x = 0; x < xbmInfo.nWidth; x++) {
            printf("%c", PeekXBM(&xbmInfo, x, y) ? 'X': ' ');
        }
        printf("\n");
    }

    // グラフィックで描画
    DrawXBM(&xbmInfo, hDC, 100, 100, xbmInfo.nWidth * 2, xbmInfo.nHeight * 2,
            RGB(0, 0, 0), RGB(255, 255, 255), SRCCOPY);

    ReleaseDC(hWnd, hDC);
    return 0;
}
#endif
