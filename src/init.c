/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-12-23 16:12:46 tfuruka1>
 * $Id: init.c,v 1.13 2004/12/23 08:11:56 tfuruka1 Exp $
 * $Name:  $
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o�̏���������
 *
 * ���Y�^
 *
 *   ����I�v�V��������ǉ������ꍇ�Ainit.c, setup.c �ɏC�����K�v�Ȃ�
 *   �̓t�@�C�������画�f����̂͗e�Ղł����A���̑��ɂ��ȉ��̃t�@�C��
 *   �ɏC�����K�v�ɂȂ�܂��B
 *
 *     wndproc.c - WM_COPYDATA �̏�����
 *
 * $Log: init.c,v $
 * Revision 1.13  2004/12/23 08:11:56  tfuruka1
 * �V���O�����C�����(�H�~�o����������������Ȃ�)�ɑΉ����܂����B�Ƃ肠��
 * ���A�T�[�o���̐ݒ�݂̂ł��B
 *
 * Revision 1.12  2004/08/21 01:01:01  tfuruka1
 * �e�L�X�g����ɉ����āu�s�ԁv�Ɓu�r���A���v���L���ɂȂ�悤�ɂ��܂����B
 *
 * Revision 1.11  2004/06/18 00:57:56  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.10  2004/01/19 05:34:26  tfuruka1
 * �t�H���g�Ɋ֘A��������v���t�@�C������擾, �i�[�ł���悤�ɂ��܂����B
 *
 * Revision 1.9  2004/01/12 09:56:58  tfuruka1
 * ���ӒԂ��ƒZ�ӒԂ��ɑΉ����܂����B�v���t�@�C���ɒZ�ӒԂ��̏���ێ���
 * ��悤�ɂ��܂����B
 *
 * Revision 1.8  2003/03/14 15:21:28  tfuruka1
 * �� PostScript�֘A�̃p�X����*����*�R�}���h���C������w��o����悤�ɁA
 *    �p�X����MAILBOX����PRT_INFO�ֈړ������B
 *
 * Revision 1.7  2001/12/23 10:24:34  tfuruka1
 * Copyright���󎚂��Ȃ����[�h���T�|�[�g����
 *
 * Revision 1.6  2001/12/18 12:59:22  tfuruka1
 * �f�o�b�O����̃I�v�V������ǉ��B
 *
 * Revision 1.5  2001/12/18 04:10:46  tfuruka1
 * �v���r���[�E�C���h�E�̕ۑ��E���A������ǉ�
 *
 * Revision 1.4  2001/12/14 17:00:24  tfuruka1
 * �v���r���[�Ή�
 *
 * Revision 1.3  2001/08/19 08:55:39  tfuruka1
 * PostScript�t�@�C�����������Ƃ���GhostScript���Ăяo����悤�ɂ����B
 *
 * Revision 1.2  2001/08/19 04:36:41  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.1  2001/02/05 17:41:51  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

#define PROFILE_NAME "ak2pr.ini"
#define PROFILE_SEC  "Print setup"
#define KEY_NUMOFUP  "UP"
#define KEY_TAB      "TabStop"
#define KEY_FONT     "FontSize"
#define KEY_BNORCVH  "bNoRcvHeader"
#define KEY_BCOLOR   "bColor"
#define KEY_BKEISEN  "bKeisen"
#define KEY_BNUM     "bNum"
#define KEY_BASELINE "nBaseLine"
#define KEY_BPREVIEW "bPreview"
#define KEY_BDEBUG   "bDebug"
#define KEY_BSINGLE_LINE "bSingleLine"
#define KEY_BNOCOPYRIGHT "bNoCopyrightPrint"
#define KEY_BSHORT_BINDING "bShortBinding"

#define SEC_FONT     "FONT"
#define KEY_THF      "same-width"
#define KEY_PPF      "Proportional"
#define KEY_OF       "Other.same-width"
#define KEY_OPPF     "Other.Proportional"

#define SEC_DEVICE   "DEVICE SETUP"
#define KEY_DEVNAME  "DeviceName"
#define KEY_DEVMODE  "DeviceMode"

#define SEC_PS       "PostScript"
#define KEY_ACRIN    "AcrobatDistillerWatchdogFolderIN"
#define KEY_GS       "GhostScript.Path"
#define KEY_GSOP     "GhostScript.Option"

#define SEC_PREVIEW  "PreView"
#define KEY_TOP      "top"
#define KEY_BOTTOM   "bottom"
#define KEY_LEFT     "left"
#define KEY_RIGHT    "right"

#define BAD_STR      "(^_^;"
#define GET_PROFILE(sec, key) GetPrivateProfileString((sec), (key),\
                             BAD_STR, szBuf, 1023, szProfile)
#define WRT_PROFILE(sec, key, val) WritePrivateProfileString((sec), (key),\
                                   val, szProfile)
#define IsBadStr(str) (0 == strcmp((str), BAD_STR))

/*--------------------------------------------------------------------
 * GetPrivateProfileString�Ɠ��l�ł��邪�AlphGlobal�փf�[�^���i�[����B
 * �������͕K�v�ȕ������A�����Ŋm�ۂ���B
 * *-------------------------------------------------------------------*/
static BOOL GetPrivateProfileData(
    LPTSTR lpszSec,                             // �Z�N�V������
    LPTSTR lpszKey,                             // �L�[��
    HGLOBAL *lphGlobal,                         // �f�[�^�i�[�n���h��
    LPTSTR lpszProfile                          // �v���t�@�C����
    )
{
    LPBYTE lpByte, lpSave, lpszData;
    DWORD cbData;
    BYTE szWk[16], szKey[256];

    sprintf(szKey, "%s.ByteSize", lpszKey);
    GetPrivateProfileString(SEC_DEVICE, szKey, BAD_STR, szWk, 15, lpszProfile);
    if (IsBadStr(szWk)) {
        return FALSE;
    }
    if (!(cbData = strtoul(szWk, NULL, 10))) {
        return FALSE;
    }

    if (NULL == (lpszData = malloc(cbData * 4))) {
        return FALSE;
    }

    sprintf(szKey, "%s.Data", lpszKey);
    GetPrivateProfileString(SEC_DEVICE, szKey, BAD_STR,
                            lpszData, cbData * 4 - 1, lpszProfile);
    if (IsBadStr(lpszData)) {
        free(lpszData);
        return FALSE;
    }
    lpSave = lpszData;

    if (NULL == (*lphGlobal = GlobalAlloc(GMEM_MOVEABLE, cbData))) {
        free(lpszData);
        return FALSE;
    }
    lpByte = GlobalLock(*lphGlobal);

    for (; *lpszData; lpByte++) {
        if ('^' == *lpszData) {
            *lpByte = *(lpszData + 1) - '@';
            lpszData += 2;
            continue;
        }
        if ('%' == *lpszData) {
            szWk[0] = *(lpszData + 1);
            szWk[1] = *(lpszData + 2);
            szWk[2] = '\0';
            *lpByte = (BYTE)(strtoul(szWk, NULL, 16) % 256);
            lpszData += 3;
            continue;
        }
        *lpByte = *lpszData;
        lpszData++;
    }

    GlobalUnlock(*lphGlobal);
    free(lpSave);
    return TRUE;
}

/*--------------------------------------------------------------------
 * WritePrivateProfileString�Ɠ��l�ł��邪�AlpData����n�܂�f�[�^��
 * cbData�o�C�g�𕶎���֕ϊ����Ă��珑�����ށB
 * *-------------------------------------------------------------------*/
static BOOL WritePrivateProfileData(
    LPTSTR lpszSec,                             // �Z�N�V������
    LPTSTR lpszKey,                             // �L�[��
    LPVOID lpData,                              // �f�[�^�擪�A�h���X
    DWORD cbData,                               // �o�C�g��
    LPTSTR szFileName                           // �v���t�@�C����
    )
{
    int i;
    LPBYTE lpszStore;
    LPBYTE lpByte = lpData;
    TCHAR szBuf[16], szKey[256];

    if (NULL == (lpszStore = malloc(cbData * 4))) {
        return FALSE;
    }

    *lpszStore = '\0';
    for (i = 0; (ULONG)i < cbData; i++) {
        if (0x20 >= *(lpByte + i)) {
            sprintf(lpszStore + strlen(lpszStore), "^%c",
                    '@' + (int)*(lpByte + i));
            continue;
        }
        if (('^' == *(lpByte + i))
            || ('%' == *(lpByte + i))
            || (0x7f <= *(lpByte + i))) {
            sprintf(lpszStore + strlen(lpszStore), "%%%02x", *(lpByte + i));
            continue;
        }
        sprintf(lpszStore + strlen(lpszStore), "%c", *(lpByte + i));
    }

    sprintf(szBuf, "%d", cbData);
    sprintf(szKey, "%s.ByteSize", lpszKey);
    WritePrivateProfileString(lpszSec, szKey, szBuf, szFileName);
    sprintf(szKey, "%s.Data", lpszKey);
    WritePrivateProfileString(lpszSec, szKey, lpszStore, szFileName);

    free(lpszStore);

    return TRUE;
}

/*--------------------------------------------------------------------
 * �f�t�H���g�̈�����𓾂�
 * *-------------------------------------------------------------------*/
void
GetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    GET_PROFILE(PROFILE_SEC, KEY_NUMOFUP);
    g_PrtInfo.nNumOfUp = IsBadStr(szBuf) ? 2 : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_TAB);
    g_PrtInfo.nTab = IsBadStr(szBuf) ? 8 : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_FONT);
    g_PrtInfo.fFontSize = IsBadStr(szBuf) ? 10.0 : atof(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BPREVIEW);
    g_PrtInfo.bPreView = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BNOCOPYRIGHT);
    g_PrtInfo.bNoCopyright = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BDEBUG);
    g_PrtInfo.bDebug = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BNORCVH);
    g_PrtInfo.bNoRcvHeader = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BCOLOR);
    g_PrtInfo.bColor = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BKEISEN);
    g_PrtInfo.bKeisen = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BASELINE);
    g_PrtInfo.nBaseLine = IsBadStr(szBuf) ? IDC_R_ENGLISH : atoi(szBuf);
    switch (g_PrtInfo.nBaseLine) {
    case IDC_R_NOINTERLINE:
    case IDC_R_ENGLISH:
    case IDC_R_JAPAN:
    case IDC_R_AUTO:
        break;
    default:
        g_PrtInfo.nBaseLine = IDC_R_ENGLISH;
    }

    GET_PROFILE(PROFILE_SEC, KEY_BNUM);
    g_PrtInfo.bNum = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BSHORT_BINDING);
    g_PrtInfo.bShortBinding = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(PROFILE_SEC, KEY_BSINGLE_LINE);
    g_PrtInfo.nSingleLine = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    // �t�H���g���𓾂�
    GET_PROFILE(SEC_FONT, KEY_THF);
    strncpy(g_PrtInfo.lfTHF.lfFaceName, IsBadStr(szBuf) ? FN_MSM : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_PPF);
    strncpy(g_PrtInfo.lfPPF.lfFaceName, IsBadStr(szBuf) ? FN_MSPG : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_OF);
    strncpy(g_PrtInfo.lfOF.lfFaceName, IsBadStr(szBuf) ? FN_COU : szBuf,
            LF_FACESIZE);

    GET_PROFILE(SEC_FONT, KEY_OPPF);
    strncpy(g_PrtInfo.lfOPPF.lfFaceName, IsBadStr(szBuf) ? FN_ARIAL : szBuf,
            LF_FACESIZE);

    // PostScript�֘A���𓾂�
    GET_PROFILE(SEC_PS, KEY_ACRIN);
    strncpy(g_PrtInfo.szAcrobat, szBuf, MAX_PATH);

    GET_PROFILE(SEC_PS, KEY_GS);
    strncpy(g_PrtInfo.szGsPath, szBuf, MAX_PATH);

    GET_PROFILE(SEC_PS, KEY_GSOP);
    strncpy(g_PrtInfo.szGsOpt, szBuf, 512);

    // �v�����^�̃f�o�C�X���𓾂�
    if (!GetPrivateProfileData(SEC_DEVICE, KEY_DEVNAME,
                               &g_MailBox.hDevNames, szProfile)) {
        g_MailBox.hDevNames = NULL;
        g_MailBox.hDevMode = NULL;
        return;
    }
    if (!GetPrivateProfileData(SEC_DEVICE, KEY_DEVMODE,
                               &g_MailBox.hDevMode, szProfile)) {
        GlobalFree(g_MailBox.hDevNames);
        g_MailBox.hDevNames = NULL;
        g_MailBox.hDevMode = NULL;
    }
}

