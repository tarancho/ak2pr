/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-08 03:20:09 tfuruka1>
 *
 * 「ak2psのようなもの」のサーバ側メイン処理
 *
 * $Id: main.c,v 1.2 2001/12/07 18:21:16 tfuruka1 Exp $
 * $Log: main.c,v $
 * Revision 1.2  2001/12/07 18:21:16  tfuruka1
 * 二重起動の時にダイアログを表示するのを止めた。
 *
 * Revision 1.1  2001/02/05 17:41:23  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

CRITICAL_SECTION g_csCriticalSection;           // クリティカルセクション

int APIENTRY
WinMain(
    HINSTANCE hInstance,                        // インスタンス
    HINSTANCE hPrevInstance,                    // 以前のインスタンス
    LPTSTR lpszCmdLine,                         // コマンドライン
    int nCmdShow                                // 表示状態
    )
{
    MSG msg;                                    // メッセージ
    HWND hWnd;

    // クリティカルセクションの初期化
    InitializeCriticalSection(&g_csCriticalSection);

    // カレントディレクトリをファイルが存在しているディレクトリへ変更
    SetCurrentDirectory(GetMyDir());

    // 起動済みチェックを行う
    if (IsPrtServerEnable()) {
        return 0;
    }

    // ウインドウを生々する
    if (!(hWnd = CreateApplication(hInstance))) {
        return 0;
    }

    // メッセージループ
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
