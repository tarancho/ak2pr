/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-01-18 19:33:37 tfuruka1>
 *
 * ak2ps�̂悤�Ȃ��̂̃e�X�g�󎚊֘A
 *
 * $Id: testprint.c,v 1.4 2004/01/19 05:39:09 tfuruka1 Exp $
 * $Log: testprint.c,v $
 * Revision 1.4  2004/01/19 05:39:09  tfuruka1
 * �t�H���g�����w��o����悤�ɂȂ������Ɋւ���C�����s���܂����B
 *
 * Revision 1.3  2001/12/23 10:21:43  tfuruka1
 * ���v���r���[���[�h�̔��f���@���C��
 *
 * Revision 1.2  2001/12/18 04:01:00  tfuruka1
 * �v���r���[��\���ł���悤�ɏC��
 *
 * Revision 1.1  2001/02/05 17:42:12  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

/* -------------------------------------------------------------------
 * �N���X��`�悷��B(x, y)�𒆐S�Ƃ��Ĕ��ar�̉~�̒��ɃN���X��`�悷��B
 * �f�o�C�X�R���e�L�X�g�͌Ăяo�����ŗp�ӂ�, �y�����Z���N�g����Ă���
 * ��Ԃł��鎖�B
 * *-----------------------------------------------------------------*/
static void DrawCross(
    HDC hDC,                                    // �f�o�C�X�R���e�L�X�g
    int x,
    int y,
    int xr,                                     // ���������̔��a
    int yr                                      // ���������̔��a
    )
{ 
    POINT pt[2];

    Arc(g_MailBox.hDC, x - xr, y - yr, x + xr, y + yr, x + xr, y, x + xr, y);

    pt[0].x = pt[1].x = x;
    pt[0].y = y + yr;
    pt[1].y = y - yr;
    Polyline(g_MailBox.hDC, &pt[0], 2);

    pt[0].y = pt[1].y = y;
    pt[0].x = x + xr;
    pt[1].x = x - xr;
    Polyline(g_MailBox.hDC, &pt[0], 2);
}

/* -------------------------------------------------------------------
 * �g���{��`�悷��B�p����1inchi�����Ƀg���{��؂�
 * *-----------------------------------------------------------------*/
void DrawTombow(void)
{
    HPEN hPen, hOldPen;                         // �y��
    POINT pt[4];
    int x, y, i, nWd;                           // �ėp
    int nPaperWidth = GetPrtWd();
    int nPaperHeight = GetPrtHt();
    int nDPIW = GetPrtDpiW();
    int nDPIH = GetPrtDpiH();
    int nPaperMarginW = GetPrtMgW();
    int nPaperMarginH = GetPrtMgH();

    hPen = CreatePrtPen(PS_SOLID, ConvX2Dt(0.005, nDPIW, CX_CM), RGB(0, 0, 0));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    // ------------------------------
    // ��Ɖ��̒����̃g���{��`�悷��
    // ------------------------------
    pt[0].x = pt[1].x = GetPrtCenter(GPC_W);
    pt[0].y = GetPrtDpiH() - GetPrtMgH();
    pt[1].y = pt[0].y - ConvX2Dt(2, GetPrtDpiH(),CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ��̏c��

    pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[1].y = pt[0].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ��̏c��

    pt[0].x = nPaperWidth / 2 - nDPIW / 2 - nPaperMarginW;
    pt[1].x = pt[0].x + nDPIW;
    pt[0].y = pt[1].y = nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ��̉���
    
    pt[0].y = pt[1].y = nPaperHeight - nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ���̉���

    // �I�t�Z�b�g�����̃g���{�̈ʒu
    DrawCross(g_MailBox.hDC, nPaperWidth / 2, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    DrawCross(g_MailBox.hDC, nPaperWidth / 2, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // ----------------------
    // ���E�̃g���{��`�悷��
    // ----------------------
    pt[0].y = pt[1].y = nPaperHeight / 2 - nPaperMarginH;
    pt[0].x = nDPIW - nPaperMarginW;
    pt[1].x = pt[0].x - ConvX2Dt(2, nDPIW, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ���̉���

    pt[0].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].x = pt[0].x + ConvX2Dt(2, nDPIW, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // �E�̉���

    pt[0].x = pt[1].x = nDPIW - nPaperMarginW;
    pt[0].y = nPaperHeight / 2 - nDPIH / 2 - nPaperMarginH;
    pt[1].y = pt[0].y + nDPIH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ���̏c��

    pt[0].x = pt[1].x = nPaperWidth - nDPIW - nPaperMarginW;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // �E�̏c��

    // �I�t�Z�b�g�����̃g���{�̈ʒu
    DrawCross(g_MailBox.hDC, nDPIW, nPaperHeight / 2,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nPaperHeight / 2,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // ������̃g���{
    // --------------
    pt[1].x = pt[2].x = nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x - ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y - ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // �I�t�Z�b�g�����̏ꍇ
    DrawCross(g_MailBox.hDC, nDPIW, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM),
              ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // �E����̃g���{
    // --------------
    pt[1].x = pt[2].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x + ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y - ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // �I�t�Z�b�g�����̏ꍇ
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // �������̃g���{
    pt[1].x = pt[2].x = nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x - ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // �I�t�Z�b�g�����̏ꍇ
    DrawCross(g_MailBox.hDC, nDPIW, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // �E�����̃g���{
    // --------------
    pt[1].x = pt[2].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x + ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // �I�t�Z�b�g�����̏ꍇ
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    // ----------------
    // �������̕`��(��)
    // ----------------
    pt[0].x = x = nDPIW - nPaperMarginW;
    pt[1].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[0].y = pt[1].y = y =
        nPaperHeight - nDPIH - nPaperMarginH + ConvX2Dt(0.5, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // ���̐�

    nWd = (int)((nPaperWidth - nDPIW * 2) * (25.4 * 2) / nDPIW);
    pt[0].y = y;
    for (i = 0; i <= nWd; i++) {
        pt[0].x = pt[1].x = x + ConvX2Dt(i / 20.0, nDPIW, CX_CM);
        switch(i % 20) {
        case 0:
            pt[1].y = pt[0].y + ConvX2Dt(0.7, nDPIH, CX_CM);
            break;
        case 10:
            pt[1].y = pt[0].y + ConvX2Dt(0.5, nDPIH, CX_CM);
            break;
        default:
            if (i % 2) {
                pt[1].y = pt[0].y + ConvX2Dt(0.15, nDPIH, CX_CM);
            }
            else {
                pt[1].y = pt[0].y + ConvX2Dt(0.3, nDPIH, CX_CM);
            }
            break;
        }
        Polyline(g_MailBox.hDC, &pt[0], 2);     // �c�̐�
    }
    // ----------------
    // �������̕`��(�c)
    // ----------------
    pt[0].x = pt[1].x = x = nPaperWidth - nDPIW
        + ConvX2Dt(.5, nDPIW, CX_CM) - nPaperMarginW;
    pt[0].y = y = nDPIH - nPaperMarginH;
    pt[1].y = nPaperHeight - nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // �c�̐�

    nWd = (int)((nPaperHeight - nDPIH * 2) * (25.4 * 2) / nDPIH);
    pt[0].x = x;
    for (i = 0; i <= nWd; i++) {
        pt[0].y = pt[1].y = y + ConvX2Dt(i / 20.0, nDPIW, CX_CM);
        switch(i % 20) {
        case 0:
            pt[1].x = pt[0].x + ConvX2Dt(0.7, nDPIW, CX_CM);
            break;
        case 10:
            pt[1].x = pt[0].x + ConvX2Dt(0.5, nDPIW, CX_CM);
            break;
        default:
            if (i % 2) {
                pt[1].x = pt[0].x + ConvX2Dt(0.15, nDPIW, CX_CM);
            }
            else {
                pt[1].x = pt[0].x + ConvX2Dt(0.3, nDPIW, CX_CM);
            }
            break;
        }
        Polyline(g_MailBox.hDC, &pt[0], 2);     // ���̐�
    }

    // �㏈��
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
}

/* -------------------------------------------------------------------
 * �e�X�g������s��
 * *-----------------------------------------------------------------*/
void
DoTestPrint(void)
{
    HFONT hFont, hFontOld;                      // �t�H���g
    RECT rc;                                    // ��`
    TCHAR szBuf[4096], szUser[64];
    SYSTEMTIME st;                              // �V�X�e������
    int i;
    int nPaperWidth, nPaperHeight, nDPIW, nDPIH, nPaperMarginW, nPaperMarginH;

    DbgPrint(NULL, 'I', "�e�X�g����J�n");

    BeginDocument();                            // �v�����^������

    nPaperWidth = GetPrtWd();
    nPaperHeight = GetPrtHt();
    nDPIW = GetPrtDpiW();
    nDPIH = GetPrtDpiH();
    nPaperMarginW = GetPrtMgW();
    nPaperMarginH = GetPrtMgH();

    StartPage(g_MailBox.hDC);                   // �y�[�W�J�n

    rc.top = rc.right = 0;
    rc.left = nPaperWidth;
    rc.bottom = nPaperHeight;
    FillRect(g_MailBox.hDC, &rc, GetStockObject(WHITE_BRUSH));

    DrawTombow();                               // �g���{�̈��

    SetTextColor(g_MailBox.hDC, RGB(0, 0, 0));

    // �^�C�g���p�t�H���g�̍쐬
    hFont = CreatePrtFont(ConvX2Dt(20, nDPIH, CX_PT), 400,
                          TRUE, FALSE, FALSE, &g_MailBox.PrtInfo.lfPPF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    // �^�C�g���̕\���̈�̐ݒ�
    rc.top = nDPIH - nPaperMarginH;
    rc.left = nDPIW - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(20, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW - nPaperMarginW;

    wsprintf(szBuf, "ak2pr %s �e�X�g��", VERSION);
    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);

    // �㏈��
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);

    // �t�b�_�p�t�H���g�̍쐬
    hFont = CreatePrtFont(ConvX2Dt(8, nDPIH, CX_PT), 400,
                          TRUE, FALSE, FALSE, &g_MailBox.PrtInfo.lfPPF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    rc.top = nPaperHeight - nDPIH - ConvX2Dt(30, nDPIH, CX_PT) - nPaperMarginH;
    rc.left = nDPIW - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(30, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW - nPaperMarginW;

    GetLocalTime(&st);
    i = 63;
    if (!GetUserName(szUser, &i)) {
        strcpy(szUser, "hogehoge");
    }

    wsprintf(szBuf, "���̗p����%d�N%d��%d��%d��%d��%d�b%d�~���b��"
             "%s�ɂ���Ĉ������܂����B\n" COPYRIGHT,
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
             st.wSecond, st. wMilliseconds, szUser);

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);

    // �㏈��
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);

    // �{���p�t�H���g�̍쐬
    hFont = CreatePrtFont(ConvX2Dt(10, nDPIH, CX_PT), 400,
                          FALSE, FALSE, FALSE, &g_MailBox.PrtInfo.lfTHF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    rc.top = nDPIH + ConvX2Dt(30, nDPIH, CX_PT) - nPaperMarginH;
    rc.left = nDPIW + ConvX2Dt(10, nDPIW, CX_PT) - nPaperMarginW;
    rc.bottom = nPaperHeight - nDPIH + ConvX2Dt(30, nDPIH, CX_PT)
        - nPaperMarginH;
    rc.right = nPaperWidth - nDPIW - ConvX2Dt(10, nDPIW, CX_PT)
        - nPaperMarginW;

    sprintf(szBuf, "�@���̗p���̃T�C�Y���v�����^�h���C�o�Őݒ肵���p���T�C�Y��"
            "��v���Ă���Ȃ��, 1�C���`(2.54cm)�����Ƀg���{���؂��Ă��锤"
            "�ł��B"
            "�������؂��Ă��Ȃ��ꍇ��, �p���T�C�Y���قȂ��Ă��邩, "
            "�v�����^�h���C�o"
            "���R�������Ă��邩, �ɂ߂��͎��̌v�Z�~�X�ł��B"
            "���݂�, �v�����^�h���C�o���F�����Ă��錻�݂̗p���T�C�Y�͉�%d"
            "�h�b�g, �c%d�h�b�g�ł��B���̃v�����^�̉𑜓x�͉�������%ddpi, "
            "�c������%ddpi�Ȃ̂�, "
            "�p���T�C�Y�͉�%f�C���`����%fcm, �c%f�C���`����%fcm�ɂȂ�܂��B\n"
            "\n�@�𑜓x�̒P��dpi��1�C���`����̃h�b�g�������킵�܂��̂�"
            "1�C���`�����Ƀg���{��؂�͕̂��G�Ȍv�Z��K�v�Ƃ��܂���B"
            "�P���ɉ𑜓x�������Z���邾���ō��W�����߂鎖���o���܂��B"
            "���̃v�����^�Ƃ��̗p���̑g�ݍ��킹�̏ꍇ��"
            "(%d, %d)-(%d, %d)�͈̔͂Ƀg���{���؂��Ă��鎖�ɂȂ�܂��B"
            "�A��, ���̃v�����^�ɂ͈���s�\�ȗ̈�Ƃ���, "
            "��������%d�h�b�g(%f�C���`����%fmm), "
            "�c������%d�h�b�g(%f�C���`����%fmm)"
            "���ꂼ�ꑶ�݂��Ă��܂��̂�, ���m�ȍ��W��"
            "(%d, %d)-(%d, %d)�ɂȂ�܂��B\n\n"
            "�@�܂�, �g���{�̓����̃h�b�g����, �p���T�C�Y���炻�ꂼ��̕���"
            "�̉𑜓x��2���|�����l�����Z�����l�ɂȂ�܂��B���̗p���̏ꍇ��"
            "����%d�h�b�g, �c��%d�h�b�g�ɂȂ�܂��B�����̒l��cm�ɕϊ�"
            "�����, ��%fcm, �c%fcm�ɂȂ�܂��B�g���{�̓����̒���������"
            "���������������Ă��锤�ł��̂�, �m�F���Ă݂ĉ������B"
            "\n\n�@���̒��Ɂ{�̃}�[�N�͈���s�̈�𖳎������ꍇ�̃g���{"
            "�̈ʒu�ł��B�]����, ���ۂ̃g���{�̈ʒu���炱�̃}�[�N�̒��S"
            "���̋����ƈ���s�̈�̃T�C�Y����v���Ă��锤�ł��B"
            "\n\n\n\n"
            "\t����      \t�h�b�g\t�C���`   \tcm\n"
            "\t------------------------------------------\n"
            "\t�p���̕�  \t%d\t%09.6f\t%09.6f\n"
            "\t�p���̍���\t%d\t%09.6f\t%09.6f\n"
            "\t�󎚕s��\t%d\t%09.6f\t%09.6f\n"
            "\t�󎚕s�c\t%d\t%09.6f\t%09.6f\n"
            "\t------------------------------------------\n"
            "\t�𑜓x: %d�~%d\n",
            nPaperWidth, nPaperHeight, nDPIW, nDPIH,
            (double)nPaperWidth / nDPIW,
            (double)nPaperWidth / nDPIW * 2.54,
            (double)nPaperHeight / nDPIH,
            (double)nPaperHeight / nDPIH * 2.54,
            nDPIW, nDPIH, nPaperWidth - nDPIW, nPaperHeight - nDPIH,
            nPaperMarginW,
            (double)nPaperMarginW / nDPIW,
            (double)nPaperMarginW / nDPIW * 25.4,
            nPaperMarginH,
            (double)nPaperMarginH /nDPIH,
            (double)nPaperMarginH /nDPIH * 25.4,
            nDPIW - nPaperMarginW, nDPIH - nPaperMarginH,
            nPaperWidth - nDPIW - nPaperMarginW,
            nPaperHeight - nDPIH - nPaperMarginH,
            nPaperWidth - nDPIW * 2, nPaperHeight - nDPIH * 2,
            (double)(nPaperWidth - nDPIW * 2) / nDPIW * 2.54,
            (double)(nPaperHeight - nDPIH * 2) / nDPIH * 2.54,
            nPaperWidth,
            (double)nPaperWidth / nDPIW,
            (double)nPaperWidth / nDPIW * 2.54,
            nPaperHeight,
            (double)nPaperHeight / nDPIH,
            (double)nPaperHeight / nDPIH * 2.54,
            nPaperMarginW,
            (double)nPaperMarginW / nDPIW,
            (double)nPaperMarginW / nDPIW * 2.54,
            nPaperMarginH,
            (double)nPaperMarginH /nDPIH,
            (double)nPaperMarginH /nDPIH * 2.54,
            nDPIW, nDPIH);

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_EXPANDTABS | DT_NOPREFIX | DT_LEFT | DT_WORDBREAK);

    // �㏈��
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);
    DbgPrint(NULL, 'I', "�e�X�g����I��");

    // �v���r���[�̏ꍇ�́A�v���r���[��ʂ�\������
    if (g_MailBox.PrtInfo.bPreView) {
        PrintPreview(g_MailBox.hWnd, &g_MailBox.PrevInfo);
    }
    else {
        EndPage(g_MailBox.hDC);                 // �y�[�W�o��
        EndDoc(g_MailBox.hDC);                  // �h�L�������g����
    }
}
