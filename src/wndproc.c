/* -*- mode: c++; coding: sjis-dos; -*-
 *
 * �uak2ps�̂悤�Ȃ��́v�̃E�C���h�E�v���V�W��
 *
 * $Id: wndproc.c,v 1.1 2001/02/05 16:52:53 tfuruka1 Exp $
 * $Log: wndproc.c,v $
 * Revision 1.1  2001/02/05 16:52:53  tfuruka1
 * Initial revision
 *
 * Revision 1.2  2000/11/05 03:48:57  tfuruka1
 * �ڍ׃E�B���h�E��\���������Ɂu�����~�v���j���[�Ƀ`�F�b�N�}�[�N��t��
 * �Ă����o�O���C���B�܂��A�u�ڍ׃E�C���h�E�v���j���[�̃`�F�b�N�}�[�N���t
 * �i���j���[���\������Ă��Ȃ��Ƃ��Ƀ`�F�b�N�}�[�N���t���Ă����j�������o
 * �O���C���B
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#define TIME_STAMP "Time-stamp: <2001-02-06 01:52:52 tfuruka1>"

#include "ak2prs.h"

#define SZ_URL "http://www.vector.co.jp/authors/tfuruka1/"
#define SZ_MAIL "mailto:tfuruka1@nifty.com"
#define TMR_ID 100
#define TMR_TO 1000

#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
     (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

enum{IDC_TASK = 100, IDC_LIST};

static NOTIFYICONDATA s_ndi;                    // �^�X�N�g���C
static HWND hWndOwn;                            // �I�[�i�E�C���h�E�n���h��
static HWND hWndList = NULL;                    // ���X�g�{�b�N�X
static UINT idTmr = 0;                          // �^�C�}���ʎq

/*--------------------------------------------------------------------
 * WM_CREATE �̏������B 
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoCreate(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    LPCREATESTRUCT pcs                          // �쐬�f�[�^�i�[����\����
    )
{
    HMENU hMenu;

    hWndOwn = pcs->hwndParent;                  // �I�[�i�E�C���h�E�n���h��
    DbgPrint(pcs->hwndParent, 'I', "AK2PR Server(%s) Start.", TIME_STAMP);

    // �^�X�N�g���C�ɃA�C�R����\������
    s_ndi.cbSize = sizeof(NOTIFYICONDATA);
    s_ndi.hWnd = hWnd; 
    s_ndi.uID = IDC_TASK; 
    s_ndi.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
    s_ndi.uCallbackMessage = WM_TASKMENU; 
    s_ndi.hIcon = LoadIcon(GetWindowInstance(hWnd),
                           MAKEINTRESOURCE(IDI_TIP));
    wsprintf(s_ndi.szTip, TEXT("%s"), SV_CAPTION);
    Shell_NotifyIcon(NIM_ADD, &s_ndi);

    // ���X�g�{�b�N�X�𐶁X����(���͂��ꂪ���Queue�̎��Ԃł���)
    if (NULL == (hWndList = CreateWindowEx(
        0, "LISTBOX", "(^_^)",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        LBS_DISABLENOSCROLL | LBS_HASSTRINGS | LBS_USETABSTOPS	|
        LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_EXTENDEDSEL,
        0, 0, 10, 10,
        hWnd, (HMENU)IDC_LIST, GetWindowInstance(hWnd), NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() ListBox", MB_ERROR);
        return FALSE;
    }

    // ���[���{�b�N�X�̏�����
    g_MailBox.bRun = TRUE;                      // �X���b�h���s��
    g_MailBox.bStop = TRUE;                     // �����~
    g_MailBox.hWnd = hWnd;

    // ����X���b�h�̐���
    g_MailBox.hThread = CreateThread(NULL, 0,
                                    (LPTHREAD_START_ROUTINE)PrintThread,
                                    NULL, 0, &g_MailBox.idThread);
    if (NULL == g_MailBox.hThread) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateThread", nErr),
                   __FILE__ "/DoCreate()", MB_ERROR);
        return FALSE;
    }

    // �C���^�[�o���^�C�}�ݒ�
    idTmr = SetTimer(hWnd, TMR_ID, TMR_TO, NULL);

    // ���j���[�̃`�F�b�N�{�^���̍X�V
    hMenu = GetMenu(hWnd);
    hMenu = GetSubMenu(hMenu, 0);
    CheckMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND |
                  g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);

    return TRUE;
}
/*--------------------------------------------------------------------
 * WM_SIZE
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoSize(HWND hWnd,                               // �n���h��
       UINT state,                              // �T�C�Y�ύX�t���O
       int cx,                                  // ��
       int cy                                   // ����
    )
{
    if (SIZE_MINIMIZED == state) {              // �A�C�R�������ꂽ��
        if (IsShellTray()) {                    // �V�F���g���C���L��ꍇ��
            ShowWindow(hWnd, SW_HIDE);          // �E�C���h�E����������
        }
        return;
    }
    if (hWndList) {
        SetWindowPos(hWndList, NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    }
}
/*--------------------------------------------------------------------
 * WM_REALCLOSE 
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoRealClose(HWND hWnd)
{
    if (IDYES == MessageBox(hWnd, "�I�����Ă��X�����ł���", SV_CAPTION,
                            MB_SETFOREGROUND | MB_ICONQUESTION | MB_YESNO)) {
        SendMessage(hWndOwn, WM_REALCLOSE, 0, 0);
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------
 * WM_CLOSE
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoClose(HWND hWnd)
{
    //ShowWindow(hWnd, SW_HIDE);
    DoRealClose(hWnd);
}

/*--------------------------------------------------------------------
 * WM_DESTROY ���b�Z�[�W�̏�����
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoDestroy(HWND hWnd)
{
    Shell_NotifyIcon(NIM_DELETE, &s_ndi);       // �^�X�N�g���C����폜
}
/*--------------------------------------------------------------------
 * WM_COPYDATA ���b�Z�[�W�̏�����
 * *-------------------------------------------------------------------*/
