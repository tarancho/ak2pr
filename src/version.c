/* -*- mode: c; coding: sjis; -*-
 * $Id: version.c,v 1.2 2005/01/15 12:03:55 tfuruka1 Exp $
 * $Name:  $
 *
 * 「ak2psのようなもの」のヴァージョン情報
 *
 * $Log: version.c,v $
 * Revision 1.2  2005/01/15 12:03:55  tfuruka1
 * 仮想記憶の情報が正しくなさげなので、出力しないようにしました。
 *
 * Revision 1.1  2004/12/23 13:11:56  tfuruka1
 * 新規追加
 *
 */
#include "ak2prs.h"

enum {
    ID_TIMER = 100
};
static UINT idTimerEvent = 0;                   // タイマ

static VOID
SetMemoryInfo(HWND hWnd)
{
    MEMORYSTATUS ms;
    TCHAR szBuf[64];

    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);

    wsprintf(szBuf, "%u", ms.dwMemoryLoad);
    SetDlgItemText(hWnd, IDC_ST_MEM_FREE_RATE, szBuf);

    wsprintf(szBuf, "%u", ms.dwTotalPhys);
    SetDlgItemText(hWnd, IDC_ST_PMEM_TOTAL, szBuf);

    wsprintf(szBuf, "%u", ms.dwAvailPhys);
    SetDlgItemText(hWnd, IDC_ST_PMEM_FREE, szBuf);
    /*
    wsprintf(szBuf, "%u", ms.dwTotalVirtual);
    SetDlgItemText(hWnd, IDC_ST_VMEM_TOTAL, szBuf);

    wsprintf(szBuf, "%u", ms.dwAvailVirtual);
    SetDlgItemText(hWnd, IDC_ST_VMEM_FREE, szBuf);
    */
}

static BOOL
DoInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
    TCHAR szBuf[1024];

    wsprintf(szBuf, "%s\r\n%s", VERSION, COPYRIGHT_STR);

    SetDlgItemText(hWnd, IDC_ST_VERSION, szBuf);
    SetMemoryInfo(hWnd);
    idTimerEvent = SetTimer(hWnd, ID_TIMER, 100, NULL);

    return TRUE;
}

static VOID
DoCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id) {
    case IDOK:
        KillTimer(hWnd, idTimerEvent);
        EndDialog(hWnd, 0);
        break;
    }
}

static VOID CALLBACK
DoTimer(HWND hWnd, DWORD idTimer)
{
    SetMemoryInfo(hWnd);
}

static BOOL CALLBACK
dlgprc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommand);
        HANDLE_MSG(hWnd, WM_TIMER, DoTimer);
    }
    return FALSE;
}

VOID ShowVersion(HWND hWnd)
{
    DialogBox(GetWindowInstance(hWnd), MAKEINTRESOURCE(IDD_VERSION),
              hWnd, dlgprc);
}
