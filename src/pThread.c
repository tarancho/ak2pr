/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2001-08-19 01:16:53 tfuruka1>
 *
 * 「ak2psのようなもの」の印刷スレッド
 *
 * $Id: pThread.c,v 1.2 2001/08/18 16:17:38 tfuruka1 Exp $
 * $Log: pThread.c,v $
 * Revision 1.2  2001/08/18 16:17:38  tfuruka1
 * ●PRT_INFO構造体からbDeleteメンバを削除したので（常に一旦作業ファイル
 *   にコピーするようにしたので、このメンバは必要なくなった）、参照しない
 *   ように修正。
 *
 * Revision 1.1  2001/02/05 17:42:54  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

MAILBOX g_MailBox;                              // スレッド間メールボックス

/* -------------------------------------------------------------------
 * 印刷スレッド。スレッドのサスペンドは自分自身で行うが、スレッドのリ
 * ジュームはメインプロセスで行う。プロセスとスレッドはメールボックス
 * で通信を行う。
 * *-----------------------------------------------------------------*/
VOID
PrintThread(LPDWORD lpIDThread)
{
    WIN32_FIND_DATA FindFileData;               // ファイル情報
    HANDLE hFile;                               // FindFile用のハンドル
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    HDC hDC;                                    // プリンタデバイスコンテキスト

    DbgPrint(NULL, 'I', "印刷Thread起動完了");

    // プリンタの設定を行う
    if (!g_MailBox.hDevNames || !g_MailBox.hDevMode) {
        SetupPrinter(g_MailBox.hWnd,
                     &g_MailBox.hDevNames, &g_MailBox.hDevMode);
    }

    // プリンタ用の設定が完了するまで待つ。上のSetupPrinterCでOKをクリッ
    // クした場合は、ここのループは回らない。キャンセルをクリックした
    // 場合は、他のスレッド(メインスレッド)でプリンタの設定が完了する
    // まで、待ち続ける。
    while (!g_MailBox.hDevMode || !g_MailBox.hDevNames) {
        Sleep(100);
    }
    DbgPrint(NULL, 'I', "プリンタ設定完了");

    // 印刷停止状態の時は停止が解除されるまで待つ
    if (g_MailBox.bStop) {
        Sleep(1000);
    }
    while (TRUE) {
        SendMessage(g_MailBox.hWnd, WM_SUSPEND, (WPARAM)g_MailBox.hThread, 0);
        DbgPrint(NULL, 'I', "印刷Threadを再開しました");

        // ---- ここから印刷処理
        EnterCriticalSection(&g_csCriticalSection);// ロック
        DbgPrint(NULL, 'I', "■ロック");

        lpDevNames = (LPDEVNAMES)GlobalLock(g_MailBox.hDevNames);
        lpDevMode = (LPDEVMODE)GlobalLock(g_MailBox.hDevMode);

        DbgPrint(NULL, 'I', "デバイス情報:%s, %s, %s",
                 (PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                 (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                 (PCHAR)lpDevNames + lpDevNames->wOutputOffset);

        hDC = CreateDC((PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                       (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                       NULL, lpDevMode);
        g_MailBox.hDC = hDC;

        GlobalUnlock(g_MailBox.hDevNames);
        GlobalUnlock(g_MailBox.hDevMode);

        DbgPrint(NULL, 'I', "■ロック解除");
        LeaveCriticalSection(&g_csCriticalSection); // ロック解除

        if (!hDC) {
            int nErr = GetLastError();
            MessageBox(g_MailBox.hWnd,
                       GetLastErrorMessage(__FILE__ " CreateDC()", nErr),
                       g_MailBox.PrtInfo.szFileName, MB_ERROR);
            continue;
        }

        // ファイル名が指定されていない場合はテスト印字として処理する
        if (!g_MailBox.PrtInfo.szFileName[0]) {
            DoTestPrint();
            DeleteDC(hDC);
            continue;
        }
        //
        // --- 以下はテスト印字以外の場合の処理
        //

        // ファイルの存在チェックを行う
        hFile = FindFirstFile(g_MailBox.PrtInfo.szFileName, &FindFileData);
        if (INVALID_HANDLE_VALUE == hFile) {
            int nErr = GetLastError();
            MessageBox(g_MailBox.hWnd,
                       GetLastErrorMessage(__FILE__ " FindFirstFile()", nErr),
                       g_MailBox.PrtInfo.szFileName,
                       MB_ICONSTOP | MB_SETFOREGROUND);
            DeleteDC(hDC);
            continue;
        }
        // 指定されたファイルがディレクトリの場合
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            MessageBox(g_MailBox.hWnd,
                       "指定されたファイルはディレクトリです",
                       g_MailBox.PrtInfo.szFileName,
                       MB_ICONSTOP | MB_SETFOREGROUND);
            DeleteDC(hDC);
            continue;
        }
        FindClose(hFile);

        // タイプがメールの場合
        if (PT_MAIL == g_MailBox.PrtInfo.nType) {
            PrintMail();                        // メール印刷処理
        }
        else {
            PrintText();                        // テキスト印刷処理
        }

        // ファイル削除指定がある場合はファイルを削除する
        if (DeleteFile(g_MailBox.PrtInfo.szFileName)) {
            DbgPrint(NULL, 'I', "ファイル[%s]を削除しました",
                     g_MailBox.PrtInfo.szFileName);
        }
        else {
            int nErr = GetLastError();
            DbgPrint(NULL, 'E', "ファイル[%s]を削除出来ませんでした\n%s",
                     g_MailBox.PrtInfo.szFileName,
                     GetLastErrorMessage("DeleteFile()", nErr));
        }
        DeleteDC(hDC);
    }
}