static BOOL CALLBACK
DoCopyData(
    HWND hWnd,                                  // �n���h��
    HWND hWndFrom,                              // ����葤�� HWND
    PCOPYDATASTRUCT pcds                        // �f�[�^���i�[����\����
    )
{
    PPRT_INFO pPrtInfo;

    DbgDump(hWnd, 'C', "����v����t", pcds->lpData, pcds->cbData);
    UpdateWindow(hWnd);

    if (NULL == (pPrtInfo = (PPRT_INFO)malloc(sizeof(PRT_INFO)))) {
        DbgPrint(hWnd, 'E', "�������s���ׁ̈A�X�v�[���ł��܂���");
        return FALSE;
    }

    memcpy(pPrtInfo, pcds->lpData, sizeof(PRT_INFO));

    // ���ݒ�I�v�V�����̓f�t�H���g�Œu��������
    if (!pPrtInfo->nNumOfUp) {
        pPrtInfo->nNumOfUp = g_PrtInfo.nNumOfUp;
    }
    if (!pPrtInfo->nTab) {
        pPrtInfo->nTab = g_PrtInfo.nTab;
    }
    if (0.0 == pPrtInfo->fFontSize) {
        pPrtInfo->fFontSize = g_PrtInfo.fFontSize;
    }

    // �p�����[�^�Őݒ�o���Ȃ��l��ݒ肷��
    pPrtInfo->bColor = g_PrtInfo.bColor;
    pPrtInfo->bNoRcvHeader = g_PrtInfo.bNoRcvHeader;

    EnQueue(hWndList, pPrtInfo);                // EnQueue

    return TRUE;
}

/*--------------------------------------------------------------------
 * WM_COMMAND ���b�Z�[�W�̏�����
 * *-------------------------------------------------------------------*/
