/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-22 00:43:37 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̈���v���r���[
 *
 * $Id: prev.c,v 1.3 2001/12/23 10:07:54 tfuruka1 Exp $
 * $Log: prev.c,v $
 * Revision 1.3  2001/12/23 10:07:54  tfuruka1
 * �v���r���[�p�̃r�b�g�}�b�v�����m�N���ɕύX�B�v�����^�̉𑜓x�Ɖ�ʂ̉�
 * ���x�̔䗦�Ńv���r���[���s���Ă������A�ǂ����Ă��덷��������ׁA�v��
 * �r���[�̉𑜓x���v�����^�̉𑜓x�Ɠ����ɂ������A��������ƁA�r�b�g�}�b
 * �v�̍쐬�ŁA�������s������������ׁA���m�N���r�b�g�}�b�v�ɕύX�����B
 *
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
    int nDPIW, nDPIH;                           // ��ʂ̉𑜓x
    int wd, ht;                                 // ���ƍ���
    HDC hDC, hDCMem;                            // �������f�o�C�X�R���e�L�X�g
    HBITMAP hBitMap;                            // �������r�b�g�}�b�v

    switch (uMsg) {
    case WM_INITDIALOG:
        SetWindowText(hWnd, PV_CAPTION);
        SetWindowPos(hWnd, HWND_TOPMOST,
                     pPrevInfo->rc.left, pPrevInfo->rc.top,
                     pPrevInfo->rc.right - pPrevInfo->rc.left,
                     pPrevInfo->rc.bottom - pPrevInfo->rc.top, 0);
        // ��ʂ̉𑜓x�𓾂�
        nDPIW = GetDeviceCaps(pPrevInfo->hDC, LOGPIXELSX);
        nDPIH = GetDeviceCaps(pPrevInfo->hDC, LOGPIXELSY);

        // ��ʂƉ𑜓x�Ɍ����������ƍ����𓾂�
        wd = pPrevInfo->wd * nDPIW / pPrevInfo->dpiW;
        ht = pPrevInfo->ht * nDPIH / pPrevInfo->dpiH;

        // �r�b�g�}�b�v���쐬����
        if (!(hBitMap = CreateBitmap(wd, ht, 1, 1, NULL))) {
            MessageBox(g_MailBox.hWnd, 
                       GetLastErrorMessage("CreateBitmap" __FILE__,
                                           GetLastError()),
                       SV_CAPTION, MB_ERROR);
            return FALSE;
        }
        hDC = GetDC(hWnd);
        hDCMem = CreateCompatibleDC(hDC);
        ReleaseDC(hWnd, hDC);

        SelectObject(hDCMem, hBitMap);

        // �v���r���[�C���[�W��]��
        if (!StretchBlt(hDCMem, 0, 0, wd, ht,
                    pPrevInfo->hDC, 0, 0, pPrevInfo->wd, pPrevInfo->ht,
                    SRCCOPY)) {
            int nErr = GetLastError();
            DbgPrint(NULL, 'E', "InitDialog: %s",
                     GetLastErrorMessage("StretchBlt()", nErr));
        }

        // �v���r���[���Ɋi�[����Ă����r�b�g�}�b�v����j�����A�V
        // ���������i�[����
        DeleteDC(pPrevInfo->hDC);
        DeleteObject(pPrevInfo->hBitmap);

        pPrevInfo->hDC = hDCMem;
        pPrevInfo->hBitmap = hBitMap;
        pPrevInfo->wd = wd;
        pPrevInfo->ht = ht;

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
 * �g�����L���Ă���r�b�g�}�b�v���j������K�v������܂��B(�����́A���
 * �̉𑜓x�𓾂āA�v�����^�̉𑜓x�Ƃ̔䗦����A�r�b�g�}�b�v�̃T�C�Y
 * ���v�Z���Ă��܂������A���̕��@���Ƃǂ����Ă��덷�������Ă��܂��A�v
 * ���r���[�ƈ���ňقȂ������ʂ������邽�߁A���݂́A�v�����^�̉�
 * �x�Ɠ����𑜓x�Ń��m�N���r�b�g�}�b�v���쐬����悤�ɂ��Ă��܂��B�����
 * �ƁA���������������ǁE�E�E)
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
#if 0
    pPrev->dpiW = GetDeviceCaps(pPrev->hDC, LOGPIXELSX);
    pPrev->dpiH = GetDeviceCaps(pPrev->hDC, LOGPIXELSY);
#else
    pPrev->dpiW = nDPIWPrt;
    pPrev->dpiH = nDPIHPrt;
#endif
    // �v�����^�̗p���T�C�Y�𓾂�
    nWidthPrt = GetDeviceCaps(hDCPrinter, PHYSICALWIDTH);
    nHeightPrt = GetDeviceCaps(hDCPrinter, PHYSICALHEIGHT);

    // ��ʂ̃T�C�Y�����肷��
#if 0
    pPrev->wd = nWidthPrt * pPrev->dpiW / nDPIWPrt;
    pPrev->ht = nHeightPrt * pPrev->dpiH / nDPIHPrt;
#else
    pPrev->wd = nWidthPrt;
    pPrev->ht = nHeightPrt;
#endif
    // �v�����^�̈���s�G���A�𓾂�
    nPaperMarginW = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETX);
    nPaperMarginH = GetDeviceCaps(hDCPrinter, PHYSICALOFFSETY);

    // ��ʂ̈���s�G���A��ݒ肷��
#if 0
    pPrev->xoff = nPaperMarginW * pPrev->dpiW / nDPIWPrt;
    pPrev->yoff = nPaperMarginH * pPrev->dpiH / nDPIHPrt;
#else
    pPrev->xoff = nPaperMarginW;
    pPrev->yoff = nPaperMarginH;
#endif

    // �r�b�g�}�b�v���쐬����
    if (!(pPrev->hBitmap
          = /*CreateCompatibleBitmap(hDC, pPrev->wd, pPrev->ht)*/
          CreateBitmap(pPrev->wd, pPrev->ht, 1, 1, NULL))) {
        MessageBox(g_MailBox.hWnd, 
                   GetLastErrorMessage("CreateCompatibleBitmap" __FILE__,
                                       GetLastError()),
                   SV_CAPTION, MB_ERROR);
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