/*--------------------------------------------------------------------
 * ����ݒ���������߂�
 * *-------------------------------------------------------------------*/
void
SetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];
    LPVOID lpData;
    DWORD cbData;

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    sprintf(szBuf, "%d", g_PrtInfo.nNumOfUp);
    WRT_PROFILE(PROFILE_SEC, KEY_NUMOFUP, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nTab);
    WRT_PROFILE(PROFILE_SEC, KEY_TAB, szBuf);

    sprintf(szBuf, "%f", g_PrtInfo.fFontSize);
    WRT_PROFILE(PROFILE_SEC, KEY_FONT, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bPreView);
    WRT_PROFILE(PROFILE_SEC, KEY_BPREVIEW, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNoCopyright);
    WRT_PROFILE(PROFILE_SEC, KEY_BNOCOPYRIGHT, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bDebug);
    WRT_PROFILE(PROFILE_SEC, KEY_BDEBUG, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNoRcvHeader);
    WRT_PROFILE(PROFILE_SEC, KEY_BNORCVH, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bColor);
    WRT_PROFILE(PROFILE_SEC, KEY_BCOLOR, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bKeisen);
    WRT_PROFILE(PROFILE_SEC, KEY_BKEISEN, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nBaseLine);
    WRT_PROFILE(PROFILE_SEC, KEY_BASELINE, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNum);
    WRT_PROFILE(PROFILE_SEC, KEY_BNUM, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bShortBinding);
    WRT_PROFILE(PROFILE_SEC, KEY_BSHORT_BINDING, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nSingleLine);
    WRT_PROFILE(PROFILE_SEC, KEY_BSINGLE_LINE, szBuf);

    // �t�H���g���̏�������
    WRT_PROFILE(SEC_FONT, KEY_THF, g_PrtInfo.lfTHF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_PPF, g_PrtInfo.lfPPF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_OF, g_PrtInfo.lfOF.lfFaceName);
    WRT_PROFILE(SEC_FONT, KEY_OPPF, g_PrtInfo.lfOPPF.lfFaceName);

    // PostScript���֘A�̏�������
    WRT_PROFILE(SEC_PS, KEY_ACRIN, g_PrtInfo.szAcrobat);
    WRT_PROFILE(SEC_PS, KEY_GS, g_PrtInfo.szGsPath);
    WRT_PROFILE(SEC_PS, KEY_GSOP, g_PrtInfo.szGsOpt);

    // �f�o�C�X���̏�������
    if (g_MailBox.hDevNames && g_MailBox.hDevMode) {

        lpData = GlobalLock(g_MailBox.hDevNames);
        cbData = GlobalSize(g_MailBox.hDevNames);
        WritePrivateProfileData(SEC_DEVICE, KEY_DEVNAME,
                                lpData, cbData, szProfile);
        GlobalUnlock(g_MailBox.hDevNames);

        lpData = GlobalLock(g_MailBox.hDevMode);
        cbData = GlobalSize(g_MailBox.hDevMode);
        WritePrivateProfileData(SEC_DEVICE, KEY_DEVMODE,
                                lpData, cbData, szProfile);
        GlobalUnlock(g_MailBox.hDevNames);
    }
}

