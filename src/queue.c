/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-10-01 21:58:39 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v��Queue����
 *
 * $Id: queue.c,v 1.4 2001/10/01 13:20:27 tfuruka1 Exp $
 * $Log: queue.c,v $
 * Revision 1.4  2001/10/01 13:20:27  tfuruka1
 * �p���̌������w��o����悤�ɏC���B
 *
 * Revision 1.3  2001/08/19 04:39:07  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.2  2001/08/18 16:15:10  tfuruka1
 * ��PRT_INFO�\���̂���bDelete�����o���폜�����̂Łi��Ɉ�U��ƃt�@�C��
 *   �ɃR�s�[����悤�ɂ����̂ŁA���̃����o�͕K�v�Ȃ��Ȃ����j�A�Q�Ƃ��Ȃ�
 *   �悤�ɏC���B
 * �����Queue���폜�������ɍ�ƃt�@�C�����폜����Ă��Ȃ������o�O���C��
 *   ����ׂɁADeleteQueue�֐���V�K�ǉ������B
 *
 * Revision 1.1  2001/02/05 17:42:36  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

static LPCTSTR
GetOrientationStr(int nOrientation)
{
    static LPCTSTR lpStr[] = {
        "DEFAULT  ",
        "PORTRAIT ",
        "LANDSCAPE"
    };

    return lpStr[nOrientation % 3];
}

static LPCTSTR
GetNTypeName(int nType)
{
    static struct {
        int nType;
        LPCTSTR lpszName;
    } obj[] = {
        {PT_TEXT, "Plain"},
        {PT_MAIL, "Mail"},
        {PT_PS_ACROBAT, "PostScript(ACROBAT)"},
        {PT_PS_GHOST, "PostScript(GhostScript)"},
        {NUM_OF_PT, NULL}
    };
    int i;

    for (i = 0; obj[i].lpszName; i++) {
        if (nType == obj[i].nType) {
            return obj[i].lpszName;
        }
    }
    return (LPCTSTR)"�s��";
}

static void
AdjustListWidth(HWND hWnd)
{
   HDC hDC;
   SIZE Size;
   int nMaxStrWd, i, j;
   TCHAR szBuf[1024];

    // �ł������s�̃h�b�g���𓾂�
    nMaxStrWd = 0;
    hDC = GetDC(hWnd);                          // ListBox��DC
    j = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
    for (i = 0; i < j; i++) {
        SendMessage(hWnd, LB_GETTEXT, i, (LPARAM)szBuf);
        GetTextExtentPoint32(hDC, szBuf, strlen(szBuf), &Size);
        if (nMaxStrWd < Size.cx) {
            nMaxStrWd = Size.cx;
        }
    }
    ReleaseDC(hWnd, hDC);                       // DC�̊J��
    // ���X�g�{�b�N�X�̕��𒲐�����
    SendMessage(hWnd, LB_SETHORIZONTALEXTENT, (WPARAM)nMaxStrWd + 16, 0);
}

BOOL
EnQueue(
    HWND hWnd,                                  // �n���h��(LIST BOX)
    PPRT_INFO pPrtInfo                          // ������
    )
{
    TCHAR szBuf[1024];
    SYSTEMTIME st;
    int nRet;

    // DLL�̃��@�[�W�����`�F�b�N
    if (0 != strcmp(pPrtInfo->szTimeStamp, pPrtInfo->szTimeStamp1) ||
        0 != strcmp(pPrtInfo->szTimeStamp, TIMESTAMP)) {
        strcpy(szBuf,  "DLL�̃��@�[�W��������v���܂���");
        MessageBox(hWnd, szBuf, __FILE__, MB_ICONSTOP);
        DbgPrint(NULL, 'E', "%s", szBuf);
        return FALSE;
    }

    GetLocalTime(&st);                          // ���ݎ����̎擾
    sprintf(szBuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d %d�i�g"
            " %fPoint TabStop=%d %s %s\t%s",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
            pPrtInfo->nNumOfUp, pPrtInfo->fFontSize, pPrtInfo->nTab,
            GetOrientationStr(pPrtInfo->nOrientation),
            GetNTypeName(pPrtInfo->nType), pPrtInfo->szTitle);
    if (0 > (nRet = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)szBuf))) {
        DbgPrint(hWnd, 'E', "LB_ADDSTRING ERROR (%d)", nRet);
        return FALSE;
    }

    if (LB_ERR == SendMessage(hWnd, LB_SETITEMDATA, nRet, (LPARAM)pPrtInfo)) {
        DbgPrint(hWnd, 'E', "LB_SETITEMDATA ERROR", nRet);
        SendMessage(hWnd, LB_DELETESTRING, nRet, 0);
        return FALSE;
    }

    AdjustListWidth(hWnd);

    DbgPrint(hWnd, 'I', "�ȉ��̒ʂ�Queueing���܂���\n"
             "����t�@�C����: %s\n"
             "����^�C�g��  : %s\n"
             "�i�g��        : %d\n"
             "�^�u��        : %d\n"
             "����^�C�v    : %s\n"
             "�t�H���g�T�C�Y: %f",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize);

    return TRUE;
}

