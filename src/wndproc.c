/* -*- mode: c++; coding: sjis-dos; -*-
 *
 * �uak2ps�̂悤�Ȃ��́v�̃E�C���h�E�v���V�W��
 *
 * $Id: wndproc.c,v 1.17 2004/01/19 05:39:32 tfuruka1 Exp $
 * $Log: wndproc.c,v $
 * Revision 1.17  2004/01/19 05:39:32  tfuruka1
 * �t�H���g�����w��o����悤�ɂȂ������Ɋւ���C�����s���܂����B
 *
 * Revision 1.16  2004/01/12 10:00:54  tfuruka1
 * ���ӒԂ��ƒZ�ӒԂ��ɑΉ����܂����B
 *
 * Revision 1.15  2003/03/29 12:52:08  tfuruka1
 * �� ���X�g�r���[�̃J�����̕��̏����l�������ύX�����B
 * �� �c�[���{�^���Ɂu�N���b�v�{�[�h����v��ǉ������B�Ȃ̂ŁA���R�A�N���b
 *    �v�{�[�h����̏�����ǉ������B
 *
 * Revision 1.14  2003/03/16 00:21:56  tfuruka1
 * �� �v���r���[�������ݒ���s���ƁA�ݒ肪�L���ɂȂ�Ȃ����ڂ���������
 *    �ŁA�C�������B
 *
 * Revision 1.13  2003/03/15 14:47:04  tfuruka1
 * �� ����ݒ���s���֐��̎d�l�ύX�ɔ����C���B
 *
 * Revision 1.12  2003/03/14 16:55:34  tfuruka1
 * �c�[���o�[�Ƀc�[���`�b�v��t���܂����B
 *
 * Revision 1.11  2003/03/14 15:57:51  tfuruka1
 * �� �u�����~�v�̏�Ԃ��A�c�[���{�^���ƃ��j���[�ňقȂ�ꍇ����������
 *    �ŁA�C�������B
 * �� �N�����ɏ�Ɂu�����~�v��Ԃɂ��Ă������A���̃R�}���h�I�v�V������
 *    �w��ł���悤�ɂ����̂ŁA�֘A���镔���̏C�����s�����B
 * �� PostScrip�֘A�̃I�v�V������ak2pr�̃R�}���h���C������*����*�w��ł�
 *    ��悤�ɂ���ׂ̉��n���쐬�����B
 *
 * Revision 1.10  2003/02/25 15:30:53  tfuruka1
 * �s�ԍ��o�͂̐���ǉ��ɂ��C��
 *
 * Revision 1.9  2001/12/23 10:20:47  tfuruka1
 * ���c�[���o�[��V�K�ǉ�
 * ���h���b�O���h���b�v��ǉ��i��ƃf�B���N�g�����̃t�@�C���͈���ΏۊO��
 *   ����悤�ɂ��Ă��܂��j
 * ���f�o�b�O�E�C���h�E�̕\���E��\���̃��j���[�ւ̔��f���A�f�o�b�O�E�C��
 *   �h�E�̃E�C���h�E�v���V�W��WM_SHOWWINDOW�ōs���悤�ɏC�������B
 *
 * Revision 1.8  2001/12/14 17:03:59  tfuruka1
 * �v���r���[�Ή�
 *
 * Revision 1.7  2001/12/10 14:06:59  tfuruka1
 * �폜�f�[�^���I������Ă��Ȃ���ԂŁA���Queue�̍폜�����s�������ɃG���[
 * ���\�������悤�ɏC���i�Ƃ������A���X�����Ȃ��Ă����̂ɁALISTBOX����
 * ListView�֕ύX�������̑Ή����R��Ă����j�B
 *
 * Revision 1.6  2001/12/08 15:21:40  tfuruka1
 * ���C����ʂ�LISTBOX��ListView�ύX�B
 *
 * Revision 1.5  2001/08/18 16:38:47  tfuruka1
 * ���I������O��DeleteQueue�֐����Ăяo���āA��ƃt�@�C�����폜����悤
 *   �ɂ����B
 * ��IDM_DELETE����M�������Ɉ��Queue���폜���鏈�����R�[�f�B���O���Ă�
 *   �����A�����̏ꏊ�ł��̏������K�v�ɂȂ����̂ŁAqueue.c��DeleteQueue��
 *   ���Ƃ��Ĕ����o���ADeleteQueue�֐����Ăяo���悤�ɂ����B
 *
 * Revision 1.4  2001/02/05 17:13:37  tfuruka1
 * �z�[���y�[�W��URL��tfuruka1����VA001687�֕ύX�����B
 *
 * Revision 1.3  2001/02/05 17:11:31  tfuruka1
 * �v�����^�̐ݒ�_�C�A���O���h���b�v�_�E�����j���[����v���p�e�B�V�[�g��
 * ���ʃ^�u�ֈړ������B
 *
 * Revision 1.2  2001/02/05 16:55:33  tfuruka1
 * �ڍ׃E�B���h�E��\���������Ɂu�����~�v���j���[�Ƀ`�F�b�N�}�[�N��t��
 * �Ă����o�O���C���B�܂��A�u�ڍ׃E�C���h�E�v���j���[�̃`�F�b�N�}�[�N���t
 * �i���j���[���\������Ă��Ȃ��Ƃ��Ƀ`�F�b�N�}�[�N���t���Ă����j�������o
 * �O���C���B
 *
 * Revision 1.1  2001/02/05 16:52:53  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#define TIME_STAMP "Time-stamp: <2004-01-19 12:23:01 tfuruka1>"

#include "ak2prs.h"

#define SZ_URL "http://www.vector.co.jp/authors/VA001687/"
#define SZ_MAIL "mailto:tfuruka1@nifty.com"
#define TMR_ID 100
#define TMR_TO 1000

#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
     (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

enum{IDC_TASK = 100, IDC_LIST, IDC_STS, IDC_TOOL};

static NOTIFYICONDATA s_ndi;                    // �^�X�N�g���C
static HWND hWndOwn;                            // �I�[�i�E�C���h�E�n���h��
static HWND hWndMain = NULL;                    // ���C���E�C���h�E
static HWND hWndList = NULL;                    // ���X�g�{�b�N�X
static HWND hWndSts = NULL;                     // �X�e�[�^�X�o�[
static HWND hWndTool = NULL;                    // �c�[���o�[
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
    int i;
    HINSTANCE hInst;
    LV_COLUMN lvcol[] = {
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 150,"�v����t����", 0},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 20,"�i�g��", 1},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 50,"�t�H���g�T�C�Y", 2},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 20,"�^�u�X�g�b�v", 3},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"�p������", 4},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"�p���T�C�Y", 5},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 100,"����t�H�[�}�b�g", 7},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 180,"�^�C�g��", 6},
        {LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
         LVCFMT_LEFT, 40,"�s�ԍ�", 7},
        {0, 0, 100, NULL, 0}};
    TBBUTTON tbb[] = {
        {0, IDM_SETUP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {1, IDM_TESTPRT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {2, IDM_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {3, IDM_STOP, TBSTATE_ENABLED /*| TBSTATE_CHECKED | TBSTATE_ELLIPSES*/,
         TBSTYLE_CHECK, 0, 0},
        {4, IDM_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {5, IDM_MAIL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {6, IDM_HTTP, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {7, IDM_SHOW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0},
        {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0},
        {8, IDM_EXIT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0}
    };

    hWndOwn = pcs->hwndParent;                  // �I�[�i�E�C���h�E�n���h��
    hWndMain = hWnd;                            // �����̃E�C���h�E
    DbgPrint(pcs->hwndParent, 'I', "AK2PR Server(%s) Start.", TIME_STAMP);

    // �^�X�N�g���C�ɃA�C�R����\������
    s_ndi.cbSize = sizeof(NOTIFYICONDATA);
    s_ndi.hWnd = hWnd; 
    s_ndi.uID = IDC_TASK; 
    s_ndi.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
    s_ndi.uCallbackMessage = WM_TASKMENU; 
    s_ndi.hIcon = LoadIcon(hInst = GetWindowInstance(hWnd),
                           MAKEINTRESOURCE(IDI_TIP));
    wsprintf(s_ndi.szTip, TEXT("%s"), SV_CAPTION);
    Shell_NotifyIcon(NIM_ADD, &s_ndi);

    // ���X�g�r���[�𐶁X����(���͂��ꂪ���Queue�̎��Ԃł���)
    InitCommonControls();
    if (NULL == (hWndList = CreateWindowEx(
        0,  WC_LISTVIEW, "(^_^)",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
        LVS_REPORT, 0, 0, 10, 10,
        hWnd, (HMENU)IDC_LIST, hInst, NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() ListViwe32", MB_ERROR);
        return FALSE;
    }

    ListView_SetExtendedListViewStyle(hWndList,
                                      LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES
                                      | LVS_EX_HEADERDRAGDROP);

    for (i = 0; lvcol[i].pszText; i++) {
        lvcol[i].iSubItem = i;
        ListView_InsertColumn(hWndList, i, &lvcol[i]);
    }

    if (!(hWndSts = CreateWindowEx(0, STATUSCLASSNAME, NULL,
                                   WS_CHILD | SBARS_SIZEGRIP | CCS_BOTTOM
                                   | WS_VISIBLE, 0, 0, 0, 0, 
                                   hWnd, (HMENU)IDC_STS, hInst, NULL))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindowEx", nErr),
                   __FILE__ "/DoCreate() Status", MB_ERROR);
        return FALSE;
    }

    // �c�[���o�[�̍쐬
    if (!(hWndTool = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE
                                     | TBSTYLE_TOOLTIPS, IDC_TOOL,
                                     sizeof(tbb) / sizeof(TBBUTTON),
                                     hInst, IDR_TOOLBAR, tbb,
                                     sizeof(tbb) / sizeof(TBBUTTON),
                                     0, 0, 40, 40, sizeof(TBBUTTON)))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateToolbarEx", nErr),
                   __FILE__ "/DoCreate() Status", MB_ERROR);
        return FALSE;
    }

    // ���[���{�b�N�X�̏�����
    g_MailBox.bRun = TRUE;                      // �X���b�h���s��
    //g_MailBox.bStop = TRUE;                     // �����~
    // ���R�}���h�I�v�V�����Őݒ肷��悤�ɕύX
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

    // �c�[���o�[�̃`�F�b�N�{�^���̏�Ԑݒ�
    SendMessage(hWndTool, TB_CHECKBUTTON, IDM_STOP,
                MAKELONG(g_MailBox.bStop, 0));

    // �h���b�O�A���h�h���b�v������
    DragAcceptFiles(hWnd, TRUE);
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
    if (SIZE_MINIMIZED == state) {              // �A�C�R�������ꂽ�ꍇ
        if (IsShellTray()) {                    // �V�F���g���C���L��ꍇ��
            ShowWindow(hWnd, SW_HIDE);          // �E�C���h�E����������
        }
        return;
    }

    /* �X�e�[�^�X�o�[�̃��T�C�Y */
    if (hWndSts) {
        SendMessage(hWndSts, WM_SIZE, state, MAKELONG(cx, cy));
    }

    /* �c�[���o�[�̃��T�C�Y */
    if (hWndTool) {
        SendMessage(hWndTool, WM_SIZE, state, MAKELONG(cx, cy));
    }

    if (hWndList) {
        RECT rc;
        RECT rct;

        GetClientRect(hWndSts, &rc);
        GetClientRect(hWndTool, &rct);
        // �c�[���o�[�̃E�C���h�E�T�C�Y�͉��̂��AClientRect�œ���ꂽ
        // �������A2�h�b�g�����̂ŁA����
        DbgPrint(NULL, 'D', "ToolBarSize: %dx%d", rct.right, rct.bottom);
        MoveWindow(hWndList, 0, rct.bottom + 2,
                   cx, cy - rc.bottom - rct.bottom - 2, TRUE);
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
        DeleteQueue(hWndList, TRUE);
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
    if (!pPrtInfo->bPreView) {
        pPrtInfo->bPreView = g_PrtInfo.bPreView;
    }
    if (-1 == pPrtInfo->bNum) {
        pPrtInfo->bNum = g_PrtInfo.bNum;
    }
    if (-1 == pPrtInfo->bShortBinding) {
        pPrtInfo->bShortBinding = g_PrtInfo.bShortBinding;
    }

    // PostScript�֘A�͌���A�R�}���h���C��������͂ł��Ȃ��̂ŁA�S��
    // �f�t�H���g�̒l���g�p����B
    strcpy(pPrtInfo->szAcrobat, g_PrtInfo.szAcrobat);
    strcpy(pPrtInfo->szGsPath, g_PrtInfo.szGsPath);
    strcpy(pPrtInfo->szGsOpt, g_PrtInfo.szGsOpt);

    // �p�����[�^�Őݒ�o���Ȃ��l��ݒ肷��(�����͑S�ăp�����[�^�Őݒ�
    // ����\��)
    pPrtInfo->bColor = g_PrtInfo.bColor;
    pPrtInfo->bNoRcvHeader = g_PrtInfo.bNoRcvHeader;
    pPrtInfo->bNoCopyright = g_PrtInfo.bNoCopyright;
    pPrtInfo->bDebug = g_PrtInfo.bDebug;

    memcpy(&pPrtInfo->lfTHF, &g_PrtInfo.lfTHF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfPPF, &g_PrtInfo.lfPPF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfOF, &g_PrtInfo.lfOF, sizeof(LOGFONT));
    memcpy(&pPrtInfo->lfOPPF, &g_PrtInfo.lfOPPF, sizeof(LOGFONT));

    // �\���̂�L���ɂ���
    pPrtInfo->valid = TRUE;

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
        break;
    case IDM_SETUP:
        SetupPrtStyle(hWnd, &g_PrtInfo);
        break;
    case IDM_TESTPRT:                           // �e�X�g��
        PrtInfo.szFileName[0] = '\0';
        strcpy(PrtInfo.szTitle, "�e�X�g��");
        PrtInfo.nNumOfUp = 0;
        PrtInfo.nTab = 0;
        PrtInfo.fFontSize = 0;
        PrtInfo.dmPaperSize = 0;
        PrtInfo.nOrientation = 0;
        PrtInfo.nType = PT_TEXT;
        // DLL��Varsion�s��v��h����, �^�C���X�^���v�𖄂߂�
        strcpy(PrtInfo.szTimeStamp, TIMESTAMP);
        strcpy(PrtInfo.szTimeStamp1, TIMESTAMP);

        cds.dwData = 0;
        cds.cbData = sizeof(PRT_INFO);
        cds.lpData = (LPVOID)&PrtInfo;
        SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
        break;
    case IDM_DELETE:
        cnt = ListView_GetSelectedCount(hWndList);
        if (0 >= cnt) {
            MessageBox(hWnd, "�폜�������f�[�^���I������Ă��܂���",
                       "�폜�ł��܂���", MB_SETFOREGROUND | MB_ICONSTOP);
            break;
        }
        i = DeleteQueue(hWndList, FALSE);

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

        // �c�[���o�[�̃`�F�b�N�{�^���̏�Ԑݒ�B���j���[����ύX����
        // ���ꍇ�Ƀc�[���o�[�̏�Ԃ��ύX����B�c�[���o�[�ŕύX���ꂽ
        // �ꍇ�͈Ӗ����������A�����͗^���Ȃ����Ȃ̂œ��Ƀ`�F�b�N�͍s
        // ��Ȃ�
        SendMessage(hWndTool, TB_CHECKBUTTON, IDM_STOP,
                    MAKELONG(g_MailBox.bStop, 0));
        break;
    case IDM_PASTE:
        // �N���b�v�{�[�h�̓��e���������
        SendPrintFromStdin(TRUE, NULL, NULL, 0, 8, 0, PT_TEXT, 0, 0, -1, -1);
        break;
    }
}