/*--------------------------------------------------------------------
 * �v���r���[�E�C���h�E�̃E�C���h�E�ʒu���v���t�@�C���֏�������
 * *-------------------------------------------------------------------*/
void
SetPreViewPos(LPRECT lprc)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    sprintf(szBuf, "%d", lprc->top);
    WRT_PROFILE(SEC_PREVIEW, KEY_TOP, szBuf);

    sprintf(szBuf, "%d", lprc->bottom);
    WRT_PROFILE(SEC_PREVIEW, KEY_BOTTOM, szBuf);

    sprintf(szBuf, "%d", lprc->left);
    WRT_PROFILE(SEC_PREVIEW, KEY_LEFT, szBuf);

    sprintf(szBuf, "%d", lprc->right);
    WRT_PROFILE(SEC_PREVIEW, KEY_RIGHT, szBuf);
}

/*--------------------------------------------------------------------
 * �v���r���[�E�C���h�E�̃E�C���h�E�ʒu���v���t�@�C������ǂݍ���
 * *-------------------------------------------------------------------*/
void
GetPreViewPos(LPRECT lprc)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];
    int wd, ht;

    wd = GetSystemMetrics(SM_CXSCREEN);
    ht = GetSystemMetrics(SM_CYSCREEN);

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    GET_PROFILE(SEC_PREVIEW, KEY_TOP);
    lprc->top = IsBadStr(szBuf) ? (ht / 2 - 300) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_BOTTOM);
    lprc->bottom = IsBadStr(szBuf) ? (ht / 2 + 300) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_LEFT);
    lprc->left = IsBadStr(szBuf) ? (wd / 2 - 400) : atoi(szBuf);

    GET_PROFILE(SEC_PREVIEW, KEY_RIGHT);
    lprc->right = IsBadStr(szBuf) ? (wd / 2 + 400) : atoi(szBuf);

    if (lprc->top > ht) {
        lprc->top = 0;
    }
    if (lprc->bottom < 0) {
        lprc->bottom = ht;
    }
    if (lprc->left > wd) {
        lprc->left = 0;
    }
    if (lprc->right < 0) {
        lprc->right = wd;
    }
}

