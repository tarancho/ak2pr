/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:32:45 tfuruka1>
 * $Id: dbg.c,v 1.3 2004/06/18 00:57:57 tfuruka1 Exp $
 * $Log: dbg.c,v $
 * Revision 1.3  2004/06/18 00:57:57  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.2  2001/08/19 04:55:59  tfuruka1
 * ���O�t�@�C���̊i�[�p�X���t���p�X�Ŏ��悤�ɂ����B
 *
 * Revision 1.1  2001/02/05 17:47:18  tfuruka1
 * Initial revision
 *
 * *-------------------------------------------------------------------
 * ���̃t�@�C���ł͎�Ƀf�o�b�O�x���Ɋւ���֐����`���Ă���B
 * *-------------------------------------------------------------------*/

// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h>

LPCSTR WINAPI GetLastErrorMessage(LPCSTR lpsz, DWORD dwErr);
LPCTSTR WINAPI GetMyDir(VOID);
// -----------------------------------------------------------------------
static BOOL bDumpIsNoDisplay = TRUE;            // �_���v�̓t�@�C������
// -----------------------------------------------------------------------

#define MAX_LOGFILE  (512 * 1024)               // �ő働�O�t�@�C���T�C�Y
#define MAX_LINE     1000                       // �ő�\���s
#define MEM_SIZE     (8192 * 2)                 // ����������i�[�o�b�t�@
#define IDC_LIST     200                        // LISTBOX��ID
#define FONT_H       13                         // �t�H���g�̍���

#define COLOR_GRAY   RGB(128, 128, 128)         // �O���C
#define COLOR_LTGRAY RGB(192, 192, 192)         // ���C�g�O���C
#define COLOR_WHITE  RGB(255, 255, 255)         // ��
#define COLOR_BLACK  RGB(0, 0, 0)               // ��

static WNDPROC lpWndProcOwn;                    // �e�E�C���h�E�̃v���V�W��
static HWND hWndList = NULL;                    // List�̃n���h��
static HFONT hFontN, hFontB, hFontI;            // �t�H���g
static CHAR szLogName[MAX_PATH] = "\0";         // ���O�t�@�C����
static CHAR szLogNameSave[MAX_PATH];            // ���O�t�@�C����(��)

enum {IDM_CLEAR = 1100, IDM_TITLE, IDM_ZOOM, IDM_MINIMIZE,
      IDM_MOVE, IDM_SIZE, IDM_VIEWLOG, IDM_DELETELOG, IDM_NODUMP,
      DM_NUM_OF_MENU};

/*--------------------------------------------------------------------
 * �e�E�C���h�E�̃T�u�N���X��WM_DRAWITEM�̏������s���B
 * *-------------------------------------------------------------------*/
static BOOL
DoDrawItem(LPDRAWITEMSTRUCT lpdis)
{
    LPTSTR lpszStr;                             // ������擾�p
    size_t nSize;                               // �T�C�Y�擾�p
    COLORREF crColor;                           // �F
    HPEN hPen, hOldPen;                         // �y��
    LOGPEN lgpn;                                // �_���y��
    POINT Point[2];                             // �|�C���g
    HFONT hFont, hOldFont;                      // �t�H���g

    if (-1 == lpdis->itemID)                    // ���ڂ���Ȃ̂�
        return TRUE;                            // �������Ȃ�

    switch (lpdis->itemAction) {                // �A�C�e���A�N�V������
    case ODA_DRAWENTIRE:                        // �S�̂̏�������
        // �����񒷂𓾂�
        nSize = SendMessage(lpdis->hwndItem, LB_GETTEXTLEN, lpdis->itemID, 0);
        // �K�v�ȃ��������m�ۂ���
        if (NULL == (lpszStr = malloc(nSize + 1))) {
            MessageBox(lpdis->hwndItem, "�������m�ۂɎ��s���܂���",
                       "Com - Dbgprt - DrawItem", MB_ICONSTOP);
            break;
        }
        // �\�����镶����𓾂�
        SendMessage(lpdis->hwndItem, LB_GETTEXT, lpdis->itemID,
                    (LPARAM)lpszStr);
        // ������̐F��ݒ肷��
        switch (*lpszStr) {
        case 'F':                               // WM_COPYDATA�̃t�H���[�h
            hFont = hFontB, crColor = RGB(0, 0, 128); break;
        case 'C':                               // WM_COPYDATA��M
            hFont = hFontB, crColor = RGB(0, 128, 64); break;
        case 'R':                               // Receive�֌W
            hFont = hFontN, crColor = RGB(0, 128, 0); break;
        case 'r':                               // Receive�֌W(����)
            hFont = hFontI, crColor = RGB(0, 128, 0); break;
        case 'S':                               // Send�֌W
            hFont = hFontN, crColor = RGB(0, 0, 255); break;
        case 's':                               // Send�֌W(����)
            hFont = hFontI, crColor = RGB(0, 0, 255); break;
        case 'W':                               // �E�H�[�j���O
            hFont = hFontB, crColor = RGB(255, 0, 255); break;
        case 'E':                               // �G���[
            hFont = hFontB, crColor = RGB(255, 0, 0); break;
        case 'B':                               // ����
            hFont = hFontB, crColor = RGB(0, 0, 0); break;
        case 'I': default:                      // �ʏ햔�̓f�t�H���g
            hFont = hFontN; crColor = RGB(0, 0, 0); break;
        }
        SetTextColor(lpdis->hDC, crColor);
        SetBkMode(lpdis->hDC, TRANSPARENT);
        // �������`�悷��
        hOldFont = SelectObject(lpdis->hDC, hFont);
        DrawText(lpdis->hDC, lpszStr + 1, -1, &lpdis->rcItem,
                 DT_VCENTER	| DT_SINGLELINE | DT_EXPANDTABS | DT_NOPREFIX);
        SelectObject(lpdis->hDC, hOldFont);

        free(lpszStr);                          // �������̊J��

        /* break ��Y��Ă���킯�ł͂���܂���B�I����Ԃ𕜌������
           ��, �ȉ��̏��������s�����Ă��܂�*/
    case ODA_SELECT:                            // �I��

        // �g�p������̐ݒ�
        lgpn.lopnStyle = PS_SOLID;              // ����
        lgpn.lopnWidth.x = 1;                   // ����

        // �A���_�[���C���̍��W��ݒ肷��
        Point[0].x = lpdis->rcItem.left;
        Point[1].x = lpdis->rcItem.right;
        Point[0].y = lpdis->rcItem.bottom - 1;
        Point[1].y = lpdis->rcItem.bottom - 1;
        if (ODS_SELECTED & lpdis->itemState) {  // �I����Ԃ̏ꍇ��
            lgpn.lopnColor = COLOR_WHITE;       // ��
        }
        else {                                  // ��I����Ԃ̏ꍇ��
            lgpn.lopnColor = COLOR_LTGRAY;      // �w�i�F
        }
        hPen = CreatePenIndirect(&lgpn);        // �y���̍쐬
        hOldPen = SelectObject(lpdis->hDC, hPen); // �y���̑I��

        Polyline(lpdis->hDC, &Point[0], 2);     // �A���_�[���C����`��

        SelectObject(lpdis->hDC, hOldPen);      // �y�������ɖ߂�
        DeleteObject(hPen);                     // �y�����폜

        // �I�[�o�[���C���̍��W��ݒ肷��
        Point[0].x = lpdis->rcItem.left;
        Point[1].x = lpdis->rcItem.right;
        Point[0].y = lpdis->rcItem.top;
        Point[1].y = lpdis->rcItem.top;
        if (ODS_SELECTED & lpdis->itemState) {  // �I����Ԃ̏ꍇ��
            lgpn.lopnColor = COLOR_GRAY;        // ��
        }
        else {                                  // ��I����Ԃ̏ꍇ��
            lgpn.lopnColor = COLOR_LTGRAY;      // �w�i�F
        }
        hPen = CreatePenIndirect(&lgpn);        // �y���̍쐬
        hOldPen = SelectObject(lpdis->hDC, hPen); // �y���̑I��

        Polyline(lpdis->hDC, &Point[0], 2);     // �I�[�o�[���C����`��

        SelectObject(lpdis->hDC, hOldPen);      // �y�������ɖ߂�
        DeleteObject(hPen);                     // �y�����폜
        return FALSE;
    }
    return TRUE;
}
/*--------------------------------------------------------------------
 * �e�E�C���h�E�̃T�u�N���X
 * *-------------------------------------------------------------------*/
