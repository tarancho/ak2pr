/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:41:23 tfuruka1>
 *
 * 「ak2psのようなもの」のサーバ側メイン処理
 *
 * $Id: main.c,v 1.1 2001/02/05 17:41:23 tfuruka1 Exp $
 * $Log: main.c,v $
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
        MessageBox(NULL, "既に起動されています", SV_CAPTION, MB_ERROR);
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
