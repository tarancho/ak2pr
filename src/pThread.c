/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2001-08-19 12:09:33 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈���X���b�h
 *
 * $Id: pThread.c,v 1.3 2001/08/19 04:34:21 tfuruka1 Exp $
 * $Log: pThread.c,v $
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

        DbgPrint(NULL, 'I', "�f�o�C�X���:%s, %s, %s",
                 (PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                 (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                 (PCHAR)lpDevNames + lpDevNames->wOutputOffset);

        hDC = CreateDC((PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                       (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                       NULL, lpDevMode);
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
        case PT_PS_GHOST:
            DbgPrint(NULL, 'W', "��������Ă܂���");
            break;
        default:
            DbgPrint(NULL, 'E', "PrtInfo.nType�̒l���s���ł�()",
                     g_MailBox.PrtInfo.nType);
        }

        // ��Ɨp�t�@�C�������폜����
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
