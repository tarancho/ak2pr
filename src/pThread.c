/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-08 01:39:29 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈���X���b�h
 *
 * $Id: pThread.c,v 1.7 2001/12/07 18:23:57 tfuruka1 Exp $
 * $Log: pThread.c,v $
 * Revision 1.7  2001/12/07 18:23:57  tfuruka1
 * �p���T�C�Y�̎w����o����悤�ɂ����B
 *
 * Revision 1.6  2001/12/06 14:01:17  tfuruka1
 * DEVMODE�\���̂̕��ʂ����S�łȂ����������C���Bsizeof(DEVMODE)����A
 * dmDriverExtra + dmSize�֕ύX�����B(���ɂ���ẮACreateDC�ŗ����鎖
 * ���������̂�)
 *
 * Revision 1.5  2001/10/01 13:20:05  tfuruka1
 * �p���̌������w��o����悤�ɏC���B
 *
 * Revision 1.4  2001/08/19 08:54:28  tfuruka1
 * PostScript�t�@�C�����������Ƃ���GhostScript���Ăяo����悤�ɂ����B
 *
 * Revision 1.3  2001/08/19 04:34:21  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.2  2001/08/18 16:17:38  tfuruka1
 * ��PRT_INFO�\���̂���bDelete�����o���폜�����̂Łi��Ɉ�U��ƃt�@�C��
 *   �ɃR�s�[����悤�ɂ����̂ŁA���̃����o�͕K�v�Ȃ��Ȃ����j�A�Q�Ƃ��Ȃ�
 *   �悤�ɏC���B
 *
 * Revision 1.1  2001/02/05 17:42:54  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

MAILBOX g_MailBox;                              // �X���b�h�ԃ��[���{�b�N�X

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

    sprintf(szCom, "\"%s\" %s \"%s\"", g_MailBox.szGsPath, g_MailBox.szGsOpt,
            g_MailBox.PrtInfo.szFileName);
    DbgPrint(0, 'I', "�v���Z�X�N��:[%s]", szCom);

    if (!(idProcess =
          ProcessExec(g_MailBox.hWnd, szCom, &hProcess, &hThread))) {
        return;
    }

    uExitCode = ProcessWait(idProcess);
    DbgPrint(0, 'I', "�v���Z�X�I��: ExitCode=%u", uExitCode);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}

static VOID
PrintPSAcrobat(VOID)
{
    TCHAR szAcrFile[MAX_PATH * 2];

    sprintf(szAcrFile, "%s/%s", g_MailBox.szAcrobat,
            BaseName(g_MailBox.PrtInfo.szFileName));

    DbgPrint(0, 'I', "�R�s�[��...[%s]��[%s]",
             g_MailBox.PrtInfo.szFileName, szAcrFile);
    if (!CopyFile(g_MailBox.PrtInfo.szFileName, szAcrFile, FALSE)) {
        DbgPrint(0, 'E', "�R�s�[���s: %s",
                 GetLastErrorMessage("CopyFile()", GetLastError()));
        return;
    }
    DbgPrint(0, 'I', "�R�s�[����");
}

/* -------------------------------------------------------------------
 * ����X���b�h�B�X���b�h�̃T�X�y���h�͎������g�ōs�����A�X���b�h�̃�
 * �W���[���̓��C���v���Z�X�ōs���B�v���Z�X�ƃX���b�h�̓��[���{�b�N�X
 * �ŒʐM���s���B
 * *-----------------------------------------------------------------*/
