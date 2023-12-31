/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:39:49 tfuruka1>
 *
 * 「ak2psのようなもの」の印刷スレッド
 *
 * $Id: pThread.c,v 1.13 2004/06/18 00:57:56 tfuruka1 Exp $
 * $Log: pThread.c,v $
 * Revision 1.13  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.12  2004/01/19 05:37:58  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.11  2003/03/15 14:41:02  tfuruka1
 * ● プレビューで印刷設定を行った場合に、再度プレビュー画面へ遷移するよ
 * 　 うにした。
 *
 * Revision 1.10  2003/03/14 15:25:37  tfuruka1
 * ● Distiller用のファイルを生成するときに、拡張子に.PSを付加するように
 *    した。
 *
 * Revision 1.9  2001/12/18 04:05:07  tfuruka1
 * プレビューキャンセル後にワークファイルが残っていた問題を修正。
 *
 * Revision 1.8  2001/12/14 17:08:43  tfuruka1
 * プレビュー対応。もともとプレビューなんて考慮していなかったので、とても
 * 汚い処理になってしまった。
 *
 * Revision 1.7  2001/12/07 18:23:57  tfuruka1
 * 用紙サイズの指定を出来るようにした。
 *
 * Revision 1.6  2001/12/06 14:01:17  tfuruka1
 * DEVMODE構造体の複写が完全でなかった問題を修正。sizeof(DEVMODE)から、
 * dmDriverExtra + dmSizeへ変更した。(環境によっては、CreateDCで落ちる事
 * があったので)
 *
 * Revision 1.5  2001/10/01 13:20:05  tfuruka1
 * 用紙の向きを指定出来るように修正。
 *
 * Revision 1.4  2001/08/19 08:54:28  tfuruka1
 * PostScriptファイルを印刷するときにGhostScriptを呼び出せるようにした。
 *
 * Revision 1.3  2001/08/19 04:34:21  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
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

static long WINAPI
ProcessExec(HWND hWnd,
            LPCTSTR lpszCmd,
            LPHANDLE lphProcess,
            LPHANDLE lphThread)
{
    long ret;
    STARTUPINFO stInfo;
    PROCESS_INFORMATION procInfo;

    stInfo.cb = sizeof(STARTUPINFO);
    stInfo.lpReserved = NULL;
    stInfo.lpDesktop = NULL;
    stInfo.lpTitle = NULL;
    stInfo.dwFlags = STARTF_USESHOWWINDOW;
    stInfo.cbReserved2 = 0;
    stInfo.lpReserved2 = NULL;
    stInfo.wShowWindow = SW_MINIMIZE;
    ret = CreateProcess(NULL, (LPTSTR)lpszCmd, NULL, NULL, FALSE, 0, NULL,
                        NULL, &stInfo, &procInfo);

    if (!ret) {
        int nErr = GetLastError();
        DbgPrint(0, 'E', "%s", GetLastErrorMessage(lpszCmd, nErr));
        return 0;
    }
    *lphProcess = procInfo.hProcess;
    *lphThread = procInfo.hThread;
    return procInfo.dwProcessId;
}

static ULONG WINAPI
ProcessWait(long id)
{
    DWORD exitCode;
    HANDLE hProcess;
    long ret;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 1, id);
    do {
        Sleep(100);
        ret = GetExitCodeProcess(hProcess, &exitCode);
    } while (exitCode == STILL_ACTIVE);
    return exitCode;
}