static VOID
DoCommand(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int id,                                     // �R���g���[��ID
    HWND hWndCtl,                               // �R���g���[���n���h��
    UINT codeNotify                             // �ʒm�R�[�h
    )
{
    int nErr, cnt, i;
    TCHAR szMsg[1024];
    COPYDATASTRUCT cds;                         // ���M�f�[�^
    PRT_INFO PrtInfo;
    LPVOID pv;
    HMENU hMenu;

    switch (id) {
    case IDM_HTTP: case IDM_MAIL:
        if (32 >= (nErr = (int)ShellExecute(hWnd, NULL,
                                       (IDM_HTTP == id) ? SZ_URL : SZ_MAIL,
                                       NULL, NULL, SW_SHOW))) {
            sprintf(szMsg, "%d�̃G���[���������܂���", nErr);
            MessageBox(hWnd, szMsg, "DoCommand() " __FILE__, MB_ERROR);
        }
        break;
    case IDM_EXIT:
        SendMessage(hWnd, WM_REALCLOSE, 0, 0);
        break;
    case IDM_SHOW:
        ShowWindow(hWndOwn, IsWindowVisible(hWndOwn) ? SW_HIDE : SW_SHOW);
        // ���j���[�̃`�F�b�N�{�^���̍X�V
        hMenu = GetMenu(hWnd);
        hMenu = GetSubMenu(hMenu, 0);
        CheckMenuItem(hMenu, IDM_SHOW, MF_BYCOMMAND |
                      IsWindowVisible(hWndOwn) ? MF_CHECKED : MF_UNCHECKED);
        break;
    case IDM_SETUP:
        SetupPrtStyle(hWnd);
        break;
    case IDM_PRINTER:
        SetupPrinter(hWnd, &g_MailBox.hDevNames, &g_MailBox.hDevMode);
        break;
    case IDM_TESTPRT:                           // �e�X�g��
        PrtInfo.szFileName[0] = '\0';
        strcpy(PrtInfo.szTitle, "�e�X�g��");
        PrtInfo.nNumOfUp = 0;
        PrtInfo.nTab = 0;
        PrtInfo.fFontSize = 0;
        // DLL��Varsion�s��v��h����, �^�C���X�^���v�𖄂߂�
        strcpy(PrtInfo.szTimeStamp, TIMESTAMP);
        strcpy(PrtInfo.szTimeStamp1, TIMESTAMP);

        cds.dwData = 0;
        cds.cbData = sizeof(PRT_INFO);
        cds.lpData = (LPVOID)&PrtInfo;
        SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        break;
    case IDM_DELETE:
        cnt = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
        if (0 > cnt) {
            break;
        }
        for (i = 0, cnt--; cnt >= 0; cnt--) {
            if (SendMessage(hWndList, LB_GETSEL, cnt, 0)) {
                pv = (LPVOID)SendMessage(hWndList, LB_GETITEMDATA, cnt, 0);
                SendMessage(hWndList, LB_DELETESTRING, cnt, 0);
                if (LB_ERR == (int)pv || !pv) {
                    MessageBox(hWnd, "����f�[�^�̍폜�Ɏ��s���܂���",
                               "LB_GETITEMDATA",
                               MB_SETFOREGROUND | MB_ICONSTOP);
                    continue;
                }
                free(pv);
                i++;
            }
        }
        if (i) {
            TCHAR szBuf[256];
            sprintf(szBuf, "%d�̈���f�[�^���폜���܂���", i);
            MessageBox(hWnd, szBuf, "�����������",
                       MB_SETFOREGROUND | MB_ICONINFORMATION);
        }
        break;
    case IDM_STOP:                              // �����~�v��
        g_MailBox.bStop = g_MailBox.bStop ? FALSE : TRUE;
        DbgPrint(NULL, 'I', "���Queue�̃X�P�W���[����%s���܂���",
                 g_MailBox.bStop ? "��~" : "�ĊJ");
        // ���j���[�̃`�F�b�N�{�^���̍X�V
        hMenu = GetMenu(hWnd);
        hMenu = GetSubMenu(hMenu, 0);
        CheckMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND |
                      g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);
        break;
    }
}