static  LRESULT CALLBACK
SubWindProcOwn(
    HWND hWnd,                                  // �E�B���h�E�n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    )
{
    long l;
    HMENU hMenu = GetSubMenu(GetMenu(hWnd), 0);

    switch (uMsg) {
    case WM_COMMAND:
        switch((int)(LOWORD(wParam))) {
        case IDM_DELETELOG:                     // ���O�폜
            if (IDYES == MessageBox(hWnd, "���O�t�@�C�����폜���Ă�"
                                    "�X�����ł���", "�{��?",
                                    MB_YESNO | MB_ICONQUESTION)) {
                unlink(szLogNameSave);
                unlink(szLogName);
            }
            return 0;
        case IDM_NODUMP:                        // ��ʂɃ_���v�\�����Ȃ�
            bDumpIsNoDisplay = !bDumpIsNoDisplay;
            CheckMenuItem(hMenu, IDM_NODUMP,
                          bDumpIsNoDisplay ? MF_CHECKED : MF_UNCHECKED);
            return 0;
        case IDM_VIEWLOG:                       // ���O������
            ShellExecute(hWnd, NULL, szLogName, NULL, NULL, SW_SHOW);
            return 0;
        case IDM_SIZE:                          // �T�C�Y�ύX
            SendMessage(hWnd, WM_SYSCOMMAND, SC_SIZE, 0);
            return 0;
        case IDM_MOVE:                          // �ړ�
            SendMessage(hWnd, WM_SYSCOMMAND, SC_MOVE, 0);
            return 0;
        case IDM_ZOOM:                          // �ő剻
            SendMessage(hWnd, WM_SYSCOMMAND, SC_ZOOM, 0);
            return 0;
        case IDM_MINIMIZE:                      // �ŏ���
            SendMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            return 0;
        case IDM_TITLE:                         // �^�C�g���o�[�̕\��/����
            if ((l = GetWindowLong(hWnd, GWL_STYLE)) & WS_CAPTION) {
                SetWindowLong(hWnd, GWL_STYLE, l & (~WS_CAPTION));
            }
            else {
                SetWindowLong(hWnd, GWL_STYLE, l | WS_CAPTION);
            }
            SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE |
                         SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
            return 0;
        case IDM_CLEAR:                         // ��ʂ̃N���A
            if (hWndList) {
                SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
            }
            return 0;
        default:
            break;
        }
    case WM_CTLCOLORLISTBOX:                    // ���X�g�{�b�N�X�̔w�i�F
        if ((HWND)lParam != hWndList) {         // �����ł͂Ȃ�
            break;
        }
        return (LRESULT)GetStockObject(LTGRAY_BRUSH);
    case WM_DRAWITEM:                           // �I�[�i�[�`��
        if (IDC_LIST != (UINT)wParam) {         // LIST BOX �ȊO��
            break;                              // �������Ȃ�
        }
        DoDrawItem((LPDRAWITEMSTRUCT)lParam);
        return TRUE;                            // [����] See help
    case WM_MEASUREITEM:                        // �I�[�i�[�`����s������
        if (IDC_LIST != (UINT)wParam) {         // LIST BOX �ȊO��
            break;                              // �������Ȃ�
        }
        ((LPMEASUREITEMSTRUCT)lParam)->itemHeight = FONT_H + 2;
        return TRUE;                            // [����] See help
    case WM_SIZE:                               // ���T�C�Y
        if (hWndList) {
            SetWindowPos(hWndList, 0, 0, 0,
                         LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER);
        }
        return 0;
    }
    // �e�̃E�C���h�E�v���V�W�����R�[������
    return CallWindowProc(lpWndProcOwn, hWnd, uMsg, wParam, lParam);
}

/*--------------------------------------------------------------------
 * LISTBOX���쐬����
 * *-------------------------------------------------------------------*/
static HWND
CreateListBox(HWND hWnd)
{
    HWND hWndList;                              // �E�B���h�E�n���h��
    RECT rc;
    LOGFONT lf = {-FONT_H, 0, 0, 0, 400, 0, 0, 0, SHIFTJIS_CHARSET,
                  3, 2, 1, 49, TEXT("�l�r �S�V�b�N")};
    double dummy;
    char *cdummy;

    dummy = strtod("10.1", &cdummy);

    // �e�E�C���h�E�̃E�C���h�E�v���V�W�����t�b�N����
    lpWndProcOwn = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    if (!lpWndProcOwn) {
        int nErr = GetLastError();
        MessageBox(hWnd,
                   GetLastErrorMessage("GetWindowLong(GWL_WNDPROC)", nErr),
                   "�E�C���h�E�v���V�W���̃t�b�N�Ɏ��s", MB_ICONSTOP);
    }
    SetWindowLong(hWnd, GWL_WNDPROC, (long)SubWindProcOwn);

    GetClientRect(hWnd, &rc);                   // �N���C�A���g�T�C�Y�𓾂�

    hWndList = CreateWindowEx(
        0, "LISTBOX", "(^_^)",
        WS_CHILD | WS_VISIBLE | LBS_DISABLENOSCROLL | LBS_HASSTRINGS |
        LBS_USETABSTOPS	| LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL |
        WS_HSCROLL | LBS_OWNERDRAWFIXED,
        0, 0, rc.right, rc.bottom,
        hWnd, (HMENU)IDC_LIST, GetWindowInstance(hWnd), NULL);

    if (!hWndList) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("CreateWindow", nErr),
                   "DbgPrint()->CreateListBox()", MB_ICONSTOP);
        return NULL;
    }

    /* �f�t�H���g�̃t�H���g���ƃ_���v�����ɂ����Ȃ�ׁC�����t�H���g��
       �ݒ肷��*/
    hFontN = CreateFontIndirect(&lf);           // �t�H���g���쐬(����)
    lf.lfItalic = TRUE;
    hFontI = CreateFontIndirect(&lf);           // �t�H���g���쐬(It)
    lf.lfItalic = FALSE, lf.lfWeight = 700;
    hFontB = CreateFontIndirect(&lf);           // �t�H���g���쐬(Bf)
    SendMessage(hWndList, WM_SETFONT, (WPARAM)hFontN, MAKELPARAM(TRUE, 0));

    return hWndList;
}