/*--------------------------------------------------------------------
 * �N���X�o�^�y�уE�C���h�E�̐������s��
 * *-------------------------------------------------------------------*/
HWND
CreateApplication(HANDLE hInstance)
{
    HWND hWnd;                                  // �E�B���h�E�n���h��
    WNDCLASS wc;                                // �E�C���h�E�N���X

    // �󎚂̏�������ǂݍ���
    GetDefaultPrtInfo();
    SetDefaultPrtInfo();                        // �t�@�C�������݂��Ȃ��ꍇ

    // �v���r���[�E�C���h�E�̈ʒu�𕜌�����
    GetPreViewPos(&g_MailBox.PrevInfo.rc);

    // �f�o�C�X��񂪗L�����ۂ����ׂ�
    if (g_MailBox.hDevNames) {
        HDC hDC;
        LPDEVNAMES lpDevNames;
        LPDEVMODE lpDevMode;

        lpDevNames = (LPDEVNAMES)GlobalLock(g_MailBox.hDevNames);
        lpDevMode = (LPDEVMODE)GlobalLock(g_MailBox.hDevMode);

        hDC = CreateDC((PCHAR)lpDevNames + lpDevNames->wDriverOffset,
                       (PCHAR)lpDevNames + lpDevNames->wDeviceOffset,
                       NULL, lpDevMode);
        GlobalUnlock(g_MailBox.hDevNames);
        GlobalUnlock(g_MailBox.hDevMode);

        if (hDC) {
            DeleteDC(hDC);
        }
        else {
            GlobalFree(g_MailBox.hDevNames);
            GlobalFree(g_MailBox.hDevMode);

            g_MailBox.hDevNames = NULL;
            g_MailBox.hDevMode = NULL;
        }
    }

    // �ڍ׃E�C���h�E
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)MainWndProcDetail;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FURUKAWA));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;                    // �w�i�͕`�悵�Ȃ�
    wc.lpszMenuName = NULL;                     // ���j���[�͖���
    wc.lpszClassName = SV_DBGCLASS;             // �N���X��

    // ���W�X�g����
    if (!RegisterClass(&wc)) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("RegisterClass", nErr),
                   "CreateApplication()", MB_ERROR);
        return NULL;
    }

    // �ڍ׃E�C���h�E���쐬����
    hWnd = CreateWindowEx(0, SV_DBGCLASS, "�ڍ�(ak2pr)",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL);

    // �E�C���h�E�̐����Ɏ��s
    if (!hWnd) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateWindowEx", nErr),
                   "CreateApplication()", MB_ERROR);
        return FALSE;
    }

    // �ڍ׃E�C���h�E��\������
    ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    // ���C���E�C���h�E
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = (WNDPROC)MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FURUKAWA));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;                    // �w�i�͕`�悵�Ȃ�
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = SV_CLASS;                // �N���X��

    // ���W�X�g����
    if (!RegisterClass(&wc)) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("RegisterClass", nErr),
                   "CreateApplication()", MB_ERROR);
        return NULL;
    }

    // ���C���E�C���h�E���쐬����(���̓��C���Ƃ����ڍ׃E�C���h�E��
    // �q���ł���)
    hWnd = CreateWindowEx(0, SV_CLASS, SV_CAPTION,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          hWnd, NULL, hInstance, NULL);

    // �E�C���h�E�̐����Ɏ��s
    if (!hWnd) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateWindowEx", nErr),
                   "CreateApplication()", MB_ERROR);
        return FALSE;
    }

    // �V�F���g���C�����݂���ꍇ�̓E�C���h�E���\���ɂ���B�V�F���g
    // ���C�����݂��Ȃ��ꍇ�́A�E�C���h�E��\������
    if (!IsShellTray()) {
        DbgPrint(NULL, 'W', "����, �u�^�X�N�o�[�v�����݂��܂���B"
                 "\n�f�X�N�g�b�v��̃E�C���h�E�Ƃ��ăA�C�R����Ԃ�"
                 "�N�����܂��B");
        CloseWindow(hWnd);
    }
    else {
        DbgPrint(NULL, 'I', "�u�^�X�N�o�[�v��������܂����B"
                 "\n�^�X�N�o�[�̎��v�ׂ̗ɃA�C�R���Ƃ���"
                 "�\�����܂��B");
    }
    ShowWindow(hWnd, IsShellTray() ? SW_HIDE : SW_SHOW);
    UpdateWindow(hWnd);

    return hWnd;
}
