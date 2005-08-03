/* -*- mode: c++; coding: sjis; -*-
 * Copyright (C) 2005 Tadamegu Furukawa
 * $Id: bitmapfile.c,v 1.2 2005/08/03 10:09:20 tfuruka1 Exp $
 * $Name:  $
 *
 * ビットマップファイル関連
 *
 * +------------------------------------------+
 * | BITMAPFILEHEADER                         |
 * +------------------------------------------+
 * | BITMAPINFO or BITMAPCOREINFO             |
 * | +----------------------------------------+
 * | | BITMAPCOREHEADER or BITMAPINFOHEADER or|
 * | | BITMAPV4HEADER or BITMAPV5HEADER       |
 * | +----------------------------------------+
 * | | Color table(Optional)                  |
 * +-+----------------------------------------+
 * | Bitmap data                              |
 * +------------------------------------------+
 * | Profile data(Optional)                   |
 * +------------------------------------------+
 *
 * 単体評価の方法：
 *
 *   以下の方法でコンパイルします。
 *
 *      cl /W3 /Zi /DDEBUG bitmapfile.c /link gdi32.lib user32.lib
 *
 *   あとは、コマンドラインから
 *
 *     bitmapfile.exe bmpFile outputFile
 *
 *   で第一引数で指定したビットマップを画面に表示し、その情報を第二引
 *   数で指定したファイルに出力します。
 *
 * $Log: bitmapfile.c,v $
 * Revision 1.2  2005/08/03 10:09:20  tfuruka1
 * メール印刷でFaceを印刷できるようにしました。
 *
 * Revision 1.1  2005/08/03 03:43:42  tfuruka1
 * 新規追加(Face対応の前準備)
 *
 */

#include "bitmapfile.h"

#define DBG_FMT(format) "%s(%d) : " format, __FILE__, __LINE__

/* 
 * ビットマップファイルを読み込み, ビットマップハンドルを作成します。
 * 成功した場合は, BITMAP ハンドルを返却します。失敗した場合は NULL を
 * 返却します。返却したビットマップが不要になったら, DeleteObject 関数
 * で削除してください。
 *
 * カラーパレットには対応していません。
 *
 */
HBITMAP WINAPI
LoadBitMapFile(
    HDC hDC,                                    // このデバイスと互換
                                                // 性のあるビットマッ
                                                // プが作成されます
    LPCTSTR lpszBmFile                          // ファイル名
    )
{
    FILE *fp;
    HBITMAP hBm = NULL;
    BITMAPFILEHEADER bmih;
    LPBITMAPINFO lpBmInfo = NULL;
    size_t cbBmInfo;
    LPVOID pColor = NULL;

    if (!(fp = fopen(lpszBmFile, "rb"))) {
        printf(DBG_FMT("%s\n"), _strerror(lpszBmFile));
        return 0;
    }

    if (sizeof(bmih) != fread(&bmih, 1, sizeof(bmih), fp)) {
        printf(DBG_FMT("BITMAPFILEHEADER を読み込めません: %s\n"), lpszBmFile);
        fclose(fp);
        return 0;
    }
    printf("BITMAPFILEHEADER %s : Type=%x(%c%c), Size=%d, Offset=%d\n",
           lpszBmFile,
           bmih.bfType,
           bmih.bfType % 256,
           bmih.bfType / 256,
           bmih.bfSize, 
           bmih.bfOffBits);
    if (0x4d42 != bmih.bfType) {
        printf("%s は有効なビットマップファイルではありません。", lpszBmFile);
        fclose(fp);
        return 0;
    }

    cbBmInfo = bmih.bfSize - sizeof(BITMAPFILEHEADER);
    printf("BITMAP情報サイズ: %d\n", cbBmInfo);

    if (!(lpBmInfo = (LPBITMAPINFO)malloc(cbBmInfo))) {
        printf(DBG_FMT("BITMAP情報を展開するメモリを確保できません。\n"));
        fclose(fp);
        return 0;
    }

    if (cbBmInfo != fread(lpBmInfo, 1, cbBmInfo, fp)) {
        printf(DBG_FMT("BITMAP情報を読み込めません: %s\n"), lpszBmFile);
        fclose(fp);
        return 0;
    }
    fclose(fp);

    pColor = ((LPSTR)lpBmInfo + (WORD)(lpBmInfo->bmiHeader.biSize)); 

    switch (lpBmInfo->bmiHeader.biSize) {
    case sizeof(BITMAPCOREHEADER):
        printf("BITMAPCOREHEADER: PCOLOR=%x\n", pColor);
        break;
    case sizeof(BITMAPINFOHEADER):
        printf("BITMAPINFOHEADER: PCOLOR=%x\n", pColor);
        break;
    case sizeof(BITMAPV4HEADER):
        printf("BITMAPV4HEADER: PCOLOR=%x\n", pColor);
        break;
    default:
        printf(DBG_FMT("BITMAPINFOHEADERの種別を特定できません\n"));
        free(lpBmInfo);
        return 0;
    }
    // カラーパレットに対応しようと思ったけど、とりあえず中止です。

    if (!(hBm = CreateDIBitmap(
              hDC,
              (LPBITMAPINFOHEADER)lpBmInfo,
              CBM_INIT,
              (LPBYTE)lpBmInfo + bmih.bfOffBits - sizeof(BITMAPFILEHEADER),
              lpBmInfo,
              DIB_RGB_COLORS))) {
        printf(DBG_FMT("BITMAPの作成に失敗しました\n"));
        free(lpBmInfo);
        return NULL;
    }
    free(lpBmInfo);
    return hBm;
}