VOID
PrintThread(LPDWORD lpIDThread)
{
    WIN32_FIND_DATA FindFileData;               // �t�@�C�����
    HANDLE hFile;                               // FindFile�p�̃n���h��
    LPDEVNAMES lpDevNames;
    LPDEVMODE lpDevMode;
    LPDEVMODE lpWkDevMode;                      // �v�����^�f�o�C�X
    LONG cbDevMode;                             // �v�����^�f�o�C�X�T�C�Y
    HDC hDC;                                    // �v�����^�f�o�C�X�R���e�L�X�g

    DbgPrint(NULL, 'I', "���Thread�N������");

    // �v�����^�̐ݒ���s��
    if (!g_MailBox.hDevNames || !g_MailBox.hDevMode) {
        SetupPrinter(g_MailBox.hWnd,
                     &g_MailBox.hDevNames, &g_MailBox.hDevMode);
    }

    // �v�����^�p�̐ݒ肪��������܂ő҂B���SetupPrinterC��OK���N���b
    // �N�����ꍇ�́A�����̃��[�v�͉��Ȃ��B�L�����Z�����N���b�N����
    // �ꍇ�́A���̃X���b�h(���C���X���b�h)�Ńv�����^�̐ݒ肪��������
    // �܂ŁA�҂�������B
    while (!g_MailBox.hDevMode || !g_MailBox.hDevNames) {
        Sleep(100);
    }
    DbgPrint(NULL, 'I', "�v�����^�ݒ芮��");

    // �����~��Ԃ̎��͒�~�����������܂ő҂�
    if (g_MailBox.bStop) {
        Sleep(1000);
    }
    while (TRUE) {
        SendMessage(g_MailBox.hWnd, WM_SUSPEND, (WPARAM)g_MailBox.hThread, 0);
        DbgPrint(NULL, 'I', "���Thread���ĊJ���܂���");

        // ---- ��������������
        EnterCriticalSection(&g_csCriticalSection);// ���b�N
        DbgPrint(NULL, 'I', "�����b�N");

        lpDevNames = (LPDEVNAMES)GlobalLock(g_MailBox.hDevNames);
        lpDevMode = (LPDEVMODE)GlobalLock(g_MailBox.hDevMode);

        // �p���̌���������������̂ŁA�I���W�i�����R�s�[���āA�p����
        // ������ݒ肷��
        cbDevMode = lpDevMode->dmSize + lpDevMode->dmDriverExtra;
        if (lpWkDevMode = malloc(cbDevMode)) {
            memcpy(lpWkDevMode, lpDevMode, cbDevMode);

            // �p���̌������w�肳��Ă���ꍇ�͐ݒ肵����
            if (g_MailBox.PrtInfo.nOrientation) {
                lpWkDevMode->dmOrientation = g_MailBox.PrtInfo.nOrientation;
            }
            // �p���T�C�Y���w�肳��Ă���ꍇ�͐ݒ肵����
            if (g_MailBox.PrtInfo.dmPaperSize) {
                lpWkDevMode->dmPaperSize = g_MailBox.PrtInfo.dmPaperSize;
            }

            DbgPrint(NULL, 'I', "�f�o�C�X���:%s, %s, %s",
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
            DbgPrint(NULL, 'E', "�������s��(DEVMODE�m��)");
        }
        g_MailBox.hDC = hDC;

        GlobalUnlock(g_MailBox.hDevNames);
        GlobalUnlock(g_MailBox.hDevMode);

        DbgPrint(NULL, 'I', "�����b�N����");
        LeaveCriticalSection(&g_csCriticalSection); // ���b�N����

        if (!hDC) {
            int nErr = GetLastError();
            MessageBox(g_MailBox.hWnd,
                       GetLastErrorMessage(__FILE__ " CreateDC()", nErr),
                       g_MailBox.PrtInfo.szFileName, MB_ERROR);
            continue;
        }

        // �t�@�C�������w�肳��Ă��Ȃ��ꍇ�̓e�X�g�󎚂Ƃ��ď�������
        if (!g_MailBox.PrtInfo.szFileName[0]) {
            DoTestPrint();
            DeleteDC(hDC);
            continue;
        }
        //
        // --- �ȉ��̓e�X�g�󎚈ȊO�̏ꍇ�̏���
        //

        // �t�@�C���̑��݃`�F�b�N���s��
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
        // �w�肳�ꂽ�t�@�C�����f�B���N�g���̏ꍇ
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            MessageBox(g_MailBox.hWnd,
                       "�w�肳�ꂽ�t�@�C���̓f�B���N�g���ł�",
                       g_MailBox.PrtInfo.szFileName,
                       MB_ICONSTOP | MB_SETFOREGROUND);
            DeleteDC(hDC);
            continue;
        }
        FindClose(hFile);

        // ����f�[�^���ɏ�����U�蕪����
        switch (g_MailBox.PrtInfo.nType) {
        case PT_MAIL:
            PrintMail();                        // ���[���������
            break;
        case PT_TEXT:
            PrintText();                        // �e�L�X�g�������
            break;
        case PT_PS_ACROBAT:                     // PostScript(Acrobat)
            PrintPSAcrobat();
            break;
        case PT_PS_GHOST:                       // PostScript(GhostScript)
            PrintPSGhost();
            break;
        default:
            DbgPrint(NULL, 'E', "PrtInfo.nType�̒l���s���ł�()",
                     g_MailBox.PrtInfo.nType);
        }

        // ��Ɨp�t�@�C�����폜����
        if (DeleteFile(g_MailBox.PrtInfo.szFileName)) {
            DbgPrint(NULL, 'I', "�t�@�C��[%s]���폜���܂���",
                     g_MailBox.PrtInfo.szFileName);
        }
        else {
            int nErr = GetLastError();
            DbgPrint(NULL, 'E', "�t�@�C��[%s]���폜�o���܂���ł���\n%s",
                     g_MailBox.PrtInfo.szFileName,
                     GetLastErrorMessage("DeleteFile()", nErr));
        }
        DeleteDC(hDC);
    }
}
