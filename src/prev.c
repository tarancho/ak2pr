/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-18 13:07:15 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈���v���r���[
 *
 * $Id: prev.c,v 1.2 2001/12/18 04:07:51 tfuruka1 Exp $
 * $Log: prev.c,v $
 * Revision 1.2  2001/12/18 04:07:51  tfuruka1
 * �v���r���[�E�C���h�E�̈ʒu�ƃT�C�Y���L�����Ă����悤�ɏC��
 *
 * Revision 1.1  2001/12/14 17:09:38  tfuruka1
 * Initial revision
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

PPREVIEW_INFO pPrevInfo = NULL;

static VOID
DoPaint(HWND hWnd)
{
    HDC hDC;
    PAINTSTRUCT ps;
    RECT rc;
    int wd, ht;
    int x, y;

    GetClientRect(hWnd, &rc);

    // �N���C�A���g�̈�̉��Z(�N���A���g�̈�Ɏ��܂�悤�ɃA�X�y�N�g��
    // ���ێ������܂܃T�C�Y���v�Z����)
    wd = rc.right;
    ht = pPrevInfo->ht * wd / pPrevInfo->wd;
    if (ht > rc.bottom) {
        ht = rc.bottom;
        wd = pPrevInfo->wd * ht / pPrevInfo->ht;
    }

    // �N���C�A���g�̈�̒����ɔz�u
    x = rc.right / 2 - wd / 2;
    y = rc.bottom / 2 - ht / 2;
    
    hDC = BeginPaint(hWnd, &ps);

    if (wd != rc.right) {
        // �������̗]����h��Ԃ�
        rc.left = x + wd;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));

        rc.left = 0;
        rc.right = x;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));
    }
    else if (ht != rc.bottom) {
        // �c�����̗]����h��Ԃ�
        rc.top = y + ht;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));

        rc.top = 0;
        rc.bottom = y;
        FillRect(hDC, &rc, GetStockObject(DKGRAY_BRUSH));
    }

    // �v���r���[�C���[�W��]��
    if (!StretchBlt(hDC, x, y, wd, ht,
                    pPrevInfo->hDC, 0, 0, pPrevInfo->wd, pPrevInfo->ht,
                    SRCCOPY)) {
        int nErr = GetLastError();
        DbgPrint(NULL, 'E', "DoPaint: %s",
                 GetLastErrorMessage("StretchBlt()", nErr));
    }

    EndPaint(hWnd, &ps);
}

static BOOL CALLBACK
DialogProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch (uMsg) {
    case WM_INITDIALOG:
        SetWindowPos(hWnd, HWND_TOPMOST,
                     pPrevInfo->rc.left, pPrevInfo->rc.top,
                     pPrevInfo->rc.right - pPrevInfo->rc.left,
                     pPrevInfo->rc.bottom - pPrevInfo->rc.top,
                     0);
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    case WM_SYSCOMMAND:
        switch (wParam) {
        case SC_CLOSE:
            EndDialog(hWnd, FALSE);
            return TRUE;
        default:
            return FALSE;
        }
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hWnd, TRUE);
            pPrevInfo->status = PVI_PRINT;
            GetWindowRect(hWnd, &pPrevInfo->rc);
            SetPreViewPos(&pPrevInfo->rc);
            return TRUE;
        case IDCANCEL:
            EndDialog(hWnd, FALSE);
            pPrevInfo->status = PVI_CANCEL;
            return TRUE;
        default:
            return FALSE;
        }
        return FALSE;
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, FALSE);
        return TRUE;
    case WM_PAINT:
        DoPaint(hWnd);
        return TRUE;
    }
    return FALSE;
}

BOOL
PrintPreview(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    PPREVIEW_INFO ppi                           // �v���r���[���
    )
{
    int nRet;
    HINSTANCE hInst = GetWindowInstance(hWnd);

    pPrevInfo = ppi;
    nRet = DialogBox(hInst, MAKEINTRESOURCE(IDD_PREV), hWnd, DialogProc);
    if (-1 == nRet) {
        DbgPrint(NULL, 'E', "%s", GetLastErrorMessage("DialogBox",
                                                      GetLastError()));
        return FALSE;
    }
    return nRet;
}

