/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2003-02-27 22:48:45 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃v�����^����֘A
 *
 * $Id: printer.c,v 1.5 2003/03/01 09:04:09 tfuruka1 Exp $
 * $Log: printer.c,v $
 * Revision 1.5  2003/03/01 09:04:09  tfuruka1
 * ���{���ȊO�̒�`��������̈�������F���{�f�B���̉e�����󂯂�ꍇ������
 * ���̂��C���B
 *
 * Revision 1.4  2001/12/23 10:10:33  tfuruka1
 * ���v���r���[�L��̔��f���@��ύX�����B
 * ��Copyright�\�����󎚂��Ȃ����[�h��p�ӂ����B
 *
 * Revision 1.3  2001/12/18 12:58:41  tfuruka1
 * �i�̕��𒴂��ĕ������󎚂��Ă��܂�������������C���B���̖��̃f�o�b
 * �O���s���ړI�ŁA�f�o�b�O�p�̃`�F�b�N�{�b�N�X������ݒ�̉�ʂɒǉ������B
 *
 * Revision 1.2  2001/12/18 04:06:40  tfuruka1
 * �v���r���[�ɑΉ����܂����B
 *
 * Revision 1.1  2001/02/05 17:46:22  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

// �t�H���g�̃e���v���[�g
static LOGFONT lft = {-13, 0, 0, 0, 800, 0, 0, 0, SHIFTJIS_CHARSET,
                      3, 2, 1, 49, TEXT("�l�r �S�V�b�N")};

// �v�����^�̌ŗL���
static int nPaperWidth, nPaperHeight;           // �p���̃T�C�Y
static int nDPIW, nDPIH;                        // �𑜓x
static int nPaperMarginW, nPaperMarginH;        // ����s�G���A
static int nPageNo;                             // �y�[�W�ԍ�
static int nBasePoint, nBaseLineK, nBaseLine;   // �x�[�X���C��
static int nCurrentX, nCurrentY;                // ���݂̍��W
static int nStartX, nEndX;                      // ���������͈̔�
static int nStartY, nEndY;                      // ���������͈̔�
static BOOL bKeisen, bKanji;                    // �r���y�ъ����t���O
static BOOL bPreviewed = FALSE;                 // T:�v���r���[�ς�
// �r���R�[�h�\
static LPTSTR szKeisen = {"������������������������������������������"
                          "������������������������������������������"
                          "������������������������������������������"
                          "����������������������������������"};

/* -------------------------------------------------------------------
 * ���𑜓xnDpi�̎��̒���value���s�N�Z�����֕ϊ�����Bvalue�̒P�ʂ�flg
 * �Ŏw�肷��BCX_PT(�|�C���g), CX_CM(�Z���`���[�g��), CX_MM(�~�����[
 * �g��)���w��o����B
 * *-----------------------------------------------------------------*/
int ConvX2Dt(double value, int nDpi, int flg)
{
    switch (flg) {
    case CX_PT:
        return (int)(value * nDpi / 72.0);
    case CX_CM:
        return (int)(value * nDpi / 2.54);
    case CX_MM:
        return (int)(value * nDpi / 25.4);
    default:
        MessageBox(NULL, "�ϊ��t���O���s���ł�", "ConvX2Dt()",
                   MB_ICONSTOP | MB_SETFOREGROUND);
        return 0;
    }
}

/* -------------------------------------------------------------------
 * ConvdDt�̋t���s���B
 * *-----------------------------------------------------------------*/
double ConvDt2X(int value, int nDpi, int flg)
{
    switch (flg) {
    case CX_PT:
        return value * 72.0 / nDpi;
    case CX_CM:
        return value * 2.54 / nDpi;
    case CX_MM:
        return value * 25.4 / nDpi;
    default:
        MessageBox(NULL, "�ϊ��t���O���s���ł�", "ConvDt2X()",
                   MB_ICONSTOP | MB_SETFOREGROUND);
        return 0;
    }
}

int GetPrtDpiW(void)
{
    return nDPIW;
}

int GetPrtDpiH(void)
{
    return nDPIH;
}

int GetPrtWd(void)
{
    return nPaperWidth;
}

int GetPrtHt(void)
{
    return nPaperHeight;
}

int GetPrtMgW(void)
{
    return nPaperMarginW;
}

int GetPrtMgH(void)
{
    return nPaperMarginH;
}

int GetPrtBasePoint(void)
{
    return nBasePoint;
}

int GetPrtCenter(int flg)
{
    int nCenter;

    if (flg & GPC_W) {
        nCenter = nPaperWidth / 2;
        if (!(flg & GPC_IM)) {
            nCenter -= nPaperMarginW;
        }
    }
    else {
        nCenter = nPaperHeight / 2;
        if (!(flg & GPC_IM)) {
            nCenter -= nPaperMarginH;
        }
    }
    return nCenter;
}

BOOL
SetupPrinter(
    HWND hWnd,                                  // �n���h��
    HGLOBAL *lphDevNames,                       // �f�o�C�X�l�[��
    HGLOBAL *lphDevMode                         // �f�o�C�X���[�h
    )
{
    PRINTDLG pd;                                // ����_�C�A���O
    BOOL bRet = FALSE;

    EnterCriticalSection(&g_csCriticalSection);// ���b�N
    DbgPrint(NULL, 'I', "�����b�N");

    DbgPrint(NULL, 'I', "�v�����^�ݒ�J�n");

    // [���]�_�C�A���O �{�b�N�X��������
    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hWnd;
    pd.hDevMode = (HANDLE)*lphDevMode;
    pd.hDevNames = (HANDLE)*lphDevNames;
    pd.hDC = (HDC)NULL;
    pd.Flags = PD_PRINTSETUP;
    pd.nFromPage = 1;
    pd.nToPage = 1;
    pd.nMinPage = 0;
    pd.nMaxPage = 0;
    pd.nCopies = 1;
    pd.hInstance = (HANDLE)NULL;
    pd.lCustData = 0L;
    pd.lpfnPrintHook = (LPPRINTHOOKPROC)NULL;
    pd.lpfnSetupHook = (LPSETUPHOOKPROC)NULL;
    pd.lpPrintTemplateName = (LPSTR)NULL;
    pd.lpSetupTemplateName = (LPSTR)NULL;
    pd.hPrintTemplate = (HANDLE)NULL;
    pd.hSetupTemplate = (HANDLE)NULL;

    /* ����_�C�A���O�{�b�N�X��\������ */
    if (!PrintDlg(&pd)) {                       // Cancel���������ꍇ
        DbgPrint(NULL, 'W', "�L�����Z���{�^������������܂���");
        goto Exit;
    }

    *lphDevNames = pd.hDevNames;
    *lphDevMode = pd.hDevMode;

    SetDefaultPrtInfo();                        // �f�o�C�X��񏑂�����

    DbgPrint(NULL, 'I', "�v�����^�ݒ�I��");

    bRet = TRUE;
 Exit:
    DbgPrint(NULL, 'I', "�����b�N����");
    LeaveCriticalSection(&g_csCriticalSection); // ���b�N����

    return bRet;
}

/* -------------------------------------------------------------------
 * �v�����^�̏����ݒ���s��
 * *-----------------------------------------------------------------*/
BOOL
BeginDocument(void)
{
    static DOCINFO di;
    TCHAR szBuf[512];

    DbgPrint(NULL, 'B', "�v�����^�������J�n");

    wsprintf(szBuf, "(^_^)%s T.Furukawa",
             !g_MailBox.PrtInfo.szFileName[0] ? "�e�X�g��" :
             (g_MailBox.PrtInfo.szTitle[0] ? g_MailBox.PrtInfo.szTitle :
              "e-mail?"));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = szBuf;
    di.lpszOutput = (LPTSTR)NULL;

    if (!g_MailBox.PrtInfo.bPreView) {
        // �v�����^�o�͂̏ꍇ
        if (SP_ERROR == StartDoc(g_MailBox.hDC, &di)) {
            int nErr = GetLastError();
            DbgPrint(NULL, 'E', "%s",
                     GetLastErrorMessage(__FILE__ " StartDoc()", nErr));
            return FALSE;
        }
        nPaperWidth = GetDeviceCaps(g_MailBox.hDC, PHYSICALWIDTH);
        nPaperHeight = GetDeviceCaps(g_MailBox.hDC, PHYSICALHEIGHT);
        nPaperMarginW = GetDeviceCaps(g_MailBox.hDC, PHYSICALOFFSETX);
        nPaperMarginH = GetDeviceCaps(g_MailBox.hDC, PHYSICALOFFSETY);

        nDPIW = GetDeviceCaps(g_MailBox.hDC, LOGPIXELSX);
        nDPIH = GetDeviceCaps(g_MailBox.hDC, LOGPIXELSY);

    }
    else {
        // �v���r���[�̏ꍇ
        nPaperWidth = g_MailBox.PrevInfo.wd;
        nPaperHeight = g_MailBox.PrevInfo.ht;
        nPaperMarginW = g_MailBox.PrevInfo.xoff;
        nPaperMarginH = g_MailBox.PrevInfo.yoff;

        nDPIW = g_MailBox.PrevInfo.dpiW;
        nDPIH = g_MailBox.PrevInfo.dpiH;

        bPreviewed = FALSE;                     // �v���r���[���ɐݒ�
    }

    DbgPrint(NULL, 'I', "�f�o�C�X���\n"
             "�T�C�Y  : %d�~%ddot��%f�~%fcm\n"
             "�}�[�W��: %ddot(Left), %ddot(Top)��%f, %fcm\n"
             "�𑜓x  : %d�~%ddpi",
             nPaperWidth, nPaperHeight,
             nPaperWidth * 2.54 / nDPIW, nPaperHeight * 2.54 / nDPIH,
             nPaperMarginW, nPaperMarginH,
             nPaperMarginW * 2.54 / nDPIW, nPaperMarginH * 2.54 / nDPIH,
             nDPIW, nDPIH);

    nCurrentX = nCurrentY = 0;
    nPageNo = 1;
    bKanji = FALSE;
    bKeisen = FALSE;

    // �x�[�X���C���X�L�b�v���̏����ݒ���s��
    nBasePoint = ConvX2Dt(g_MailBox.PrtInfo.fFontSize, nDPIH, CX_PT);
    nBaseLineK = ConvX2Dt(g_MailBox.PrtInfo.fFontSize * 1.5, nDPIH, CX_PT);
    nBaseLine = ConvX2Dt(g_MailBox.PrtInfo.fFontSize * 1.2, nDPIH, CX_PT);

    // �e�L�X�g�̃o�b�N���[�h(����)
    SetBkMode(g_MailBox.hDC, TRANSPARENT);

    DbgPrint(NULL, 'B', "�v�����^����������");
    return TRUE;
}

HPEN
CreatePrtPen(
    UINT nStyle,                                // �y���̃X�^�C��
    LONG nWidth,                                // ����
    COLORREF crColor                            // �F
    )
{
    LOGPEN lgpen;                               // �_���y��

    lgpen.lopnStyle = nStyle;
    lgpen.lopnWidth.x = nWidth;
    lgpen.lopnColor = crColor;

    return CreatePenIndirect(&lgpen);
}

HFONT
CreatePrtFont(
    LPTSTR lpszFontName,                        // �t�H���g��
    int nHeight,                                // �t�H���g�̍���
    int nWeight,                                // �t�H���g�̑���
    BOOL bItalic,                               // T: �C�^���b�N
    BOOL bUnderline,                            // T: ����
    BOOL bStrikeOut,                            // T: �ł�������
    BOOL bSjis                                  // T: ���{��
    )
{
    LOGFONT lf;

    memcpy(&lf, &lft, sizeof(LOGFONT));
    lf.lfHeight = nHeight;
    lf.lfWeight = nWeight;
    lf.lfItalic = bItalic;
    lf.lfUnderline = bUnderline;
    lf.lfStrikeOut = bStrikeOut;
    lf.lfCharSet = bSjis ? SHIFTJIS_CHARSET : ANSI_CHARSET;
    strcpy(&lf.lfFaceName[0], lpszFontName);

    return CreateFontIndirect(&lf);
}

/* -------------------------------------------------------------------
 * �y�[�W�o�͂��J�n����
 * *-----------------------------------------------------------------*/
BOOL
BeginPage(void)
{
    static int nLeft, nRight;
    time_t t;
    HPEN hPen, hOldPen;
    HBRUSH hBrush, hOldBrush;
    HFONT hFont, hOldFont;
    POINT pt[3];
    RECT rc;
    int nError, mod, i, nCenter;
    LPTSTR szErr = NULL;
    TCHAR szBuf[512];

    SetTextColor(g_MailBox.hDC, RGB(0, 0, 0));

    // n�i�g�݂̍ŏ��̒i�ȊO(������ԍ����̒i�ȊO)�̎��͍��W�̍X�V�����s��
    if (0 != (mod = ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp))) {
        double fWidth = (double)(nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp;
        nStartX = (int)(nLeft + fWidth * mod);
        nEndX = (int)(nStartX + fWidth);
        nStartX += ConvX2Dt(5, nDPIW, CX_PT);
        nEndX -= ConvX2Dt(5, nDPIW, CX_PT);
        nPageNo++;
        goto Exit;
    }
    // ----------------------------------------------
    // �ȉ��͈�ԍŏ��̒i�̈���J�n���̂ݎ��s����܂�
    // ----------------------------------------------
    if (!g_MailBox.PrtInfo.bPreView) {
        // �v���r���[�̏ꍇ�͂��̃u���b�N���̏����͍s��Ȃ�
        nError = StartPage(g_MailBox.hDC);
        if (nError <= 0) {
            DbgPrint(NULL, 'E', "%s", 
                     GetLastErrorMessage("StartPage()", GetLastError()));

            return FALSE;
        }
    }

    rc.top = rc.left = 0;
    rc.right = nPaperWidth;
    rc.bottom = nPaperHeight;
    FillRect(g_MailBox.hDC, &rc, GetStockObject(WHITE_BRUSH));

    // �O�g�̕`��
    hPen = CreatePrtPen(PS_SOLID, (int)(nDPIW / 72.0 + 1), RGB(0, 0, 0));
    hBrush = CreateSolidBrush(RGB(255, 255, 255));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    hOldBrush = SelectObject(g_MailBox.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // ���u���̏ꍇ
        RoundRect(g_MailBox.hDC,
                  nStartX = nDPIW / 2 - nPaperMarginW,
                  nStartY = nDPIH - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }
    else {                                      // �c�u���̏ꍇ
        RoundRect(g_MailBox.hDC,
                  nStartX = nDPIW - nPaperMarginW,
                  nStartY = nDPIH / 2 - nPaperMarginH,
                  nEndX = nPaperWidth - nDPIW / 2 - nPaperMarginW,
                  nEndY = nPaperHeight - nDPIH / 2 - nPaperMarginH,
                  nDPIW / 4, nDPIH / 4);
    }

    // �^�C�g���Ɩ{���̎d�؂��
    nStartY += ConvX2Dt(20, nDPIH, CX_PT);
    pt[0].x = nLeft = nStartX;
    pt[0].y = pt[1].y = nStartY;
    pt[1].x = nRight = nEndX;
    Polyline(g_MailBox.hDC, &pt[0], 2);

    // �y�[�W�̎d�؂����`�悷��
    for (i = 1; i < g_MailBox.PrtInfo.nNumOfUp; i++) {
        double fWidth = (nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp * i;
        pt[0].x = pt[1].x = (int)(fWidth + nLeft);
        pt[0].y = nStartY;
        pt[1].y = nEndY;
        Polyline(g_MailBox.hDC, &pt[0], 2);
    }
    nEndX = (int)(nStartX + (nRight - nLeft) / g_MailBox.PrtInfo.nNumOfUp
                  - ConvX2Dt(5, nDPIW, CX_PT));
    nStartX += ConvX2Dt(5, nDPIW, CX_PT);
    nStartY += ConvX2Dt(5, nDPIH, CX_PT);
    nEndY -= ConvX2Dt(5, nDPIH, CX_PT);

    SelectObject(g_MailBox.hDC, hOldBrush);
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // �Ԃ��ʒu��`�悷��
    hPen = CreatePrtPen(PS_SOLID, 1, RGB(0, 0, 0));
    hBrush = CreateSolidBrush(RGB(0, 0, 0));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    hOldBrush = SelectObject(g_MailBox.hDC, hBrush);

    if (nPaperWidth > nPaperHeight) {           // ���u���̏ꍇ
        pt[0].x = nCenter = GetPrtCenter(GPC_W);
        pt[1].y = pt[2].y = ConvX2Dt(10, nDPIH, CX_PT);
        pt[0].y = 0;
        pt[1].x = pt[0].x + ConvX2Dt(5, nDPIW, CX_PT);
        pt[2].x = pt[0].x - ConvX2Dt(5, nDPIW, CX_PT);
    }
    else {                                      // �c�u���̏ꍇ
        pt[0].x = 0;
        pt[1].x = pt[2].x = ConvX2Dt(10, nDPIW, CX_PT);
        pt[0].y = nCenter = GetPrtCenter(GPC_H);
        pt[1].y = pt[0].y + ConvX2Dt(5, nDPIH, CX_PT);
        pt[2].y = pt[0].y - ConvX2Dt(5, nDPIH, CX_PT);
    }
    Polygon(g_MailBox.hDC, &pt[0], 3);

    // 2 HOLE PUNCHI ��`�悷��B�A�����ۂ̐������K�i(JIS?)�����͒m��
    // �܂���B�������Ōv����, ���ƌ��̋�����8cm, ���̒��a��5.5mm, �p
    // ���̒[���猊�̒��_���̋�����1cm�Ƃ��ĕ`�悵�Ă��܂��B
    if (nPaperWidth > nPaperHeight) {           // ���u���̏ꍇ
        double fbtHole = ConvX2Dt(8, nDPIW, CX_CM);
        double f2rHoleW = ConvX2Dt(.55, nDPIW, CX_CM);
        double f2rHoleH = ConvX2Dt(.55, nDPIH, CX_CM);
        double fTopMg = ConvX2Dt(1, nDPIH, CX_CM) - nPaperMarginH;
        Arc(g_MailBox.hDC,
            (int)(nCenter - fbtHole / 2 - f2rHoleW / 2),
            (int)(fTopMg),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2),
            (int)(nCenter - fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2));
        Arc(g_MailBox.hDC,
            (int)(nCenter + fbtHole / 2 - f2rHoleW / 2),
            (int)(fTopMg),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg  + f2rHoleH),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg  + f2rHoleH / 2),
            (int)(nCenter + fbtHole / 2 + f2rHoleW / 2),
            (int)(fTopMg + f2rHoleH / 2));
    }
    else {
        double fbtHole = ConvX2Dt(8, nDPIH, CX_CM);
        double f2rHoleW = ConvX2Dt(.55, nDPIW, CX_CM);
        double f2rHoleH = ConvX2Dt(.55, nDPIH, CX_CM);
        double fLeftMg = ConvX2Dt(1, nDPIW, CX_CM) - nPaperMarginW;
        Arc(g_MailBox.hDC,
            (int)(fLeftMg),
            (int)(nCenter - fbtHole / 2 - f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2 + f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter - fbtHole / 2));
        Arc(g_MailBox.hDC,
            (int)(fLeftMg),
            (int)(nCenter + fbtHole / 2 - f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2 + f2rHoleH / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2),
            (int)(fLeftMg + f2rHoleW),
            (int)(nCenter + fbtHole / 2));
    }

    SelectObject(g_MailBox.hDC, hOldBrush);
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    // �y�[�W�ԍ��̏o��
    hFont = CreatePrtFont(FN_ARIAL, ConvX2Dt(8, nDPIH, CX_PT),
                          400, FALSE, FALSE, FALSE, FALSE);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);

    if (nPaperWidth > nPaperHeight) {           // ���u���̏ꍇ
        rc.left = nDPIW / 2 - nPaperMarginW;
    }
    else {                                      // �c�u���̏ꍇ
        rc.left = nDPIW - nPaperMarginW;
    }
    rc.top = nPaperHeight - nDPIH / 2 - nPaperMarginH
        + ConvX2Dt(2, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(10, nDPIH, CX_PT);

    t = 64;
    if (GetUserName(szBuf, &t)) {
        strcat(szBuf, " - ");
    }
    else {
        szBuf[0] = '\0';
    }

    t = time(NULL);
    sprintf(szBuf + strlen(szBuf), "Printout time: %s", ctime(&t));
    szBuf[strlen(szBuf) - 1] = '\0';
    strcat(szBuf, " - ");

    sprintf(szBuf + strlen(szBuf), "Page: %d",
            (nPageNo - 1) / g_MailBox.PrtInfo.nNumOfUp + 1);
    nPageNo++;

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    // Copyright�\��
    if (!g_PrtInfo.bNoCopyright) {
        hFont = CreatePrtFont(FN_ARIAL, ConvX2Dt(3, nDPIH, CX_PT),
                              400, FALSE, FALSE, FALSE, FALSE);
        hOldFont = SelectObject(g_MailBox.hDC, hFont);

        DrawText(g_MailBox.hDC, COPYRIGHT, -1, &rc,
                 DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_WORDBREAK);

        SelectObject(g_MailBox.hDC, hOldFont);
        DeleteObject(hFont);
    }

 Exit:
    // �f�o�b�O���[�h�̏ꍇ�́A����G���A��h��ׂ�
    if (g_PrtInfo.bDebug) {
        RECT rc;
        rc.left = nStartX;
        rc.right = nEndX;
        rc.top = nStartY;
        rc.bottom = nEndY;
        DrawRect(g_MailBox.hDC, &rc, RGB(0, 0, 255), PS_SOLID);
    }

    return TRUE;
}

/* -------------------------------------------------------------------
 * �p����r������
 * *-----------------------------------------------------------------*/
BOOL EndPageDocument(void)
{
    HFONT hFont, hOldFont;
    RECT rc, rc2, rcBase;
    TCHAR szBuf[768];
    int nHt;

    // �^�C�g���̏o��
    hFont = CreatePrtFont(FN_MSPG, ConvX2Dt(18, nDPIH, CX_PT),
                          700, FALSE, FALSE, FALSE, TRUE);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);
    if (nPaperWidth > nPaperHeight) {           // ���u���̏ꍇ
        rc.left = nDPIW / 2 - nPaperMarginW;
        rc.top = nDPIH - nPaperMarginH;
    }
    else {                                      // �c�u���̏ꍇ
        rc.left = nDPIW - nPaperMarginW;
        rc.top = nDPIH / 2 - nPaperMarginH;
    }
    rc.bottom = rc.top + ConvX2Dt(20, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW / 2 - nPaperMarginW;

    memcpy(&rcBase, &rc, sizeof(RECT));

    rc.left += ConvX2Dt(10, nDPIW, CX_PT);
    rc.right -= ConvX2Dt(10, nDPIW, CX_PT);
    DrawText(g_MailBox.hDC, g_MailBox.PrtInfo.szTitle, -1, &rc,
             DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    // ����Ώۂ����[���̏ꍇ�̂�
    if (PT_MAIL == g_MailBox.PrtInfo.nType) {
        // From�Ǝ�M�����̕\��
        sprintf(szBuf, "%s\r\n%s", g_MailBox.szFrom, g_MailBox.szDate);
        hFont = CreatePrtFont(FN_MSPG, ConvX2Dt(8, nDPIH, CX_PT),
                              800, FALSE, FALSE, FALSE, TRUE);
        hOldFont = SelectObject(g_MailBox.hDC, hFont);

        memcpy(&rc2, &rc, sizeof(RECT));
        nHt = DrawText(g_MailBox.hDC, szBuf, -1, &rc2,
                       DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK | DT_CALCRECT);
        rc.top += (ConvX2Dt(20, nDPIH, CX_PT) - nHt) / 2;
        rc.bottom = rc.top + nHt;
        DrawText(g_MailBox.hDC, szBuf, -1, &rc,
                 DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

        // ���t�@�����X�̕\���ƃ��b�Z�[�WID�̕\��
        memcpy(&rc, &rcBase, sizeof(RECT));
        rc.top -= ConvX2Dt(18, nDPIH, CX_PT);
        rc.bottom = rc.top + ConvX2Dt(16, nDPIH, CX_PT);
        sprintf(szBuf, "%s\r\n%s",
                g_MailBox.szReference, g_MailBox.szMessageID);
        DrawText(g_MailBox.hDC, szBuf, -1, &rc,
                 DT_NOPREFIX | DT_RIGHT | DT_WORDBREAK);

        SelectObject(g_MailBox.hDC, hOldFont);
        DeleteObject(hFont);
    }

    // �v���r���[�̏ꍇ�����ŁA�v���r���[��ʂ�\������
    if (g_MailBox.PrtInfo.bPreView) {
        if (!bPreviewed) {
            PrintPreview(g_MailBox.hWnd, &g_MailBox.PrevInfo);
        }
        bPreviewed = TRUE;                      // �v���r���[�ς݂ɐݒ�
        return FALSE;
    }
    if (0 >= EndPage(g_MailBox.hDC)) {
        int nErr = GetLastError();
        MessageBox(g_MailBox.hWnd, GetLastErrorMessage("EndPage()", nErr),
                   "EndPageDocument()", MB_ICONSTOP | MB_SETFOREGROUND);
        DbgPrint(NULL, 'E', "EndPageDocument(): %s",
                 GetLastErrorMessage("EndPage()", nErr));
        return FALSE;
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * �h�L�������g�̈������������
 * *-----------------------------------------------------------------*/
BOOL
EndDocument(void)
{
    int nErr;

    EndPageDocument();
    if (g_MailBox.PrtInfo.bPreView) {
        return FALSE;
    }

    nErr = EndDoc(g_MailBox.hDC);
    if (nErr <= 0) {
        MessageBox(g_MailBox.hWnd, GetLastErrorMessage("EndDoc()", nErr),
                   TEXT("FinePrinter"), MB_ICONSTOP);
        DbgPrint(NULL, 'E', "FilePrinter(): %s",
                 GetLastErrorMessage("EndDoc()", nErr));
        return FALSE;
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * �ꕶ���v�����^�֏o�͂���B���̊֐��͊O������Ăяo����邱�Ƃ͖����B
 * �t�H���g�͊��ɑI������Ă���K�v������B
 * *-----------------------------------------------------------------*/
static BOOL
PutcPrinter(
    LPTSTR szBuf,                               // �o�͕���
    int cbString                                // �o�͕�����
    )
{
    SIZE Size;

    if ('\r' == *szBuf || '\n' == *szBuf) {
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;
        return TRUE;
    }

    // �󎚂������ʁA���𒴂��Ȃ������ׂ�
    GetTextExtentPoint32(g_MailBox.hDC, szBuf, cbString, &Size);
    if ((nCurrentX + Size.cx) > nEndX) {        // ���𒴂��Ă���
        nCurrentX = nStartX;
        if (bKeisen) {
            nCurrentY += nBasePoint;
        }
        else {
            nCurrentY += (bKanji ? nBaseLineK : nBaseLine);
        }
        bKanji = bKeisen = FALSE;

        // ���s�������ʁA�Ō�̍s�𒴂���ꍇ�́A���y�[�W����
        if ((nCurrentY + Size.cy) > nEndY) {
            // �Ō�̒i�̏ꍇ�͔r������
            if (0 == ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp)) {
                if (!EndPageDocument()) {
                    return FALSE;
                }
            }
            if (!BeginPage()) {
                return FALSE;
            }
            nCurrentX = nStartX;
            nCurrentY = nStartY;
            bKanji = bKeisen = FALSE;
        }
    }

    // ���[���̈�����̂�
    if (PT_MAIL == g_MailBox.PrtInfo.nType) {
        if (IsKanjiSJIS(*szBuf)) {
            TCHAR szTmp[4];
            memcpy(szTmp, szBuf, 2);
            szTmp[2] = '\0';
            if (strstr(szKeisen, szTmp)) {
                bKeisen = TRUE;
            }
            bKanji = TRUE;
        }
    }

    // �f�o�b�O���[�h�̏ꍇ�́A�����̉�����`�ň͂�
    if (g_PrtInfo.bDebug) {
        RECT rc;
        rc.left = nCurrentX;
        rc.right = nCurrentX + Size.cx;
        rc.top = nCurrentY;
        rc.bottom = nCurrentY + Size.cy;
        DrawRect(g_MailBox.hDC, &rc, RGB(255, 0, 0), PS_SOLID);
    }
    TextOut(g_MailBox.hDC, nCurrentX, nCurrentY, szBuf, cbString);
    nCurrentX += Size.cx;

    return TRUE;
}

/* -------------------------------------------------------------------
 * �v�����^�֏o�͂���B�t�H���g�͊��ɑI������Ă���K�v������B
 * *-----------------------------------------------------------------*/
BOOL
PutsPrinter(LPTSTR szBuf)
{
    SIZE Size;
    TCHAR szTmp[4];
    int cbString = 1, i;

    // ��ԍŏ����̈�
    if (!nCurrentY) {
        if (!BeginPage()) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = FALSE;
        bKeisen = FALSE;
    }

    // �Ō�̕�����CR or LF�̏ꍇ�͑Ҕ����Ă���
    szTmp[0] = '\0';
    if ('\n' == *(szBuf + strlen(szBuf) - 1) ||
        '\r' == *(szBuf + strlen(szBuf) - 1)) {
        szTmp[0] = '\n';
        *(szBuf + strlen(szBuf) - 1) = '\0';
    }

    // �Z�߂ďo�͂ł��邩�ǂ����m���߂�
    GetTextExtentPoint32(g_MailBox.hDC, szBuf, strlen(szBuf), &Size);
    if ((nCurrentY + Size.cy) > nEndY) {        // �ŏI���C���𒴂��Ă���
        // �Ō�̒i�̏ꍇ�͔r������
        if (0 == ((nPageNo - 1) % g_MailBox.PrtInfo.nNumOfUp)) {
            if (!EndPageDocument()) {
                return FALSE;
            }
        }
        if (!BeginPage()) {
            return FALSE;
        }
        nCurrentX = nStartX;
        nCurrentY = nStartY;
        bKanji = bKeisen = FALSE;
    }

    if ((nCurrentX + Size.cx) < nEndX) {
        // �Z�߂ďo�͉\
        // �f�o�b�O���[�h�̏ꍇ�́A�����̉�����`�ň͂�
        if (g_PrtInfo.bDebug) {
            RECT rc;
            rc.left = nCurrentX;
            rc.right = nCurrentX + Size.cx;
            rc.top = nCurrentY;
            rc.bottom = nCurrentY + Size.cy;
            DrawRect(g_MailBox.hDC, &rc, RGB(0, 255, 0), PS_SOLID);
        }
        TextOut(g_MailBox.hDC, nCurrentX, nCurrentY, szBuf, strlen(szBuf));
        nCurrentX += Size.cx;

        if (PT_MAIL == g_MailBox.PrtInfo.nType) {
            for (i = 0; i < (int)strlen(szBuf); i++) {
                if (IsKanjiSJIS(*(LPBYTE)(szBuf + i))) {
                    TCHAR szTmp[4];
                    memcpy(szTmp, szBuf + i, 2);
                    szTmp[2] = '\0';
                    if (strstr(szKeisen, szTmp)) {
                        bKeisen = TRUE;
                    }
                    bKanji = TRUE;
                    break;
                }
            }
        }
        
        if (szTmp[0]) {                         // ���s���������݂���
            PutcPrinter(szTmp, 1);
        }
        return TRUE;
    }

    // �ȉ��͓Z�߂ďo�͂ł��Ȃ������̂ł��܂��܂Əo�͂��鏈��
    if (szTmp[0]) {
        strcat(szBuf, "\n");
    }

    for (; *szBuf; szBuf++) {
        szTmp[0] = *szBuf;
        cbString = 1;
        if (IsKanjiSJIS((BYTE)szTmp[0])) {
            szTmp[1] = *(szBuf + 1);
            szBuf++;
            cbString = 2;
        }
        if (!PutcPrinter(szTmp, cbString)) {
            return FALSE;
        }
    }
    return TRUE;
}

/* -------------------------------------------------------------------
 * lpszStr�v�����^�֏o�͂���B�t�H���g�͎w�肳�ꂽ�p�����[�^�����ɍ쐬
 * ����B
 * *-----------------------------------------------------------------*/
BOOL
SetFontAndPrint(
    LPTSTR lpszStr,                             // �o�͕�����
    LPTSTR lfFaceName,                          // �t�H���g��
    UINT lfHeight,                              // �t�H���g�̍���
    UINT lfWeight,                              // �t�H���g�̃E�G�C�g
    BOOL lfItalic,                              // T:�C�^���b�N
    BOOL lfUnderline,                           // T:����
    BOOL lfStrikeout,                           // T:�ł�������
    BOOL bJapan                                 // T:���{��t�H���g
    )
{
    HFONT hFont, hOldFont;
    BOOL bResult;

    hFont = CreatePrtFont(lfFaceName, lfHeight, lfWeight, lfItalic,
                          lfUnderline, lfStrikeout, bJapan);
    hOldFont = SelectObject(g_MailBox.hDC, hFont);

    bResult = PutsPrinter(lpszStr);

    SelectObject(g_MailBox.hDC, hOldFont);
    DeleteObject(hFont);

    return bResult;
}

/* -------------------------------------------------------------------
 * ��`��`�悷��
 * *-----------------------------------------------------------------*/
void WINAPI
DrawRect(
    HDC hDC,                                    // �f�o�C�X�R���e�L�X�g
    LPRECT lprc,                                // ��`���W
    COLORREF rgb,                               // �F
    int pnStyle                                 // ���̃X�^�C��
    )
{
    HPEN hPen, hPenOld;                         // �y��
	LOGPEN lgpen;                               // �_���y��
    POINT pt[5];

    // �y���̍쐬
    lgpen.lopnStyle = pnStyle;                  // ����
    lgpen.lopnWidth.x = 1;                      // ����1
    lgpen.lopnColor = rgb;                      // �F�͍�
    hPen = CreatePenIndirect(&lgpen);           // �y���̍쐬
    hPenOld = SelectObject(hDC, hPen);          // ���݂̃y���ɐݒ�

    pt[0].x = lprc->left, pt[0].y = lprc->top;
    pt[1].x = lprc->right - 1, pt[1].y = lprc->top;
    pt[2].x = lprc->right - 1, pt[2].y = lprc->bottom - 1;
    pt[3].x = lprc->left, pt[3].y = lprc->bottom - 1;
    pt[4].x = lprc->left, pt[4].y = lprc->top;

    Polyline(hDC, pt, 5);

    SelectObject(hDC, hPenOld);                 // �y�������ɖ߂�
    DeleteObject(hPen);                         // �y���̍폜
}
