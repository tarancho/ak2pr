/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-08-19 12:47:54 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈󎚐ݒ�
 *
 * $Id: setup.c,v 1.3 2001/08/19 04:33:53 tfuruka1 Exp $
 * $Log: setup.c,v $
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

    // �^�u���̃Z�b�g
    sprintf(szBuf, "%d", g_PrtInfo.nTab);
    SetDlgItemText(hWnd, IDC_TABSTOP, szBuf);
    // �t�H���g�T�C�Y�̃Z�b�g
    sprintf(szBuf, "%f", g_PrtInfo.fFontSize);
    SetDlgItemText(hWnd, IDC_FONTSIZE, szBuf);
    // �i�g���̃Z�b�g
    sprintf(szBuf, "%d", g_PrtInfo.nNumOfUp);
    SetDlgItemText(hWnd, IDC_NUP, szBuf);

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

    DbgPrint(NULL, 'I', "���ʐݒ�I��\nTABSTOP:%d\nFONTSIZE:%f\nNUP:%d",
             PrtInfoTmp.nTab, PrtInfoTmp.fFontSize, PrtInfoTmp.nNumOfUp);
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
    // �J���[����̃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_COLOR, g_PrtInfo.bColor ? TRUE : FALSE);
    // Received�w�b�_�̈󎚃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_NORHEAD,
                   g_PrtInfo.bNoRcvHeader ? TRUE : FALSE);

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
    UINT id;

    // �r���A���̃`�F�b�N
    CheckDlgButton(hWnd, IDC_C_KEISEN, g_PrtInfo.bKeisen ? TRUE : FALSE);
    CheckDlgButton(hWnd, IDC_C_NUM, g_PrtInfo.bNum ? TRUE : FALSE);

    switch (g_PrtInfo.nBaseLine) {
    case 0: id = IDC_R_NOINTERLINE; break;
    case 1: id = IDC_R_ENGLISH; break;
    case 2: id = IDC_R_JAPAN; break;
    default: id = IDC_R_AUTO; break;
    }
    CheckDlgButton(hWnd, id, TRUE);

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
        PrtInfoTmp.nBaseLine = 0;
        break;
    case IDC_R_ENGLISH:
        PrtInfoTmp.nBaseLine = 1;
        break;
    case IDC_R_JAPAN:
        PrtInfoTmp.nBaseLine = 2;
        break;
    case IDC_R_AUTO:
        PrtInfoTmp.nBaseLine = 9;
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.bNum = TRUE;
        PrtInfoTmp.nBaseLine = 1;
        PrtInfoTmp.bKeisen = FALSE;

        CheckDlgButton(hWnd, IDC_C_KEISEN, FALSE);
        CheckDlgButton(hWnd, IDC_C_NUM, TRUE);
        CheckDlgButton(hWnd, IDC_R_ENGLISH, TRUE);

        break;
    }
}

static void
DoCloseText(HWND hWnd)
{
    // �r���A��
    PrtInfoTmp.bKeisen = IsDlgButtonChecked(hWnd, IDC_C_KEISEN);
    // �s�ԍ�
    PrtInfoTmp.bNum = IsDlgButtonChecked(hWnd, IDC_C_NUM);
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
    SetDlgItemText(hWnd, IDC_ED_ACRIN, g_MailBox.szAcrobat);
    SetDlgItemText(hWnd, IDC_ED_GHOST, g_MailBox.szGsPath);
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
    TCHAR szBuf[1024];

    switch (id) {                               // �R���g���[���ԍ�
    case IDC_BT_ACRIN:
        GetDlgItemText(hWnd, IDC_ED_ACRIN, szBuf, MAX_PATH);
        if (GetOpenFileNameWrap(hWnd,
                                "�S�Ẵt�@�C��\0*.*\0\0",
                                "Acrobat Distiller�̊Ď��t�H���_[In]",
                                szBuf)) {
            SetDlgItemText(hWnd, IDC_ED_ACRIN, szBuf);
        }
        break;
    case IDC_BT_GHOST:
        GetDlgItemText(hWnd, IDC_ED_GHOST, szBuf, MAX_PATH);
        if (GetOpenFileNameWrap(hWnd,
                                "���s�t�@�C��\0*.exe\0\0",
                                "GhostScript",
                                szBuf)) {
            SetDlgItemText(hWnd, IDC_ED_GHOST, szBuf);
        }
        break;
    }
}

static void
DoClosePs(HWND hWnd)
{
    GetDlgItemText(hWnd, IDC_ED_ACRIN, g_MailBox.szAcrobat, MAX_PATH);
    GetDlgItemText(hWnd, IDC_ED_GHOST, g_MailBox.szGsPath, MAX_PATH);
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

// ������������������������������������������������
// ������������ �Z�b�g�A�b�v�_�C�A���O ������������
// ������������������������������������������������
VOID
SetupPrtStyle(HWND hWnd)
{
    PROPSHEETPAGE psp[5];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 4; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE | PSH_USEHICON;
        psp[i].hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        psp[i].hIcon = LoadIcon(NULL, IDI_APPLICATION);
        psp[i].lParam = i;
    }

    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_SETUP);
    psp[0].pfnDlgProc = (DLGPROC)DialogProcCom;
    psp[0].pszTitle = "���ʐݒ�";

    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_MAIL);
    psp[1].pfnDlgProc = (DLGPROC)DialogProcMail;
    psp[1].pszTitle = "e-mail���";

    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_TEXT);
    psp[2].pfnDlgProc = (DLGPROC)DialogProcText;
    psp[2].pszTitle = "�e�L�X�g���";

    psp[3].pszTemplate = MAKEINTRESOURCE(IDD_PS);
    psp[3].pfnDlgProc = (DLGPROC)DialogProcPs;
    psp[3].pszTitle = "PostScript";

    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_USEHICON | PSH_PROPSHEETPAGE | PSH_PROPTITLE | 
        PSH_NOAPPLYNOW;
    psh.hwndParent = hWnd;
    psh.hInstance = psp[0].hInstance;
    psh.pszCaption = (LPSTR)"����X�^�C���̐ݒ�";
    psh.hIcon = LoadIcon(NULL, IDI_ASTERISK);
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.nPages = 4;
    i = PropertySheet(&psh);

    if (1 != i) {
        return;
    }

    // �ݒ茋�ʂ𔽉f
    memcpy(&g_PrtInfo, &PrtInfoTmp, sizeof(PRT_INFO));
    // �������t�@�C���ւ����f
    SetDefaultPrtInfo();
}
