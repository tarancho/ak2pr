/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-08-21 08:30:49 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈󎚐ݒ�
 *
 * �o��
 *
 * �v���p�e�B�V�[�g�̊e�X�̃_�C�A���O�́A*�^�u�I��*�����Ƃ���
 * INIT_DIALOG �̃C�x���g����������B���RWM_CLOSE�́AINIT_DIALOG������
 * �Ă��Ȃ��ꍇ�́A���s����Ȃ��B���̎���m��Ȃ������̂ŁA�s���Ȍ�
 * �ۂ��������Ă��܂����iINIT_DIALOG����ɔ������Ă���Ǝv���Ă����j�B
 *
 * $Id: setup.c,v 1.15 2004/08/21 01:01:01 tfuruka1 Exp $
 * $Log: setup.c,v $
 * Revision 1.15  2004/08/21 01:01:01  tfuruka1
 * �e�L�X�g����ɉ����āu�s�ԁv�Ɓu�r���A���v���L���ɂȂ�悤�ɂ��܂����B
 *
 * Revision 1.14  2004/06/18 00:57:56  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.13  2004/01/19 05:38:35  tfuruka1
 * �t�H���g�����w��o����悤�ɂȂ������Ɋւ���C�����s���܂����B
 *
 * Revision 1.12  2004/01/12 10:00:05  tfuruka1
 * ���ӒԂ��ƒZ�ӒԂ��ɑΉ��������ɂ��A�`�F�b�N�{�b�N�X�����������ɂ��
 * �Ή��B
 *
 * Revision 1.11  2003/03/29 12:49:31  tfuruka1
 * �� �Z�b�g�A�b�v�_�C�A���O�Ɂu�ʐݒ�v�̃^�u��ǉ����܂����B���t�ŏ�
 *    ���Ƃ��ꂾ���ł����ǁA���\�C�����Ă܂��B
 *
 * Revision 1.10  2003/03/15 14:43:43  tfuruka1
 * �� ����ݒ�������i�[����G���A���w��ł���悤�ɕύX�����B�v���r���[
 *    ���̈���ݒ�ɑΉ�����ׂ̏C���ł���B
 *
 * Revision 1.9  2003/03/14 15:29:49  tfuruka1
 * �� PostScript�֘A�̃p�����[�^��PRT_INFO�Ɏ��悤�ɕύX�����B����ɂ��
 *    �āAak2pr�̃R�}���h���C������GhostScript�̃p�X�����w�肪�\�ɂȂ�
 *    �i�����Ή��\��j�B
 *
 * Revision 1.8  2003/03/01 09:08:25  tfuruka1
 * ���y�[�W�ݒ��e-mail�^�u�̓��e���N���A�i����̎菇�ő�����s�����ꍇ�j
 *   �����ꍇ���������̂ŁA�C�������B���v���p�e�B�V�[�g�̃_�C�A���O�́A
 *   �^�u��I����Ԃɂ��Ȃ��ƁAINITDIALOG���������Ȃ����Ƃ�m��Ȃ�������
 *   ���N���B
 * ���y�[�W�ݒ��PostScript�^�u�̓��e���L�����Z����I�����Ă����f����Ă�
 *   �܂������C�������B
 *
 * Revision 1.7  2001/12/23 10:13:56  tfuruka1
 * ���ݒ�_�C�A���O���A��ȏ�\�������i�ł���j�����C�������B
 * ��Copyright�\�����󎚂��Ȃ����[�h���T�|�[�g�����B
 *
 * Revision 1.6  2001/12/18 12:59:04  tfuruka1
 * �f�o�b�O����̃I�v�V������ǉ��B
 *
 * Revision 1.5  2001/12/14 17:09:17  tfuruka1
 * �v���r���[�Ή�
 *
 * Revision 1.4  2001/08/19 08:54:55  tfuruka1
 * PostScript�t�@�C�����������Ƃ���GhostScript���Ăяo����悤�ɂ����B
 *
 * Revision 1.3  2001/08/19 04:33:53  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.2  2001/02/05 17:03:33  tfuruka1
 * �v�����^�̐ݒ���A���j���[����v���p�e�B�V�[�g�́u���ʁv�ֈړ������B
 *
 * Revision 1.1  2001/02/05 16:58:52  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

#define SETUP_CAPTION "����X�^�C���̐ݒ�"
#define GS_DEF_OPT "-dNOPAUSE -dBATCH -sDEVICE=mswinpr2"

PRT_INFO g_PrtInfo;                             // �f�t�H���g������
static PRT_INFO PrtInfoTmp;                     // �������Ɨp

static LPTSTR WINAPI
GetOpenFileNameWrap(
    HWND hWnd,
    LPCTSTR lpszFilter,
    LPCTSTR lpszTitle,
    LPTSTR lpszFileName
    )
{
    OPENFILENAME of;

    of.lStructSize       = sizeof(OPENFILENAME);
    of.hwndOwner         = hWnd;
    of.hInstance         = NULL;
    of.lpstrFilter = lpszFilter;
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter    = 0;
    of.nFilterIndex      = 0;
    of.lpstrFile         = lpszFileName;
    of.nMaxFile          = MAX_PATH;
    of.lpstrFileTitle    = NULL;
    of.nMaxFileTitle     = 0;
    of.lpstrInitialDir   = NULL;
    of.lpstrTitle        = lpszTitle;
    of.Flags             = OFN_HIDEREADONLY;
    of.nFileOffset       = 0;
    of.nFileExtension    = 0;
    of.lpstrDefExt       = NULL;
    of.lCustData         = 0;
    of.lpfnHook          = NULL;
    of.lpTemplateName    = NULL;

    if (!GetOpenFileName(&of)) return NULL;
    return lpszFileName;
}

//
// �������������ȉ��͋��ʐݒ�̃R�[���o�b�N�W�ł���������������������
//
static BOOL CALLBACK
DoInitDialogCom(
    HWND hWnd,                                  // �n���h��
    HWND hWndFocus,                             // �t�H�[�J�X�n���h��
    LPARAM lParam                               // �������p�����[�^
    )
{
    TCHAR szBuf[64];

    // �^�u���I������Ȃ��ƁAINIT_DIALOG������Ȃ�������Ȃ��炵��
    DbgPrint(NULL, 'D', "InitDialog(����)");

    // �^�u���̃Z�b�g
    sprintf(szBuf, "%d", PrtInfoTmp.nTab);
    SetDlgItemText(hWnd, IDC_TABSTOP, szBuf);
    // �t�H���g�T�C�Y�̃Z�b�g
    sprintf(szBuf, "%f", PrtInfoTmp.fFontSize);
    SetDlgItemText(hWnd, IDC_FONTSIZE, szBuf);
    // �i�g���̃Z�b�g
    sprintf(szBuf, "%d", PrtInfoTmp.nNumOfUp);
    SetDlgItemText(hWnd, IDC_NUP, szBuf);

    // �v���r���[�̎��s�L��
    CheckDlgButton(hWnd, IDC_CHK_PREVIEW, PrtInfoTmp.bPreView ? TRUE : FALSE);

    // �f�o�b�O����̎��s�L��
    CheckDlgButton(hWnd, IDC_C_DEBUG, PrtInfoTmp.bDebug ? TRUE : FALSE);

    // Copyright���̈���L��
    CheckDlgButton(hWnd, IDC_CHK_NOCOPYRIGHT,
                   PrtInfoTmp.bNoCopyright ? TRUE : FALSE);

    // �Z�ӒԂ��̗L��
    CheckDlgButton(hWnd, IDC_CHK_SHORT_BINDING,
                   PrtInfoTmp.bShortBinding ? TRUE : FALSE);

    // �t�H���g�I���R���{�{�b�N�X�̏�����
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_THF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_PPF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_OF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_OPPF));

    // �f�t�H���g�t�H���g�̑I��
    SendDlgItemMessage(hWnd, IDC_COMBO_THF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfTHF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_PPF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfPPF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_OF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfOF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_OPPF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfOPPF.lfFaceName);

    return TRUE;
}

static VOID
DoCommandCom(
    HWND hWnd,                                  // �n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���̃n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    TCHAR szBuf[64];
    int i;
    double d;

    switch (id) {                               // �R���g���[���ԍ�
    case IDC_TABSTOP:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            i = (int)strtoul(szBuf, NULL, 10);
            if (0 >= i || 32 < i) {
                i = 8;
            }
            sprintf(szBuf, "%d", i);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.nTab = i;
        }
        break;
    case IDC_FONTSIZE:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            d = strtod(szBuf, NULL);
            if (2 >= d || 48 < d) {
                d = 10.0;
            }
            sprintf(szBuf, "%f", d);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.fFontSize = d;
        }
        break;
    case IDC_NUP:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            i = (int)strtoul(szBuf, NULL, 10);
            if (0 >= i || 10 < i) {
                i = 1;
            }
            sprintf(szBuf, "%d", i);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.nNumOfUp = i;
        }
        break;
    case IDC_CHK_PREVIEW:
        PrtInfoTmp.bPreView = IsDlgButtonChecked(hWnd, IDC_CHK_PREVIEW);
        break;
    case IDC_CHK_SHORT_BINDING:
        PrtInfoTmp.bShortBinding = IsDlgButtonChecked(hWnd,
                                                      IDC_CHK_SHORT_BINDING);
        break;
    case IDC_CHK_NOCOPYRIGHT:
        PrtInfoTmp.bNoCopyright
            = IsDlgButtonChecked(hWnd, IDC_CHK_NOCOPYRIGHT);
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.nTab = 8;
        PrtInfoTmp.nNumOfUp = 2;
        PrtInfoTmp.fFontSize = 10.0;

        sprintf(szBuf, "%d", PrtInfoTmp.nTab);
        SetDlgItemText(hWnd, IDC_TABSTOP, szBuf);

        sprintf(szBuf, "%d", PrtInfoTmp.nNumOfUp);
        SetDlgItemText(hWnd, IDC_NUP, szBuf);

        sprintf(szBuf, "%f", PrtInfoTmp.fFontSize);
        SetDlgItemText(hWnd, IDC_FONTSIZE, szBuf);

        PrtInfoTmp.bPreView = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_PREVIEW, PrtInfoTmp.bPreView);

        PrtInfoTmp.bShortBinding = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_SHORT_BINDING, PrtInfoTmp.bShortBinding);

        PrtInfoTmp.bNoCopyright = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_NOCOPYRIGHT, PrtInfoTmp.bNoCopyright);

        // �t�H���g�̑I��
        strcpy(PrtInfoTmp.lfTHF.lfFaceName, FN_MSM);
        strcpy(PrtInfoTmp.lfPPF.lfFaceName, FN_MSPG);
        strcpy(PrtInfoTmp.lfOF.lfFaceName, FN_COU);
        strcpy(PrtInfoTmp.lfOPPF.lfFaceName, FN_ARIAL);
        SendDlgItemMessage(hWnd, IDC_COMBO_THF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfTHF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_PPF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfPPF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_OF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfOF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_OPPF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfOPPF.lfFaceName);
        break;
    case IDC_PRINTER:
        SetupPrinter(hWnd, &g_MailBox.hDevNames, &g_MailBox.hDevMode);
        break;
    }
}

static VOID
DoCloseCom(HWND hWnd)
{
    TCHAR szBuf[64];
    int i;
    double d;

    DbgPrint(NULL, 'D', "Close(����)");

    GetDlgItemText(hWnd, IDC_TABSTOP, szBuf, 10);
    i = (int)strtoul(szBuf, NULL, 10);
    if (0 >= i || 32 < i) {
        i = 8;
    }
    PrtInfoTmp.nTab = i;

    GetDlgItemText(hWnd, IDC_FONTSIZE, szBuf, 10);
    d = strtod(szBuf, NULL);
    if (2 >= d || 48 < d) {
        d = 10.0;
    }
    PrtInfoTmp.fFontSize = d;

    GetDlgItemText(hWnd, IDC_NUP, szBuf, 10);
    i = (int)strtoul(szBuf, NULL, 10);
    if (0 >= i || 10 < i) {
        i = 1;
    }
    PrtInfoTmp.nNumOfUp = i;

    PrtInfoTmp.bPreView = IsDlgButtonChecked(hWnd, IDC_CHK_PREVIEW);
    PrtInfoTmp.bDebug = IsDlgButtonChecked(hWnd, IDC_C_DEBUG);
    PrtInfoTmp.bNoCopyright = IsDlgButtonChecked(hWnd, IDC_CHK_NOCOPYRIGHT);
    PrtInfoTmp.bShortBinding = IsDlgButtonChecked(hWnd, IDC_CHK_SHORT_BINDING);

    // �t�H���g
    GetDlgItemText(hWnd, IDC_COMBO_THF, PrtInfoTmp.lfTHF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_PPF, PrtInfoTmp.lfPPF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_OF, PrtInfoTmp.lfOF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_OPPF, PrtInfoTmp.lfOPPF.lfFaceName,
                   LF_FACESIZE);

    DbgPrint(NULL, 'I', "���ʐݒ�I��\nTABSTOP:%d\nFONTSIZE:%f\n"
             "NUP:%d\nPREVIE:%d\nCOPYRIGHT:%d\nSHORTBIND:%d\n"
             "TH=%s,PP=%s,OTHER=%s, OTHER(PP)=",
             PrtInfoTmp.nTab, PrtInfoTmp.fFontSize, PrtInfoTmp.nNumOfUp,
             PrtInfoTmp.bPreView, PrtInfoTmp.bNoCopyright,
             PrtInfoTmp.bShortBinding,
             PrtInfoTmp.lfTHF.lfFaceName,
             PrtInfoTmp.lfPPF.lfFaceName,
             PrtInfoTmp.lfOF.lfFaceName,
             PrtInfoTmp.lfOPPF.lfFaceName);
}

static BOOL
DialogProcCom(
    HWND hWnd,                                  // �_�C�A���O�{�b�N�X�̃n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // ��1���b�Z�[�W �p�����[�^
    LPARAM lParam                               // ��2���b�Z�[�W �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogCom);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandCom);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseCom);
    }
    return FALSE;    
}

//
// �������������ȉ��̓��[������̃R�[���o�b�N�W�ł���������������������
//
static BOOL CALLBACK
DoInitDialogMail(
    HWND hWnd,                                  // �n���h��
    HWND hWndFocus,                             // �t�H�[�J�X�n���h��
    LPARAM lParam                               // �������p�����[�^
    )
{
    DbgPrint(NULL, 'D', "InitDialog(e-mail)");

    // �J���[����̃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_COLOR, PrtInfoTmp.bColor ? TRUE : FALSE);
    // Received�w�b�_�̈󎚃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_NORHEAD,
                   PrtInfoTmp.bNoRcvHeader ? TRUE : FALSE);

    return TRUE;
}

static VOID
DoCommandMail(
    HWND hWnd,                                  // �n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���̃n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    switch (id) {                               // �R���g���[���ԍ�
    case IDC_C_COLOR:
        // Color����̎w��
        PrtInfoTmp.bColor = IsDlgButtonChecked(hWnd, IDC_C_COLOR);
        break;
    case IDC_C_NORHEAD:
        // Received�w�b�_�̈󎚎w��
        PrtInfoTmp.bNoRcvHeader = IsDlgButtonChecked(hWnd, IDC_C_NORHEAD);
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.bColor = TRUE;
        PrtInfoTmp.bNoRcvHeader = TRUE;
        CheckDlgButton(hWnd, IDC_C_COLOR, PrtInfoTmp.bColor);
        CheckDlgButton(hWnd, IDC_C_NORHEAD, PrtInfoTmp.bNoRcvHeader);
        break;
    }
}

static void
DoCloseMail(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(e-mail)");

    // Color����̎w��
    PrtInfoTmp.bColor = IsDlgButtonChecked(hWnd, IDC_C_COLOR);
    // Received�w�b�_�̈󎚎w��
    PrtInfoTmp.bNoRcvHeader = IsDlgButtonChecked(hWnd, IDC_C_NORHEAD);
}

static BOOL
DialogProcMail(
    HWND hWnd,                                  // �_�C�A���O�{�b�N�X�̃n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // ��1���b�Z�[�W �p�����[�^
    LPARAM lParam                               // ��2���b�Z�[�W �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogMail);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandMail);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseMail);
    }
    return FALSE;    
}
//
// �������������ȉ��̓e�L�X�g����̃R�[���o�b�N�W�ł���������������������
//
static BOOL CALLBACK
DoInitDialogText(
    HWND hWnd,                                  // �n���h��
    HWND hWndFocus,                             // �t�H�[�J�X�n���h��
    LPARAM lParam                               // �������p�����[�^
    )
{
    DbgPrint(NULL, 'D', "InitDialog(Text)");

    // �r���A���̃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_KEISEN, PrtInfoTmp.bKeisen ? TRUE : FALSE);
    // �s�ԍ��̃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_NUM, PrtInfoTmp.bNum ? TRUE : FALSE);
    // �x�[�X���C��
    CheckRadioButton(hWnd, IDC_R_JAPAN, IDC_R_AUTO, PrtInfoTmp.nBaseLine);

    return TRUE;
}

static VOID
DoCommandText(
    HWND hWnd,                                  // �n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���̃n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    switch (id) {                               // �R���g���[���ԍ�
    case IDC_R_NOINTERLINE:
    case IDC_R_ENGLISH:
    case IDC_R_JAPAN:
    case IDC_R_AUTO:
        PrtInfoTmp.nBaseLine = id;
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.bNum = TRUE;
        PrtInfoTmp.nBaseLine = IDC_R_ENGLISH;
        PrtInfoTmp.bKeisen = FALSE;

        CheckDlgButton(hWnd, IDC_C_KEISEN, FALSE);
        CheckDlgButton(hWnd, IDC_C_NUM, TRUE);
        CheckDlgButton(hWnd, IDC_R_ENGLISH, TRUE);
        // �x�[�X���C��
        CheckRadioButton(hWnd, IDC_R_JAPAN, IDC_R_AUTO, PrtInfoTmp.nBaseLine);
        break;
    }
}

static void
DoCloseText(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(text)");

    // �r���A��
    PrtInfoTmp.bKeisen = IsDlgButtonChecked(hWnd, IDC_C_KEISEN);
    // �s�ԍ�
    PrtInfoTmp.bNum = IsDlgButtonChecked(hWnd, IDC_C_NUM);

    // �x�[�X���C���X�L�b�v
    if (IsDlgButtonChecked(hWnd, IDC_R_NOINTERLINE)) {
        PrtInfoTmp.nBaseLine = IDC_R_NOINTERLINE;
    } else if (IsDlgButtonChecked(hWnd, IDC_R_ENGLISH)) {
        PrtInfoTmp.nBaseLine = IDC_R_ENGLISH;
    } else if (IsDlgButtonChecked(hWnd, IDC_R_JAPAN)) {
        PrtInfoTmp.nBaseLine = IDC_R_JAPAN;
    } else {
        PrtInfoTmp.nBaseLine = IDC_R_AUTO;
    }
}

static BOOL
DialogProcText(
    HWND hWnd,                                  // �_�C�A���O�{�b�N�X�̃n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // ��1���b�Z�[�W �p�����[�^
    LPARAM lParam                               // ��2���b�Z�[�W �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogText);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandText);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseText);
    }
    return FALSE;    
}

//
// �������������ȉ���PostScript�̃R�[���o�b�N�W�ł���������������������
//
static BOOL CALLBACK
DoInitDialogPs(
    HWND hWnd,                                  // �n���h��
    HWND hWndFocus,                             // �t�H�[�J�X�n���h��
    LPARAM lParam                               // �������p�����[�^
    )
{
    DbgPrint(NULL, 'D', "InitDialog(PS)");

    SetDlgItemText(hWnd, IDC_ED_ACRIN, PrtInfoTmp.szAcrobat);
    SetDlgItemText(hWnd, IDC_ED_GHOST, PrtInfoTmp.szGsPath);
    SetDlgItemText(hWnd, IDC_ED_GSOP, PrtInfoTmp.szGsOpt);
    return TRUE;
}

static VOID
DoCommandPs(
    HWND hWnd,                                  // �n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���̃n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    switch (id) {                               // �R���g���[���ԍ�
    case IDC_BT_GSOP:
        SetDlgItemText(hWnd, IDC_ED_GSOP, GS_DEF_OPT);
        break;
    }
}

static void
DoClosePs(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(PS)");

    GetDlgItemText(hWnd, IDC_ED_ACRIN, PrtInfoTmp.szAcrobat, MAX_PATH);
    GetDlgItemText(hWnd, IDC_ED_GHOST, PrtInfoTmp.szGsPath, MAX_PATH);
    GetDlgItemText(hWnd, IDC_ED_GSOP, PrtInfoTmp.szGsOpt, 512);
}

static BOOL
DialogProcPs(
    HWND hWnd,                                  // �_�C�A���O�{�b�N�X�̃n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // ��1���b�Z�[�W �p�����[�^
    LPARAM lParam                               // ��2���b�Z�[�W �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogPs);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandPs);
        HANDLE_MSG(hWnd, WM_DESTROY, DoClosePs);
    }
    return FALSE;    
}

//
// �������������ȉ��͌ʐݒ�̃R�[���o�b�N�W�ł���������������������
//
static BOOL CALLBACK
DoInitDialogKbt(
    HWND hWnd,                                  // �n���h��
    HWND hWndFocus,                             // �t�H�[�J�X�n���h��
    LPARAM lParam                               // �������p�����[�^
    )
{
    DbgPrint(NULL, 'D', "InitDialog(��)");

    // �^�C�g���̐ݒ�
    SetDlgItemText(hWnd, IDC_ED_TITLE, PrtInfoTmp.szTitle);
    return TRUE;
}

static VOID
DoCommandKbt(
    HWND hWnd,                                  // �n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���̃n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    // ���݂͓��ɏ����Ȃ�
    switch (id) {                               // �R���g���[���ԍ�
    default:
        break;
    }
}

static void
DoCloseKbt(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(��)");

    GetDlgItemText(hWnd, IDC_ED_TITLE, PrtInfoTmp.szTitle, 255);
}

static BOOL
DialogProcKbt(
    HWND hWnd,                                  // �_�C�A���O�{�b�N�X�̃n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // ��1���b�Z�[�W �p�����[�^
    LPARAM lParam                               // ��2���b�Z�[�W �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogKbt);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandKbt);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseKbt);
    }
    return FALSE;    
}

// ������������������������������������������������
// ������������ �Z�b�g�A�b�v�_�C�A���O ������������
// ������������������������������������������������
VOID
SetupPrtStyle(HWND hWnd, PPRT_INFO lppiArea)
{
    PROPSHEETPAGE psp[6];
    PROPSHEETHEADER psh;
    int i;
    int iNumOfDlg;                              // �_�C�A���O�̐�
    static BOOL running = FALSE;

    // ���݃Z�b�g�A�b�v�E�C���h�E��\�����̏ꍇ�͉������Ȃ��B�����ɂ�
    // �N���e�B�J���Z�N�V�������K�v�ł����A����ۂǂ̎�����������͑�
    // ��v�ł��傤�B���Ƀ_�C�A���O����\�����ꂽ�Ƃ���ŁA�債���Q
    // �͂Ȃ��̂ŁA�t���O�ł��܂����܂��B
    if (running) {
        return;
    }

    running = TRUE;

    // �_�C�A���O�̐���ݒ�B�f�t�H���g�ݒ��4�A�ʂ̏ꍇ��5
    iNumOfDlg = (&g_PrtInfo == lppiArea) ? 4 : 5;

    for (i = 0; i < iNumOfDlg; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE | PSH_USEHICON;
        psp[i].hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        psp[i].lParam = i;
    }

    i = 0;
    psp[i].hIcon = LoadIcon(NULL, IDI_APPLICATION);
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_SETUP);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcCom;
    psp[i].pszTitle = "���ʐݒ�";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_MAIL));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_MAIL);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcMail;
    psp[i].pszTitle = "e-mail���";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_TEXT));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_TEXT);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcText;
    psp[i].pszTitle = "�e�L�X�g���";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_PS));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_PS);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcPs;
    psp[i].pszTitle = "PostScript";

    // �ȉ��̐ݒ�́A�ʐݒ�̎��̂ݗL���ɂȂ邪�A�ݒ�͏�ɍs���B�f
    // �t�H���g�ݒ�̏ꍇ�͕\������Ȃ��B
    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_KOBETSU));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_KOBETSU);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcKbt;
    psp[i].pszTitle = "�ʐݒ�";

    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_USEHICON | PSH_PROPSHEETPAGE | PSH_PROPTITLE | 
        PSH_NOAPPLYNOW;
    psh.hwndParent = hWnd;
    psh.hInstance = psp[0].hInstance;
    psh.pszCaption = (LPSTR)SETUP_CAPTION;
    psh.hIcon = LoadIcon(NULL, IDI_ASTERISK);
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.nPages = iNumOfDlg;

    // ���݂̓��e�𕡎�
    memcpy(&PrtInfoTmp, lppiArea, sizeof(PRT_INFO));

    i = PropertySheet(&psh);

    running = FALSE;

    if (1 != i) {
        return;
    }

    // �ݒ茋�ʂ𔽉f
    memcpy(lppiArea, &PrtInfoTmp, sizeof(PRT_INFO));

    // �ݒ肵���G���A���f�t�H���g�I�v�V�����G���A�Ɠ����ꍇ
    if (&g_PrtInfo == lppiArea) {
        // �������t�@�C���ւ����f
        SetDefaultPrtInfo();
    }
}
