/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:41:23 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o�����C������
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

    // �N���e�B�J���Z�N�V�����̏�����
    InitializeCriticalSection(&g_csCriticalSection);

    // �J�����g�f�B���N�g�����t�@�C�������݂��Ă���f�B���N�g���֕ύX
    SetCurrentDirectory(GetMyDir());

    // �N���ς݃`�F�b�N���s��
    if (IsPrtServerEnable()) {
        MessageBox(NULL, "���ɋN������Ă��܂�", SV_CAPTION, MB_ERROR);
        return 0;
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