/*--------------------------------------------------------------------
 * ���O�t�@�C���̃T�C�Y�̃`�F�b�N���s���C�t�@�C���T�C�Y�𒴂��Ă�����
 * ���́C���O�t�@�C�������l�[������B
 * *-------------------------------------------------------------------*/
static VOID
CheckLogFile(
    LPCSTR CurrentLog,                          // ���݂̃��O�t�@�C����
    LPCSTR SaveLog                              // �ۑ��t�@�C����
    )
{
    struct _stat buffer;

    if (-1 == _stat(CurrentLog, &buffer)) {     // ���O�t�@�C���̏��
        return;                                 // �߂�
    }

    if (MAX_LOGFILE < buffer.st_size) {
        unlink(SaveLog);
        rename(CurrentLog, SaveLog);
    }
}

/*--------------------------------------------------------------------
 * lpstr�Ŏw�肳�ꂽ����������������ɏ]���ĕҏW���\���E�B���h�E�֕\��
 * ����B������printf�Ɠ��l�ł���B�\���E�B���h�E�����݂��Ȃ��ꍇ�́C
 * hWnd�֕\���E�B���h�E��Create������ɕ\������BnFlag�̓��b�Z�[�W����
 * �ʂ���ׂ̃C���W�P�[�^�Ƃ��ĕ\�������BnFlag�� 'N' ������0x20�`
 * 0x7e���̉��ꂩ�̃R�[�h���w��o����B�߂�l�͏�ɕ\���E�C���h�E�̃n
 * ���h���ł���B�A���C����������ɏ]���č쐬���ꂽ�������16384������
 * ���łȂ���΂Ȃ�Ȃ��BnFlag�� 'I', 'C', 'F', 'R', 'W', 'E', 'S', '
 * s', 'r' ��F�����܂��B����ȊO��'I'�Ƃ��ď������܂��B
 *
 * ����: �\���E�C���h�E�̃R���g���[��ID��200���g�p���܂��B�]���ăI�[�i
 * �E�C���h�E�ł�ID=200�̃R���g���[���͎g�p�ł��܂���(�ʏ��ID��100��
 * ��g�p����܂��̂ŁC�Ԃ���\���͏��Ȃ����ł�)�BID=200��
 * WM_CTLCOLORLISTBOX, WM_DRAWITEM, WM_MEASUREITEM�y��, WM_SIZE���t�b
 * �N���܂��̂ŁC�����̃C�x���g�̓I�[�i�[�E�C���h�E�֒ʒm����܂���B
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    )
{
    static DWORD OldTickCount = 0;              // �O��o�͂� TickCount
    FILE *fp = NULL;                            // ���O�t�@�C���|�C���^
    va_list args;                               // �����W�J�p
    char *szLine, *p1, *p2;                     // �����񑀍�p
    SYSTEMTIME t;                               // ���
    DWORD NewTickCount, TickInterval;           // �O��o�͂���̎���
    SIZE Size;                                  // �T�C�Y�擾�p
    HDC hDC;                                    // �f�o�C�X�R���e�L�X�g
    int i, j, nMaxStrWd;                        // �ėp
    BOOL bNoDisplay = FALSE;                    // ��ʏo�͖���
    TCHAR szBuf[1024];                          // �ėp

    if (!szLogName[0]) {
        // ���O�t�@�C�������擾����
        if (!GetWindowText(hWnd, szBuf, MAX_PATH - 5)) {
            int nErr = GetLastError();
            TCHAR szHwnd[256];
            sprintf(szHwnd, "[hWnd=%d]�E�C���h�E�e�L�X�g�̎擾��"
                    "���s���܂���", hWnd);
            MessageBox(hWnd, GetLastErrorMessage("GetWindowText()", nErr),
                       szHwnd, MB_ICONSTOP);
            strcpy(szBuf, "hogehoge");
        }

        sprintf(szLogName, "%s/%d%s.log", GetMyDir(), 1, szBuf);
        sprintf(szLogNameSave, "%s/%d%s.log", GetMyDir(), 0, szBuf);

    }
    if (!hWndList) {
        HMENU hMenu = GetMenu(hWnd), hSubMenu;
        if (NULL == hMenu) {
            hMenu = CreateMenu();
            SetMenu(hWnd, hMenu);
        }
        hSubMenu = CreateMenu();

        AppendMenu(hSubMenu, 0, IDM_NODUMP, "�_���v����ʂɕ\�����Ȃ�");
        AppendMenu(hSubMenu, 0, IDM_TITLE, "�^�C�g���o�[");
        AppendMenu(hSubMenu, 0, IDM_CLEAR, "��ʏ���");
        AppendMenu(hSubMenu, MF_SEPARATOR, IDM_CLEAR, NULL);
        AppendMenu(hSubMenu, 0, IDM_VIEWLOG, "���O������");
        AppendMenu(hSubMenu, 0, IDM_DELETELOG, "���O���폜����");
        AppendMenu(hSubMenu, MF_SEPARATOR, IDM_CLEAR, NULL);
        AppendMenu(hSubMenu, 0, IDM_ZOOM, "�ő剻");
        AppendMenu(hSubMenu, 0, IDM_MINIMIZE, "�ŏ���");
        AppendMenu(hSubMenu, 0, IDM_MOVE, "�ړ�");
        AppendMenu(hSubMenu, 0, IDM_SIZE, "�T�C�Y�ύX");

        CheckMenuItem(hSubMenu, IDM_NODUMP,
                      bDumpIsNoDisplay ? MF_CHECKED : MF_UNCHECKED);

        AppendMenu(hMenu,  MF_POPUP | MF_BYPOSITION, (UINT)hSubMenu,
                   "&Special");

        DrawMenuBar(hWnd);
        if(!(hWndList = CreateListBox(hWnd))){  // ���X�g�{�b�N�X�̍쐬
            return NULL;                        // ���s�������͒��߂�
        }
    }

    if (!bNoDisplay) {
        // LISTBOX�̍ő�s���𒴂��Ă���ꍇ�͔����폜����
        if (MAX_LINE < SendMessage(hWndList, LB_GETCOUNT, 0, 0)) {
            ShowWindow(hWndList, SW_HIDE);
            for (i = 0; i < (MAX_LINE / 2); i++) {
                SendMessage(hWndList, LB_DELETESTRING, 0, 0);
            }
            ShowWindow(hWndList, SW_SHOW);
        }
    }

    if (NULL == (szLine = malloc(MEM_SIZE))) {  // �������̊m��
        MessageBox(hWnd, "�������m�ۂɎ��s���܂���",
                   "DbgPrint()", MB_SETFOREGROUND);
        return NULL;
    }

    // �\�����b�Z�[�W��ҏW����
    if ((nFlag & 0xff) == 'N') {                // �w�b�_�����̏ꍇ
        nFlag >>= 8;
        *szLine = (BYTE)nFlag;
        strcpy(szLine + 1, "\t");
        if (bDumpIsNoDisplay) {
            bNoDisplay = TRUE;                  // �f�B�X�v���C�o�͖���
        }
    }
    else {                                      // �w�b�_�L��̏ꍇ
        ULONG hh, mm, ss, ms;                   // �O��Ƃ̎���
        NewTickCount = GetTickCount();          // �N�����Ă���̎���
        if (!OldTickCount) {                    // ���߂Ẵ��O�o��
            TickInterval = 0;                   // �O��Ƃ̎��Ԃ�0
        }
        else {
            TickInterval = NewTickCount - OldTickCount;
        }
        OldTickCount = NewTickCount;            // �O��Ƃ̎��Ԃ𓾂�
        hh = TickInterval / (3600 * 1000);
        mm = (TickInterval %= (3600 * 1000)) / (60 * 1000);
        ss = (TickInterval %= (60 * 1000)) / 1000;
        ms = TickInterval % 1000;
        GetLocalTime(&t);                       // ���ݓ����̓ǂݍ���
        sprintf(szLine, "%c[%d]%02d/%02d/%02d %02d:%02d:%02d.%03d"
                "(+%02d:%02d:%02d.%03d)[%c]: ",
                nFlag, GetCurrentThreadId(),
                t.wYear % 100, t.wMonth, t.wDay,
                t.wHour, t.wMinute, t.wSecond, t.wMilliseconds,
                hh, mm, ss, ms, nFlag);
    }

    // ������������ɍ����Đ��`����
    va_start(args, lpstr);
    vsprintf(szLine + strlen(szLine), lpstr, args);
    va_end(args);

    CheckLogFile(szLogName, szLogNameSave);     // ���O�t�@�C���̃T�C�Y
    if (NULL == (fp = fopen(szLogName, "at"))) {
        MessageBox(hWnd, "���O�t�@�C���̃I�[�v���Ɏ��s���܂���",
                   "DbgPrint()", MB_ICONSTOP);
    }
    // LISTBOX�֕\��
    p1 = szLine;
    while (p1) {
        p2 = strchr(p1, '\n');
        if (p2) {
            *p2 = '\0';
        }

        if (!bNoDisplay) {
            if (0 > SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)p1)) {
                /*
                MessageBox(hWnd, "ListBox�ւ̏o�͂ŃG���[���������܂���",
                           "DbgPrint()", MB_SETFOREGROUND);
                           */
                break;
            }
        }
        if (fp) {                               // �t�@�C���|�C���^������
            fprintf(fp, "%s\n", p1 + 1);        // ���O�o��
        }
        if (p2) {
            *p2-- = '\t';
            *p2 = (BYTE)nFlag;
        }
        p1 = p2;
    }

    if (fp) {                                   // �t�@�C���|�C���^������
        fclose(fp);                             // �t�@�C���N���[�Y
    }

    if (!bNoDisplay) {
        // ��ɍŏI�s��������悤�ɂ���
        SendMessage(hWndList, LB_SETTOPINDEX,
                    SendMessage(hWndList, LB_GETCOUNT, 0, 0) - 1, 0);


        // �ł������s�̃h�b�g���𓾂�
        nMaxStrWd = 0;
        hDC = GetDC(hWndList);                  // ListBox��DC
        j = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
        for (i = 0; i < j; i++) {
            SendMessage(hWndList, LB_GETTEXT, i, (LPARAM)szLine);
            GetTextExtentPoint32(hDC, szLine, strlen(szLine), &Size);
            if (nMaxStrWd < Size.cx) {
                nMaxStrWd = Size.cx;
            }
        }
        ReleaseDC(hWndList, hDC);               // DC�̊J��
        // ���X�g�{�b�N�X�̕��𒲐�����
        SendMessage(hWndList, LB_SETHORIZONTALEXTENT,
                    (WPARAM)nMaxStrWd + 16, 0);
    }

    free(szLine);                               // �������J��
    return hWndList;
}