/*--------------------------------------------------------------------
 * WM_TASKMENU ���b�Z�[�W������
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
DoTaskMenu(
    HWND hWnd,                                  // �n���h��
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    )
{
    HMENU hMenu;                                // ���j���[�n���h��
    HMENU hMenuTrackPopup;                      // ���j���[�n���h��
    POINT pt;                                   // ���W

    switch (lParam) {
    case WM_RBUTTONDOWN:                        // �E�{�^������
        // �悸�͂���
        SendMessage(hWnd, WM_NULL, 0, 0);       // ���S�Ȃ���
        SetForegroundWindow(hWnd);              // �E�C���h�E��O�ʂ�

        GetCursorPos(&pt);                      // ���݂̃}�E�X���W�𓾂�

        // ���\�[�X���烁�j���[�𓾂�
        if (!(hMenu = LoadMenu(GetWindowInstance(hWnd),
                               MAKEINTRESOURCE(IDR_MENU)))) {
            return 0;                           // ���s�����Ƃ��͖���
        }
        // �T�u���j���[�𓾂�
        hMenuTrackPopup = GetSubMenu(hMenu, 0);

        // �ڍ׃E�C���h�E�̕\����Ԃɂ���ă`�F�b�N��ݒ肷��
        CheckMenuItem(hMenuTrackPopup, IDM_SHOW, MF_BYCOMMAND | 
                      IsWindowVisible(hWndOwn) ? MF_CHECKED : MF_UNCHECKED);

        // �����~��Ԃɂ���ă`�F�b�N��ݒ肷��
        CheckMenuItem(hMenuTrackPopup, IDM_STOP, MF_BYCOMMAND |
                      g_MailBox.bStop ? MF_CHECKED : MF_UNCHECKED);

        // ���j���[��\������(���j���[���I�������܂ł́A���̊֐�����
        // �߂��ė��Ȃ�)
        TrackPopupMenu(hMenuTrackPopup, 0, pt.x, pt.y, 0, hWnd, NULL);
        // ���j���[���폜����
        DestroyMenu(hMenu);
        break;
    case WM_LBUTTONDBLCLK:                      // ���{�^���̃_�u���N���b�N
        ShowWindow(hWnd, SW_SHOW);              // ���C���E�C���h�E��\��
        SetForegroundWindow(hWnd);              // �E�C���h�E��O�ʂ�
        if (IsIconic(hWnd)) {                   // �A�C�R����Ԃ̏ꍇ��
            OpenIcon(hWnd);                     // �A�C�R�����J��
        }
        break;
    }
    return 0;
}

static VOID CALLBACK
DoTimer(
    HWND hWnd,
    UINT id
    )
{
    if (g_MailBox.bRun) {                       // ����X���b�h�����쒆�̏ꍇ
        return;                                 // �͉������Ȃ�
    }

    if (g_MailBox.bStop) {                      // �����~����
        return;                                 // �������Ȃ�
    }

    // Queue����̏ꍇ�͉������Ȃ�
    if (!DeQueue(hWndList, &g_MailBox.PrtInfo)) {
        return;
    }
    
    // ����X���b�h���N������
    g_MailBox.bRun = TRUE;
    ResumeThread(g_MailBox.hThread);
}

/*--------------------------------------------------------------------
 * �T�X�y���h�v��
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
DoSuspend(
    HWND hWnd,
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    )
{
    DbgPrint(NULL, 'I', "���Thread���~���܂���");
    g_MailBox.bRun = FALSE;
    SuspendThread((HANDLE)wParam);
    return 0;
}
    
/*--------------------------------------------------------------------
 * ���C���E�C���h�E�̃E�C���h�E�v���V�W��
 * *-------------------------------------------------------------------*/
LRESULT CALLBACK
MainWndProc(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_CREATE, DoCreate);
        HANDLE_MSG(hWnd, WM_CLOSE, DoClose);
        HANDLE_MSG(hWnd, WM_DESTROY, DoDestroy);
        HANDLE_MSG(hWnd, WM_COPYDATA, DoCopyData);
        HANDLE_MSG(hWnd, WM_TASKMENU, DoTaskMenu);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommand);
        HANDLE_MSG(hWnd, WM_REALCLOSE, DoRealClose);
        HANDLE_MSG(hWnd, WM_SIZE, DoSize);
        HANDLE_MSG(hWnd, WM_TIMER, DoTimer);
        HANDLE_MSG(hWnd, WM_SUSPEND, DoSuspend);
    default:
        // ��L�̉���̃��b�Z�[�W�ȊO�̏ꍇ�̓f�t�H���g�̃v���V�W��
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/*--------------------------------------------------------------------
 * �ڍ׃E�C���h�E�̃E�C���h�E�v���V�W��(���͖w�ǉ������Ȃ�)
 * *-------------------------------------------------------------------*/
LRESULT CALLBACK
MainWndProcDetail(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    )
{
    switch (uMsg) {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        ShowWindow(hWnd, SW_HIDE);
        return 0;
    case WM_REALCLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);                     // ���b�Z�[�W���[�v�I��
        return 0;
    default:
        // ��L�̉���̃��b�Z�[�W�ȊO�̏ꍇ�̓f�t�H���g�̃v���V�W��
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
