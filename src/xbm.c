/* -*- mode: c++ -*-
 * $Id: xbm.c,v 1.2 2005/05/07 12:14:33 tfuruka1 Exp $
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
 * Revision 1.2  2005/05/07 12:14:33  tfuruka1
 * printfで行っていたデバッグをak2pr用のものに変更しました。
 *
 * Revision 1.1  2005/04/30 17:15:08  tfuruka1
 * 新規追加
 *
 */

#include "xbm.h"

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
#else
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // ウインドウハンドル
    int nFlag,                                  // フラグ
    LPCSTR lpstr,                               // 書式printfと同じ
    ...                                         // 引数
    );
#endif

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
    int max = (AXBM_XBM == kind) ? 4 : 6;
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
 * XBM または, uncompface ファイルの情報を展開する。引数で渡されたファ
 * イルポインタは処理完了後クローズします。エラーが発生した場合は, ク
 * ローズしません。
 */
LPXBM_INFO
AllocXBM(FILE *fp, int kind)
{
    int i;
    long c;

    LPXBM_INFO lpXbm = (LPXBM_INFO)malloc(sizeof(XBM_INFO));

    if (!lpXbm) {
        DbgPrint(0, 'D', "%s(%d)メモリ不足(XBM-INFO)", __FILE__, __LINE__);
        return NULL;
    }
    lpXbm->lpData = NULL;
    lpXbm->kind = kind;

    if (AXBM_XBM == kind) {                     // xbm
        lpXbm->nWidth = GetXbmWidth(fp);
        lpXbm->nHeight = GetXbmHeight(fp);
    } else {                                    // uncompface
        lpXbm->nWidth = 48;
        lpXbm->nHeight = 48;
    }
    lpXbm->cbWidth = ((lpXbm->nWidth / 8)
                      + (0 == (lpXbm->nWidth % 8) ? 0 : 1));
    lpXbm->cbData = lpXbm->cbWidth * lpXbm->nHeight;

    lpXbm->lpData = (unsigned char *)malloc(lpXbm->cbData);
    if (!lpXbm->lpData) {
        free(lpXbm);
        DbgPrint(0, 'D', "%s(%d)メモリ不足(XBM-DATA)", __FILE__, __LINE__);
        return NULL;
    }

    if (AXBM_XBM == kind) {                     // xbm
        SkipToFirstByte(fp);

        for (i = 0; i < lpXbm->cbData; i++) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = c & 0xff;
        }
    } else {                                    // uncompface
        for (i = 0; i < lpXbm->cbData; i += 2) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = (c >> 8)& 0xff;
            *(lpXbm->lpData + i + 1) = c & 0xff;
        }
    }
    fclose(fp);

    return lpXbm;
}

/*
 * AllocXBMで取得した情報エリアを開放します。
 */
void
FreeXBM(LPXBM_INFO lpXbm)
{
    if (lpXbm && lpXbm->lpData) {
        free(lpXbm->lpData);
    }
    if (lpXbm) {
        free(lpXbm);
    }
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

    if (AXBM_UFACE == lpXbm->kind) {
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
    FILE *fp;
    int x, y;
    int kind;
    LPXBM_INFO lpXbm;
    HWND hWnd = GetConsoleHwnd();
    HDC hDC = GetDC(hWnd);

    if (3 != argc) {
        printf("Usage: xbm [u | x] FileName\n");
        return 1;
    }

    kind = (*argv[1] == 'u') ? AXBM_UFACE : AXBM_XBM;

    if (!(fp = fopen(argv[2], "rt"))) {
        perror(argv[1]);
        return 1;
    }

    if (!(lpXbm = AllocXBM(fp, kind))) {
        return 1;
    }

    // テキストで描画
    for (y = 0; y < lpXbm->nHeight; y++) {
        for (x = 0; x < lpXbm->nWidth; x++) {
            printf("%c", PeekXBM(lpXbm, x, y) ? 'X': ' ');
        }
        printf("\n");
    }

    // グラフィックで描画
    DrawXBM(lpXbm, hDC, 100, 100, lpXbm->nWidth * 2, lpXbm->nHeight * 2,
            RGB(0, 0, 0), RGB(255, 255, 255), SRCCOPY);
    FreeXBM(lpXbm);

    ReleaseDC(hWnd, hDC);
    return 0;
}
#endif
