/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-29 21:41:05 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃N���b�v�{�[�h�֌W�̏���
 *
 * $Id: clipboard.c,v 1.1 2004/06/17 15:01:06 tfuruka1 Exp $
 * $Log: clipboard.c,v $
 * Revision 1.1  2004/06/17 15:01:06  tfuruka1
 * Initial revision
 *
 * Revision 1.1  2003/03/29 12:41:05  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

/* -------------------------------------------------------------------
 * �N���b�v�{�[�h�̓��e���t�@�C���ɏo�͂���
 * *-----------------------------------------------------------------*/
BOOL WINAPI
    ReadClipBoardToFP(HWND hWnd, // �N���b�v�{�[�h���I�[�v������E�C��
                                 // �h�E�FNULL�͌��݂̃^�X�N
                      FILE *fp                  // �t�@�C���|�C���^ 
                      )
{
    HGLOBAL hGlobal;
    LPTSTR lpStr;

    // �N���b�v�{�[�h�Ƀf�[�^�����݂��Ȃ��ꍇ�̓G���[��ԋp����
    if(!IsClipboardFormatAvailable(CF_TEXT)) {
        return FALSE;
    }

    // �N���b�v�{�[�h�̃I�[�v���Ɏ��s�����ꍇ�̓G���[��ԋp����
    if (!OpenClipboard(hWnd)) {
        return FALSE;
    }

    // �N���b�v�{�[�h�̃n���h���𓾂�
    if (!(hGlobal = (HGLOBAL)GetClipboardData(CF_TEXT))) {
        CloseClipboard();
        return FALSE;
    }

    // ���������b�N
    if (!(lpStr = (LPSTR)GlobalLock(hGlobal))) {
        CloseClipboard();
        return FALSE;
    }

    // �t�@�C���ɏo�͂���
    for (; *lpStr; lpStr++) {
        if ('\r' == *lpStr) {                   // ���A�R�[�h�͖��� 
            continue;
        }
        fprintf(fp, "%c", *lpStr);
    }

    // ���������b�N����
    GlobalUnlock(hGlobal);
    CloseClipboard();
    return TRUE;
}
