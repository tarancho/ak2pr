/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-29 21:41:05 tfuruka1>
 *
 * 「ak2psのようなもの」のクリップボード関係の処理
 *
 * $Id: clipboard.c,v 1.1 2004/06/17 15:01:06 tfuruka1 Exp $
 * $Log: clipboard.c,v $
 * Revision 1.1  2004/06/17 15:01:06  tfuruka1
 * Initial revision
 *
 * Revision 1.1  2003/03/29 12:41:05  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

/* -------------------------------------------------------------------
 * クリップボードの内容をファイルに出力する
 * *-----------------------------------------------------------------*/
BOOL WINAPI
    ReadClipBoardToFP(HWND hWnd, // クリップボードをオープンするウイン
                                 // ドウ：NULLは現在のタスク
                      FILE *fp                  // ファイルポインタ 
                      )
{
    HGLOBAL hGlobal;
    LPTSTR lpStr;

    // クリップボードにデータが存在しない場合はエラーを返却する
    if(!IsClipboardFormatAvailable(CF_TEXT)) {
        return FALSE;
    }

    // クリップボードのオープンに失敗した場合はエラーを返却する
    if (!OpenClipboard(hWnd)) {
        return FALSE;
    }

    // クリップボードのハンドルを得る
    if (!(hGlobal = (HGLOBAL)GetClipboardData(CF_TEXT))) {
        CloseClipboard();
        return FALSE;
    }

    // メモリロック
    if (!(lpStr = (LPSTR)GlobalLock(hGlobal))) {
        CloseClipboard();
        return FALSE;
    }

    // ファイルに出力する
    for (; *lpStr; lpStr++) {
        if ('\r' == *lpStr) {                   // 復帰コードは無視 
            continue;
        }
        fprintf(fp, "%c", *lpStr);
    }

    // メモリロック解除
    GlobalUnlock(hGlobal);
    CloseClipboard();
    return TRUE;
}
