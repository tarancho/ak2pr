/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:35:13 tfuruka1>
 * Copyright (C) 2004 Tadamegu Furukawa
 *
 * フォントに関連する定義
 *
 * $Id: fonts.c,v 1.2 2004/06/18 00:57:56 tfuruka1 Exp $
 * $Log: fonts.c,v $
 * Revision 1.2  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.1  2004/01/19 05:32:32  tfuruka1
 * Initial revision
 *
 */

#include "ak2prs.h"

/*
 * フォント列挙コールバック関数
 */
int CALLBACK
EnumFontFamProc(
    ENUMLOGFONT FAR *lpelf,             // pointer to logical-font data
    NEWTEXTMETRIC FAR *lpntm,           // pointer to physical-font data
    int FontType,                       // type of font
    LPARAM lParam                       // address of application-defined data
    )
{
    PFONT_FUNK_INFO pFFI = (PFONT_FUNK_INFO)lParam;

    // データが存在しない場合は処理しない
    if (!lpelf) {
        return 0;
    }

    switch (pFFI->funcNumber) {
    case FFI_FN_SETCOMBO:
        if (FontType & TRUETYPE_FONTTYPE) {
            SendMessage(pFFI->hWnd, CB_ADDSTRING, 0, 
                        (LPARAM)lpelf->elfLogFont.lfFaceName);
        }
        break;
    case FFI_FN_GETLOGFONT:
        memcpy(&pFFI->lf, &lpelf->elfLogFont, sizeof(LOGFONT));
        return 0;
    }

    return 1;
}

/*
 * フォントの一覧をコンボボックスへ設定する
 */
VOID WINAPI
LsFontToCOMBO(HWND hWnd)
{
    FONT_FUNK_INFO ffi;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

    ffi.funcNumber = FFI_FN_SETCOMBO;
    ffi.hWnd = hWnd;

    // コンボボックスの内容を全て消去
    SendMessage(hWnd, CB_RESETCONTENT, 0, 0);

    EnumFontFamilies(hDC, NULL, (FONTENUMPROC)EnumFontFamProc, (LPARAM)&ffi);
    DeleteDC(hDC);
}

/*
 * フォント名から, LOGFONT を作成するフォント名は LOGFONT に予め設定さ
 * れている必要があります。
 */
BOOL WINAPI
GetLogFont(LPLOGFONT lplf)
{
    // デフォルトの LOGFONT
    static LOGFONT lft = {-13, 0, 0, 0, 400, 0, 0, 0, SHIFTJIS_CHARSET, 
                          3, 2, 1, 49, TEXT("MS ゴシック")};
    FONT_FUNK_INFO ffi;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

    ffi.funcNumber = FFI_FN_GETLOGFONT;
    memset(&ffi.lf, 0, sizeof(LOGFONT));
    EnumFontFamilies(hDC, lplf->lfFaceName, 
                     (FONTENUMPROC)EnumFontFamProc, (LPARAM)&ffi);
    DeleteDC(hDC);

    if (ffi.lf.lfFaceName[0]) {
        memcpy(lplf, &ffi.lf, sizeof(LOGFONT));
        return TRUE;
    } else {
        memcpy(lplf, &lft, sizeof(LOGFONT));
        return FALSE;
    }
}
#if 0
int main(int argc, char *argv[])
{
    LsFontToCOMBO(NULL);
    printf("以上\n");
}
#endif