BOOL
DeQueue(
    HWND hWnd,                                  // �n���h��
    PPRT_INFO pPrtInfo                          // ������
    )
{
    PPRT_INFO pPrtInfoTmp;

    // Queue����̏ꍇ�͉������Ȃ�
    if (1 > SendMessage(hWnd, LB_GETCOUNT, 0, 0)) {
        return FALSE;
    }

    // ����������o��
    if (LB_ERR == (int)(pPrtInfoTmp = (PPRT_INFO)SendMessage(
        hWnd, LB_GETITEMDATA, 0, 0))) {
        DbgPrint(hWnd, 'E', "DeQueue�Ɏ��s���܂���");
        return FALSE;
    }

    if (LB_ERR == SendMessage(hWnd, LB_DELETESTRING, 0, 0)) {
        DbgPrint(hWnd, 'E', "DeQueue�Ɏ��s���܂���");
        free(pPrtInfoTmp);
        return FALSE;
    }

    AdjustListWidth(hWnd);

    // ������𕡎ʂ���
    memcpy(pPrtInfo, pPrtInfoTmp, sizeof(PRT_INFO));
    free(pPrtInfoTmp);                          // �������J��

    DbgPrint(hWnd, 'I', "�ȉ��̒ʂ�DeQueue���܂���\n"
             "����t�@�C����: %s\n"
             "����^�C�g��  : %s\n"
             "�i�g��        : %d\n"
             "�^�u��        : %d\n"
             "����^�C�v    : %s\n"
             "�t�H���g�T�C�Y: %f",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize);

    return TRUE;
}

int
DeleteQueue(
    HWND hWnd,                                  // �n���h��
    BOOL bForce                                 // T:�S�č폜
    )
{
    PPRT_INFO pPrtInfoTmp;
    int i;
    int cnt;

    cnt = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
    if (0 > cnt) {
        return 0;
    }
    for (i = 0, cnt--; cnt >= 0; cnt--) {
        if (bForce || SendMessage(hWnd, LB_GETSEL, cnt, 0)) {
            pPrtInfoTmp = (PPRT_INFO)SendMessage(hWnd, LB_GETITEMDATA, cnt, 0);
            SendMessage(hWnd, LB_DELETESTRING, cnt, 0);
            if (LB_ERR == (int)pPrtInfoTmp || !pPrtInfoTmp) {
                MessageBox(hWnd, "����f�[�^�̍폜�Ɏ��s���܂���",
                           "LB_GETITEMDATA",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                continue;
            }
            unlink(pPrtInfoTmp->szFileName);
            DbgPrint(NULL, 'I', "�ȉ��̃f�[�^���폜���܂���\n"
                     "����t�@�C����: %s\n"
                     "����^�C�g��  : %s\n",
                     pPrtInfoTmp->szFileName,
                     pPrtInfoTmp->szTitle);
            free(pPrtInfoTmp);                  // �������J��
            i++;
        }
    }
    AdjustListWidth(hWnd);
    return i;                                   // �폜����ԋp����
}