/*--------------------------------------------------------------------
 * WM_NOTIFY ���b�Z�[�W������
 * *-------------------------------------------------------------------*/
static LRESULT CALLBACK
    DoNotify(HWND hWnd,                         // �n���h��
             int idCtrl,                        // �R���g���[��
             LPNMHDR pnmh                       // Notify���
             )
{
    LPTOOLTIPTEXT lptip;

    switch (pnmh->code) {
    case TTN_NEEDTEXT:
        lptip = (LPTOOLTIPTEXT)pnmh;
        lptip->hinst = GetWindowInstance(hWnd);
        // �X�g�����O���\�[�X�ƁA�R���g���[���𓯂��ԍ��ɂ��Ă���̂ŁA
        // �ȉ��̂悤�ɂ��Ă���B����A�قȂ�ԍ��ɂ������͒��ӂ��K�v�I
        lptip->lpszText = MAKEINTRESOURCE(lptip->hdr.idFrom);
        return 1;
    default:
        break;
    }
    return 0;
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
 * �f�B���N�g�����̑S�Ẵt�@�C��������Ώۃt�@�C���Ƃ��ă��N�G�X�g��
 * ��BWM_DROPFILE�̏�������̂݌Ăяo�����
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
SendPrintDirectory(LPTSTR lpszFile)
{
    TCHAR szFind[MAX_PATH * 2];
    TCHAR szFile[MAX_PATH * 2];
    WIN32_FIND_DATA FindFileData;
    HANDLE hFile;

    sprintf(szFind, "%s/*.*", lpszFile);
    hFile = FindFirstFile(szFind, &FindFileData);
    while (INVALID_HANDLE_VALUE != hFile) {
        sprintf(szFile, "%s/%s", lpszFile, FindFileData.cFileName);
        // �f�B���N�g���̏ꍇ
        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // �������g���́A�����̐e�f�B���N�g���ȊO�̓t�@�C��������
            // �s��
            if (0 != strcmp(FindFileData.cFileName, ".")
                && 0 != strcmp(FindFileData.cFileName, "..")) {

                if (0 != lstrcmpi(szFile, GetTempDirectoryName())) {
                    SendPrintDirectory(szFile);
                }
                else {
                    DbgPrint(NULL, 'W', "%s�͍�ƃf�B���N�g���ł�", szFile);
                }
            }
        }
        else {
            // �f�B���N�g���ȊO�̏ꍇ�́A����v�����s���B�A���A��ƃf�B
            // ���N�g�����̃t�@�C���͈󎚂��Ȃ��B�������[�v�����Ⴄ��
            // �ŁB
            if (0 != _strnicmp(szFile, GetTempDirectoryName(),
                               strlen(GetTempDirectoryName()))) {
                SendPrintFromFileCopy(NULL, NULL, szFile, 0, 0, 0,
                                      PT_TEXT, 0, 0, -1, -1);
            }
            else {
                DbgPrint(NULL, 'W', "%s�͍�ƃf�B���N�g�����̃t�@�C���ł�",
                         szFile);
            }
        }

        // ���̃t�@�C������������
        if (!FindNextFile(hFile, &FindFileData)) {
            break;
        }
    }
    FindClose(hFile);
}