static VOID
PrintPSGhost(VOID)
{
    long idProcess;
    ULONG uExitCode;
    HANDLE hProcess, hThread;
    TCHAR szCom[2048];

    sprintf(szCom, "\"%s\" %s \"%s\"",
            g_MailBox.PrtInfo.szGsPath,
            g_MailBox.PrtInfo.szGsOpt,
            g_MailBox.PrtInfo.szFileName);
    DbgPrint(0, 'I', "プロセス起動:[%s]", szCom);

    if (!(idProcess =
          ProcessExec(g_MailBox.hWnd, szCom, &hProcess, &hThread))) {
        return;
    }

    uExitCode = ProcessWait(idProcess);
    DbgPrint(0, 'I', "プロセス終了: ExitCode=%u", uExitCode);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

static VOID
PrintPSAcrobat(VOID)
{
    TCHAR szAcrFile[MAX_PATH * 2];

    sprintf(szAcrFile, "%s/%s.ps", g_MailBox.PrtInfo.szAcrobat,
            BaseName(g_MailBox.PrtInfo.szFileName));

    DbgPrint(0, 'I', "コピー中...[%s]→[%s]",
             g_MailBox.PrtInfo.szFileName, szAcrFile);
    if (!CopyFile(g_MailBox.PrtInfo.szFileName, szAcrFile, FALSE)) {
        DbgPrint(0, 'E', "コピー失敗: %s",
                 GetLastErrorMessage("CopyFile()", GetLastError()));
        return;
    }
    DbgPrint(0, 'I', "コピー完了");
}

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
    LPDEVMODE lpWkDevMode;                      // プリンタデバイス
    LONG cbDevMode;                             // プリンタデバイスサイズ
    HDC hDC;                                    // プリンタデバイスコンテキスト
    BOOL bSuspend = TRUE;                       // T: 自分をサスペンドする

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
        if (bSuspend) {
            SendMessage(g_MailBox.hWnd, WM_SUSPEND,
                        (WPARAM)g_MailBox.hThread, 0);
        }
        bSuspend = TRUE;
        DbgPrint(NULL, 'I', "印刷Threadを再開しました");

        // プレビューでキャンセルされた場合は、何もしない
        if (!g_MailBox.PrtInfo.valid) {
            DbgPrint(NULL, 'D', "印刷情報無効（意図）");
            continue;
        }

        // ---- ここから印刷処理
        EnterCriticalSection(&g_csCriticalSection);// ロック
        DbgPrint(NULL, 'I', "■ロック");

        lpDevNames = (LPDEVNAMES)GlobalLock(g_MailBox.hDevNames);
        lpDevMode = (LPDEVMODE)GlobalLock(g_MailBox.hDevMode);

        // 用紙の向きを書き換えるので、オリジナルをコピーして、用紙の
        // 向きを設定する
        cbDevMode = lpDevMode->dmSize + lpDevMode->dmDriverExtra;
        if (lpWkDevMode = malloc(cbDevMode)) {
            memcpy(lpWkDevMode, lpDevMode, cbDevMode);

            // 用紙の向きが指定されている場合は設定し直す
            if (g_MailBox.PrtInfo.nOrientation) {
                lpWkDevMode->dmOrientation = g_MailBox.PrtInfo.nOrientation;
            }
            // 用紙サイズが指定されている場合は設定し直す
            if (g_MailBox.PrtInfo.dmPaperSize) {
                lpWkDevMode->dmPaperSize = g_MailBox.PrtInfo.dmPaperSize;
            }

            DbgPrint(NULL, 'I', "デバイス情報:%s, %s, %s",
                     (PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                     (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                     (PCHAR)lpDevNames + lpDevNames->wOutputOffset);

            hDC = CreateDC((PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                           (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                           NULL, lpWkDevMode);
            free(lpWkDevMode);
        }
        else {
            hDC = NULL;
            DbgPrint(NULL, 'E', "メモリ不足(DEVMODE確保)");
        }
        g_MailBox.hDCPrinter = hDC;
        g_MailBox.hDC = NULL;

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

        // デバイスコンテキストの設定
        if (g_MailBox.PrtInfo.bPreView) {
            // プレビュー見る場合は、デバイスコンテキストにプレビュー
            // 用のデバイスコンテキストを設定する
            g_MailBox.hDC = MakePreviewInfo(g_MailBox.hWnd, hDC,
                                            &g_MailBox.PrevInfo);
            // プレビュー用のデバイスコンテキストの作成に失敗した場合
            // は、プリンタデバイスコンテキストを削除して処理終了
            if (!g_MailBox.hDC) {
                DeleteDC(hDC);
                continue;
            }
        }
        else {
            // プレビューを行わない場合はデバイスプリンタのデバイスコ
            // ンテキストを設定する
            g_MailBox.hDC = g_MailBox.hDCPrinter;
        }

        // フォント名からLOGFONTを生成する
        GetLogFont(&g_MailBox.PrtInfo.lfTHF);
        GetLogFont(&g_MailBox.PrtInfo.lfPPF);
        GetLogFont(&g_MailBox.PrtInfo.lfOF);
        GetLogFont(&g_MailBox.PrtInfo.lfOPPF);

        // ───────── 以上で印刷準備完了 ─────────

        // ファイル名が指定されていない場合はテスト印字として処理する
        if (!g_MailBox.PrtInfo.szFileName[0]) {
            DoTestPrint();                      // テストプリント実行

            if (g_MailBox.PrtInfo.bPreView) {
                // プレビューでキャンセルされた場合は、構造体を無効に
                // 設定
                if (PVI_CANCEL == g_MailBox.PrevInfo.status) {
                    g_MailBox.PrtInfo.valid = FALSE;
                }

            }
            goto PostProcess;
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
            goto PostProcess;
        }
        // 指定されたファイルがディレクトリの場合
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            MessageBox(g_MailBox.hWnd,
                       "指定されたファイルはディレクトリです",
                       g_MailBox.PrtInfo.szFileName,
                       MB_ICONSTOP | MB_SETFOREGROUND);
            goto PostProcess;
        }
        FindClose(hFile);

        // 印刷データ毎に処理を振り分ける
        switch (g_MailBox.PrtInfo.nType) {
        case PT_MAIL:
            PrintMail();                        // メール印刷処理
            break;
        case PT_TEXT:
            PrintText();                        // テキスト印刷処理
            break;
        case PT_PS_ACROBAT:                     // PostScript(Acrobat)
            PrintPSAcrobat();
            g_MailBox.PrtInfo.valid = FALSE;    // プレビューは有り得ない
            break;
        case PT_PS_GHOST:                       // PostScript(GhostScript)
            PrintPSGhost();
            g_MailBox.PrtInfo.valid = FALSE;    // プレビューは有り得ない
            break;
        default:
            DbgPrint(NULL, 'E', "PrtInfo.nTypeの値が不正です()",
                     g_MailBox.PrtInfo.nType);
        }

        if (g_MailBox.PrtInfo.valid && g_MailBox.PrtInfo.bPreView) {
            // プレビューでキャンセルされた場合は、構造体を無効に
            // 設定
            if (PVI_CANCEL == g_MailBox.PrevInfo.status) {
                g_MailBox.PrtInfo.valid = FALSE;
            }
            else {
                // プレビューがキャンセルされていない場合は、次の回の
                // 処理で、印刷を行うので、ファイルは削除しない
                goto PostProcess;
            }
        }

        // 作業用ファイルを削除する
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
    PostProcess:
        DeleteDC(hDC);
        if (g_MailBox.PrtInfo.bPreView) {
            // プレビューで印刷設定が選択された場合は印刷設定を行い、
            // 印刷を継続する
            if (PVI_SETUP != g_MailBox.PrevInfo.status) {
                // キャンセルされた場合又は印刷を行う場合
                g_MailBox.PrtInfo.bPreView = FALSE;
            }
            DeleteDC(g_MailBox.PrevInfo.hDC);
            DeleteObject(g_MailBox.PrevInfo.hBitmap);
            bSuspend = FALSE;

        }
        DbgPrint(NULL, 'I', "印刷スレッド後処理完了");
    }
}
