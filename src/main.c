/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-29 02:50:43 tfuruka1>
 *
 * 「ak2psのようなもの」のサーバ側メイン処理
 *
 * $Id: main.c,v 1.4 2003/03/29 12:45:28 tfuruka1 Exp $
 * $Log: main.c,v $
 * Revision 1.4  2003/03/29 12:45:28  tfuruka1
 * ● アクセラレータを処理するように対応した。
 *
 * Revision 1.3  2003/03/14 15:23:37  tfuruka1
 * ● 起動オプションを追加した。-S オプションを指定したときに「印刷停止」
 *    状態で動作するようにした。
 *
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
    HACCEL hAcc;                                // アクセラレータ
    HWND hWnd;
    int i;


    // クリティカルセクションの初期化
    InitializeCriticalSection(&g_csCriticalSection);

    // カレントディレクトリをファイルが存在しているディレクトリへ変更
    SetCurrentDirectory(GetMyDir());

    // 起動済みチェックを行う
    if (IsPrtServerEnable()) {
        return 0;
    }

    // ----- 起動オプションのチェック -----
    g_MailBox.bStop = FALSE;

    for (i = 1; i < __argc; i++) {
        if ('-' != **(__argv + i)) {
            break;
        }
        switch (*(*(__argv + i) + 1)) {
        case 'S':
            g_MailBox.bStop = TRUE;
            continue;
        default:
            Syslog(TRUE, "不正なパラメータです: %s", *(__argv + i));
            continue;
        }
    }

    // ウインドウを生々する
    if (!(hWnd = CreateApplication(hInstance))) {
        return 0;
    }

    if (!(hAcc = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACC)))) {
        MessageBox(NULL, "アクセラレータのハンドル取得失敗", "", MB_ERROR);
    }

    // メッセージループ
    while (GetMessage(&msg, NULL, 0, 0)) {
        // アクセラレータの処理を実行し、結果が0の時はメッセージ処理を
        // 行う
        if (!TranslateAccelerator(hWnd, hAcc, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}