/*--------------------------------------------------------------------
 * WM_DROPFILE�̏������s��
 * *-------------------------------------------------------------------*/
static VOID CALLBACK
DoDropFiles(
    HWND hWnd,
    HDROP hDrop
    )
{
    TCHAR szFile[MAX_PATH];
    UINT numOfFile;
    UINT i;
    DWORD dwFa;
 
    numOfFile = DragQueryFile(hDrop, 0xFFFFFFFF, szFile, MAX_PATH);
    for (i = 0; i < numOfFile; i++) {
        DragQueryFile(hDrop, i, szFile, MAX_PATH);
        dwFa = GetFileAttributes(szFile);
        if (0xFFFFFFFF == dwFa) {
            DbgPrint(NULL, 'E', "%s",
                     GetLastErrorMessage("DropFiles()", GetLastError()));
            continue;
        }

        // �f�B���N�g���̏ꍇ�́A���̉��̃t�@�C������������
        if (FILE_ATTRIBUTE_DIRECTORY & dwFa) {
            SendPrintDirectory(szFile);
        }
        else {
            // ��ƃt�@�C�����̃t�@�C���ȊO�̏ꍇ�́A����v�����s��
            if (0 != _strnicmp(szFile, GetTempDirectoryName(),
                               strlen(GetTempDirectoryName()))) {
                SendPrintFromFileCopy(NULL, NULL, szFile, 0, 0, 0,
                                      PT_TEXT, 0, 0, -1, -1);
            }
            else {
                DbgPrint(NULL, 'W', "%s�͍�ƃf�B���N�g�����̃t�@�C���ł�",
                         szFile);
            }
        }
    }
    DragFinish(hDrop);
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
        HANDLE_MSG(hWnd, WM_DROPFILES, DoDropFiles);
        HANDLE_MSG(hWnd, WM_NOTIFY, DoNotify);
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
    case WM_SHOWWINDOW:
        if (hWndMain) {
            HMENU hMenu;
            // ���j���[�̃`�F�b�N�{�^���̍X�V
            hMenu = GetMenu(hWndMain);
            hMenu = GetSubMenu(hMenu, 0);
            CheckMenuItem(hMenu, IDM_SHOW, MF_BYCOMMAND |
                          wParam ? MF_CHECKED : MF_UNCHECKED);
        }
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
