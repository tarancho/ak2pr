/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:41:47 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o�̏���������
 *
 * $Id: init.c,v 1.1 2001/02/05 17:41:51 tfuruka1 Exp $
 * $Log: init.c,v $
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

#define SEC_DEVICE   "DEVICE SETUP"
#define KEY_DEVNAME  "DeviceName"
#define KEY_DEVMODE  "DeviceMode"

#define BAD_STR      "(^_^;"
#define GET_PROFILE(key) GetPrivateProfileString(PROFILE_SEC, (key),\
                                                 BAD_STR, szBuf, 1023,\
                                                 szProfile)
#define WRT_PROFILE(key, val) WritePrivateProfileString(PROFILE_SEC, (key),\
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
 *
 * *-------------------------------------------------------------------*/
void
GetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    GET_PROFILE(KEY_NUMOFUP);
    g_PrtInfo.nNumOfUp = IsBadStr(szBuf) ? 2 : atoi(szBuf);

    GET_PROFILE(KEY_TAB);
    g_PrtInfo.nTab = IsBadStr(szBuf) ? 8 : atoi(szBuf);

    GET_PROFILE(KEY_FONT);
    g_PrtInfo.fFontSize = IsBadStr(szBuf) ? 10.0 : atof(szBuf);

    GET_PROFILE(KEY_BNORCVH);
    g_PrtInfo.bNoRcvHeader = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(KEY_BCOLOR);
    g_PrtInfo.bColor = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(KEY_BKEISEN);
    g_PrtInfo.bKeisen = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

    GET_PROFILE(KEY_BASELINE);
    g_PrtInfo.nBaseLine = IsBadStr(szBuf) ? 1 : atoi(szBuf);

    GET_PROFILE(KEY_BNUM);
    g_PrtInfo.bNum = IsBadStr(szBuf) ? FALSE : atoi(szBuf);

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
 *
 * *-------------------------------------------------------------------*/
void
SetDefaultPrtInfo(void)
{
    TCHAR szProfile[MAX_PATH];
    TCHAR szBuf[1024];

    wsprintf(szProfile, "%s/%s", GetMyDir(), PROFILE_NAME);

    sprintf(szBuf, "%d", g_PrtInfo.nNumOfUp);
    WRT_PROFILE(KEY_NUMOFUP, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nTab);
    WRT_PROFILE(KEY_TAB, szBuf);

    sprintf(szBuf, "%f", g_PrtInfo.fFontSize);
    WRT_PROFILE(KEY_FONT, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNoRcvHeader);
    WRT_PROFILE(KEY_BNORCVH, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bColor);
    WRT_PROFILE(KEY_BCOLOR, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bKeisen);
    WRT_PROFILE(KEY_BKEISEN, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.nBaseLine);
    WRT_PROFILE(KEY_BASELINE, szBuf);

    sprintf(szBuf, "%d", g_PrtInfo.bNum);
    WRT_PROFILE(KEY_BNUM, szBuf);

    // �f�o�C�X���̏�������
    if (g_MailBox.hDevNames && g_MailBox.hDevMode) {
        LPVOID lpData;
        DWORD cbData;

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