/*--------------------------------------------------------------------
 * hDCPrinter�Ŏw�肳��Ă��f�o�C�X�R���e�L�X�g����A�v���r���[�p�̃f
 * �o�C�X�R���e�L�X�g���쐬����B�v���r���[�p�̃f�o�C�X�R���e�L�X�g��
 * �g�p���I�������ɔj������K�v������܂��B�܂��A�f�o�C�X�R���e�L�X
 * �g�����L���Ă���r�b�g�}�b�v���j������K�v������܂��B
 * *-------------------------------------------------------------------*/
HDC WINAPI
MakePreviewInfo(HWND hWnd,                      // �E�C���h�E�n���h��
                HDC hDCPrinter,                 // �v�����^DC
                PPREVIEW_INFO pPrev             // �v���r���[���
    )
{
    HDC hDC = GetDC(hWnd);
    int nDPIWPrt, nDPIHPrt;                     // �v�����^�̉𑜓x
    int nWidthPrt, nHeightPrt;                  // �v�����^�̃T�C�Y
    int nPaperMarginW, nPaperMarginH;           // ����s�G���A

    if (!hDC) {
        DbgPrint(NULL, 'E', "%s",
                 GetLastErrorMessage("GetDC" __FILE__,
                                     GetLastError()));
        return NULL;
    }

    if (!(pPrev->hDC = CreateCompatibleDC(hDC))) {
        DbgPrint(NULL, 'E', "%s",
                 GetLastErrorMessage("CreateCompatibleDC" __FILE__,
                                     GetLastError()));
        ReleaseDC(hWnd, hDC);
        return NULL;
    }

    // �v�����^�̉𑜓x�𓾂�
    nDPIWPrt = GetDeviceCaps(hDCPrinter, LOGPIXELSX);
    nDPIHPrt = GetDeviceCaps(hDCPrinter, LOGPIXELSY);

    // ��ʂ̉𑜓x�𓾂�
    pPrev->dpiW = GetDeviceCaps(pPrev->hDC, LOGPIXELSX);
    pPrev->dpiH = GetDeviceCaps(pPrev->hDC, LOGPIXELSY);

    // �v�����^�̗p���T�C�Y�𓾂�
    nWidthPrt = GetDeviceCaps(hDCPrinter, PHYSICALWIDTH);
    nHeightPrt = GetDeviceCaps(hDCPrinter, PHYSICALHEIGHT);

    // ��ʂ̃T�C�Y�����肷��
    pPrev->wd = nWidthPrt * pPrev->dpiW / nDPIWPrt;
    pPrev->ht = nHeightPrt * pPrev->dpiH / nDPIHPrt;

    // �v�����^�̈���s�G���A�𓾂�
    nPaperMarginW = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETX);
    nPaperMarginH = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETY);

    // ��ʂ̈���s�G���A��ݒ肷��
    pPrev->xoff = nPaperMarginW * pPrev->dpiW / nDPIWPrt;
    pPrev->yoff = nPaperMarginH * pPrev->dpiH / nDPIHPrt;

    // �r�b�g�}�b�v���쐬����
    if (!(pPrev->hBitmap
          = CreateCompatibleBitmap(hDC, pPrev->wd, pPrev->ht))) {
        GetLastErrorMessage("CreateCompatibleBitmap" __FILE__,
                            GetLastError());
        ReleaseDC(hWnd, hDC);
        return NULL;
    }
    SelectObject(pPrev->hDC, pPrev->hBitmap);

    DbgPrint(NULL, 'D', "�v���r���[�p�f�o�C�X�R���e�L�X�g���쐬���܂���\n"
             "     |   Printer   |   Preview\n"
             "DPI  | %5dx%5d | %5dx%5d\n"
             "SIZE | %5dx%5d | %5dx%5d",
             nDPIWPrt, nDPIHPrt, pPrev->dpiW, pPrev->dpiH,
             nWidthPrt, nHeightPrt, pPrev->wd, pPrev->ht);

    ReleaseDC(hWnd, hDC);

    return pPrev->hDC;
}