/*
 * ビットマップをビットマップファイルに保存します。
 */
BOOL WINAPI
SaveBitmapFile(
    HDC hDC,
    HBITMAP hBm,
    LPCTSTR lpszSaveFileName
    )
{
    FILE *fp;
    BITMAP bm;
    BITMAPFILEHEADER bmfh;
    BITMAPINFO bmi;
    LPBYTE lpBits;

    if (!GetObject(hBm, sizeof(BITMAP), &bm)) {
        return FALSE;
    }

    memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&bmi, 0, sizeof(BITMAPINFO));

    bmfh.bfType = 'M' * 256 + 'B';
    bmfh.bfSize = sizeof(BITMAPFILEHEADER)
        + sizeof(BITMAPINFO) + bm.bmWidthBytes * bm.bmHeight;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = bm.bmHeight;
    bmi.bmiHeader.biPlanes = bm.bmPlanes;
    bmi.bmiHeader.biBitCount = bm.bmBitsPixel;

    if (!(lpBits = (LPBYTE)malloc(bm.bmWidthBytes * bm.bmHeight))) {
        printf(DBG_FMT("ビットマップを展開するメモリを確保できません\n"));
        return FALSE;
    }

    if (!GetDIBits(hDC, hBm, 0, bm.bmHeight, lpBits, &bmi, DIB_RGB_COLORS)) {
        free(lpBits);
        printf(DBG_FMT("ビット情報の取得に失敗しました\n"));
        return FALSE;
    }

    if (!(fp = fopen(lpszSaveFileName, "wb"))) {
        free(lpBits);
        printf(DBG_FMT("ファイルのオープンに失敗しました: %s\n"),
               _strerror(0));
        return FALSE;
    }
    fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
    fwrite(&bmi, 1, sizeof(BITMAPINFO), fp);
    fwrite(lpBits, 1, bm.bmWidthBytes * bm.bmHeight, fp);
    free(lpBits);
    fclose(fp);
    
    return TRUE;
}
//━━━━━━━━━━━━━━━━━━━━━━━━━━以下は評価用関数
#if defined(DEBUG)
#include <time.h>
int main(int argc, char *argv[])
{
    TCHAR szTitle[1024];
    HBITMAP hBM, hBMOld;
    BITMAP bm;
    HWND hWnd;
    HDC hDC;
    HDC hDCMem;

    sprintf(szTitle, "Test-%d", time(NULL));
    SetConsoleTitle(szTitle);
    Sleep(40);

    hWnd = FindWindow(NULL, szTitle);
    printf("HWND: %x\n", hWnd);
    hDC = GetDC(hWnd);
    hDCMem = CreateCompatibleDC(hDC);
    printf("HDC: %x, %x\n", hDC, hDCMem);

    if (!(hBM = LoadBitMapFile(hDC, argv[1]))) {
        ReleaseDC(hWnd, hDC);
        DeleteDC(hDCMem);
        return 1;
    }

    printf("GetObject: %d\n", GetObject(hBM, sizeof(BITMAP), &bm));
    printf("Size: %d×%d %x\n", bm.bmWidth, bm.bmHeight, bm.bmBits);

    hBMOld = SelectObject(hDCMem, hBM);
    printf("BitBlt: %d\n",
           BitBlt(hDC, 0, 32, bm.bmWidth, bm.bmHeight, hDCMem, 0, 0, SRCCOPY));
    SelectObject(hDCMem, hBMOld);

    SaveBitmapFile(hDCMem, hBM, argv[2]);

    DeleteObject(hBM);

    ReleaseDC(hWnd, hDC);
    DeleteDC(hDCMem);
    return 0;
}

#endif