/*--------------------------------------------------------------------
 * lpStart����uLen�o�C�g��16�i�_���v��\������B�\���E�B���h�E�����݂�
 * �Ȃ��ꍇ�́ChWnd�֕\���E�B���h�E��Create������ɕ\������BlpszHead
 * �̓_���v�̑O�ɕ\�����镶������w�肷��BnFlag�̈Ӗ���DbgPrint��
 * �Q�Ƃ̎��B
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgDump(
    HWND hWnd,                                  // �n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpszHead,                            // �^�C�g��
    LPBYTE lpStart,                             // �J�n�A�h���X
    ULONG uLen                                  // �_���v��
    )
{
    TCHAR szBuf[128], szBufc[20];               // ��Ɨp
    HWND hWndRet;                               // �߂�l
    ULONG i;                                    // �ėp
    UCHAR uc;                                   // �ėp
    int nSameLine = 0;                          // �O16�o�C�g�Ɠ����f�[�^

    // �^�C�g����\������
    hWndRet = DbgPrint(hWnd, nFlag, "%s (Length=%d)", lpszHead, uLen);

    for (i = 0; i < ((uLen + 15) & ~15); i++) {
        if (0 == (i % 16)) {
            if (i) {
                // �������̓��e���O16�o�C�g�Ɠ����ꍇ�͏o�͂��Ȃ�
                if (i > 16 && 0 == memcmp(lpStart + i - 16,
                                          lpStart + i - 32, 16)) {
                    nSameLine++;
                }
                else {
                    if (nSameLine) {            // �o�͂���Ă��Ȃ��s���L��
                        DbgPrint(hWnd, 'N' + nFlag * 256,
                                     "\t\t==== SAME %d BYTE ====",
                                     nSameLine * 16);
                        nSameLine = 0;
                    }
                    DbgPrint(hWnd, 'N' + nFlag * 256, "%s",
                                 strcat(strcat(szBuf, szBufc), " |"));
                }
            }
            sprintf(szBuf, "%08x |", i & ~15);
            strcpy(szBufc, " | ");
        }
        if (i < uLen) {
            uc = *(lpStart + i);
            sprintf(szBuf + strlen(szBuf), " %02x", uc);
            if (0x20 > uc || 0x7e < uc) {
                uc = '.';
            }
            sprintf(szBufc + strlen(szBufc), "%c", uc);
        }
        else {
            strcat(szBuf, "   ");
            strcat(szBufc, " ");
        }
    }
    if (nSameLine) {                            // �o�͂���Ă��Ȃ��s���L��
        DbgPrint(hWnd, 'N' + nFlag * 256,
                     "\t\t==== SAME %d BYTE ====", nSameLine * 16);
    }
    DbgPrint(hWnd, 'N' + nFlag * 256,
                 "%s", strcat(strcat(szBuf, szBufc), " |"));

    return hWndRet;
}
