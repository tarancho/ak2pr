/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-14 21:22:04 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o�����C������
 *
 * $Id: main.c,v 1.3 2003/03/14 15:23:37 tfuruka1 Exp $
 * $Log: main.c,v $
 * Revision 1.3  2003/03/14 15:23:37  tfuruka1
 * �� �N���I�v�V������ǉ������B-S �I�v�V�������w�肵���Ƃ��Ɂu�����~�v
 *    ��Ԃœ��삷��悤�ɂ����B
 *
 * Revision 1.2  2001/12/07 18:21:16  tfuruka1
 * ��d�N���̎��Ƀ_�C�A���O��\������̂��~�߂��B
 *
 * Revision 1.1  2001/02/05 17:41:23  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

CRITICAL_SECTION g_csCriticalSection;           // �N���e�B�J���Z�N�V����

int APIENTRY
WinMain(
    HINSTANCE hInstance,                        // �C���X�^���X
    HINSTANCE hPrevInstance,                    // �ȑO�̃C���X�^���X
    LPTSTR lpszCmdLine,                         // �R�}���h���C��
    int nCmdShow                                // �\�����
    )
{
    MSG msg;                                    // ���b�Z�[�W
    HWND hWnd;
    int i;

    // �N���e�B�J���Z�N�V�����̏�����
    InitializeCriticalSection(&g_csCriticalSection);

    // �J�����g�f�B���N�g�����t�@�C�������݂��Ă���f�B���N�g���֕ύX
    SetCurrentDirectory(GetMyDir());

    // �N���ς݃`�F�b�N���s��
    if (IsPrtServerEnable()) {
        return 0;
    }

    // ----- �N���I�v�V�����̃`�F�b�N -----
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
            Syslog(TRUE, "�s���ȃp�����[�^�ł�: %s", *(__argv + i));
            continue;
        }
    }

    // �E�C���h�E�𐶁X����
    if (!(hWnd = CreateApplication(hInstance))) {
        return 0;
    }

    // ���b�Z�[�W���[�v
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
