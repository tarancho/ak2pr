/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-15 01:44:10 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v��Queue����
 *
 * $Id: queue.c,v 1.6 2001/12/14 17:03:27 tfuruka1 Exp $
 * $Log: queue.c,v $
 * Revision 1.6  2001/12/14 17:03:27  tfuruka1
 * �v���r���[�̏�ԏ�Ԃ�EnQueue/DeQueue�Łi�f�o�b�O�j�\������悤�ɂ����B
 *
 * Revision 1.5  2001/12/08 15:20:08  tfuruka1
 * ���C����ʂ�LISTBOX��ListView�֕ύX�B
 *
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
        "DEFAULT",
        "PORTRAIT",
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

BOOL
EnQueue(
    HWND hWnd,                                  // �n���h��(LIST BOX)
    PPRT_INFO pPrtInfo                          // ������
    )
{
    TCHAR szBuf[1024];
    SYSTEMTIME st;
    LV_ITEM item;
    
    // DLL�̃��@�[�W�����`�F�b�N
    if (0 != strcmp(pPrtInfo->szTimeStamp, pPrtInfo->szTimeStamp1) ||
        0 != strcmp(pPrtInfo->szTimeStamp, TIMESTAMP)) {
        strcpy(szBuf,  "DLL�̃��@�[�W��������v���܂���");
        MessageBox(hWnd, szBuf, __FILE__, MB_ICONSTOP);
        DbgPrint(NULL, 'E', "%s", szBuf);
        return FALSE;
    }

    GetLocalTime(&st);                          // ���ݎ����̎擾

    sprintf(szBuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.pszText = szBuf;
    item.iItem = ListView_GetItemCount(hWnd);
    item.iSubItem = 0;
    item.lParam = (LPARAM)pPrtInfo;
    ListView_InsertItem(hWnd, &item);

    sprintf(szBuf, "%d", pPrtInfo->nNumOfUp);
    item.mask = LVIF_TEXT;
    item.pszText = szBuf;
    item.iSubItem = 1;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%f", pPrtInfo->fFontSize);
    item.pszText = szBuf;
    item.iSubItem = 2;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%d", pPrtInfo->nTab);
    item.pszText = szBuf;
    item.iSubItem = 3;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetOrientationStr(pPrtInfo->nOrientation));
    item.pszText = szBuf;
    item.iSubItem = 4;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetPaperSizeComment(pPrtInfo->dmPaperSize));
    item.pszText = szBuf;
    item.iSubItem = 5;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetNTypeName(pPrtInfo->nType));
    item.pszText = szBuf;
    item.iSubItem = 6;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", pPrtInfo->szTitle);
    item.pszText = szBuf;
    item.iSubItem = 7;
    ListView_SetItem(hWnd, &item);

    DbgPrint(hWnd, 'I', "�ȉ��̒ʂ�Queueing���܂���\n"
             "����t�@�C����: %s\n"
             "����^�C�g��  : %s\n"
             "�i�g��        : %d\n"
             "�^�u��        : %d\n"
             "����^�C�v    : %s\n"
             "�t�H���g�T�C�Y: %f\n"
             "�v���r���[    : %d\n"
             "�A�h���X      : %x",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize,
             pPrtInfo->bPreView,
             pPrtInfo);

    return TRUE;
}

BOOL
DeQueue(
    HWND hWnd,                                  // �n���h��
    PPRT_INFO pPrtInfo                          // ������
    )
{
    PPRT_INFO pPrtInfoTmp;
    LVITEM item;
    TCHAR szBuf[1024];

    // Queue����̏ꍇ�͉������Ȃ�
    if (1 > ListView_GetItemCount(hWnd)) {
        return FALSE;
    }

    // ����������o��
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 0;    
    item.pszText = szBuf;
    item.cchTextMax = 1020;
    if (!ListView_GetItem(hWnd, &item)) {
        DbgPrint(hWnd, 'E', "DeQueue�Ɏ��s���܂���");
        return FALSE;
    }
    pPrtInfoTmp = (PPRT_INFO)item.lParam;
    DbgPrint(hWnd, 'D', "DeQueued: %x", pPrtInfoTmp);

    // ������𕡎ʂ���
    memcpy(pPrtInfo, pPrtInfoTmp, sizeof(PRT_INFO));

    if (!ListView_DeleteItem(hWnd, 0)) {
        DbgPrint(hWnd, 'E', "DeQueue�Ɏ��s���܂���");
        free(pPrtInfoTmp);
        return FALSE;
    }   
    free(pPrtInfoTmp);                          // �������J��

    DbgPrint(hWnd, 'I', "�ȉ��̒ʂ�DeQueue���܂���\n"
             "����t�@�C����: %s\n"
             "����^�C�g��  : %s\n"
             "�i�g��        : %d\n"
             "�^�u��        : %d\n"
             "����^�C�v    : %s\n"
             "�t�H���g�T�C�Y: %f\n"
             "�v���r���[    : %d",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize,
             pPrtInfo->bPreView);

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
    LVITEM item;

    cnt = ListView_GetItemCount(hWnd);		

    if (0 > cnt) {
        return 0;
    }
    for (i = 0, cnt--; cnt >= 0; cnt--) {
        if (bForce || LVIS_SELECTED == ListView_GetItemState(
            hWnd, cnt, LVIS_SELECTED)) {
            item.mask = LVIF_PARAM;
            item.iItem = cnt;
            item.iSubItem = 0;
            if (!ListView_GetItem(hWnd, &item)) {
                MessageBox(hWnd, "����f�[�^�̍폜�Ɏ��s���܂���",
                           "GetItem",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                continue;
            }

            pPrtInfoTmp = (PPRT_INFO)item.lParam;
            if (!ListView_DeleteItem(hWnd, cnt)) {
                MessageBox(hWnd, "����f�[�^�̍폜�Ɏ��s���܂���",
                           "DeleteItem",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                unlink(pPrtInfoTmp->szFileName);
                free(pPrtInfoTmp);
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
    return i;                                   // �폜����ԋp����
}
