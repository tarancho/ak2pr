/* -*- mode: C++; coding: sjis; -*-
 * Time-stamp: <2005-05-08 22:02:25 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃��[���󎚏���
 *
 * $Id: prtMail.c,v 1.7 2005/05/08 13:02:29 tfuruka1 Exp $
 * $Log: prtMail.c,v $
 * Revision 1.7  2005/05/08 13:02:29  tfuruka1
 * X-Face�֘A�̒ǉ�
 *
 * Revision 1.6  2004/08/21 05:35:46  tfuruka1
 * ���[������ŁA�w�b�_���̓�s�ڈȍ~�̃t�H���g���������Ȃ������C���B
 *
 * Revision 1.5  2004/08/16 14:24:01  tfuruka1
 * �w�b�_�ƃ{�f�B�̋�؂���n�C�t�H��8�ł��F������悤�ɂ��܂����B
 *
 * Revision 1.4  2004/06/18 00:57:56  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.3  2004/01/19 05:37:04  tfuruka1
 * �t�H���g�����w��o����悤�ɂȂ������Ɋւ���C�����s���܂����B
 *
 * Revision 1.2  2003/03/01 09:05:25  tfuruka1
 * ���s�ԍ��̈���ɑΉ�����
 * ��e-mail������̃J���[����������J���t���ɂ����B
 *
 * Revision 1.1  2001/02/05 17:46:02  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

#define SZ_IN_REPLY_TO "In-Reply-To:"
#define SZ_REFERENCES  "References:"
#define SZ_FROM        "From:"
#define SZ_DATE        "Date:"
#define SZ_RECEIVED    "Received:"
#define SZ_MAILER      "X-Mailer:"

#define MULE_GREEN RGB(0, 255, 0)
#define MULE_BLUE  RGB(0, 0, 255)
#define MULE_DRED  RGB(178, 34, 34)
#define MULE_PRP   RGB(255, 0, 255)
#define MULE_ORG   RGB(210, 105, 30)
#define MULE_GRAY  RGB(127, 127, 127)
#define MULE_BROWN RGB(139, 69, 0)
#define MULE_BLACK RGB(0, 0, 0)

VOID PrintMail(void)
{
    const static COLORREF QuoteColor[] = {
        MULE_GREEN, MULE_BLUE, MULE_PRP, MULE_BROWN, MULE_DRED
    };
    static struct {
        LPTSTR szHeader;                        // �w�b�_������
        int nWeight;                            // �E�G�C�g
        BOOL bUnderLine;                        // ����
        COLORREF crHead;                        // �J���[������̐F
        COLORREF crDetail;                      // �J���[������̐F
    } Header[] = {
        {"Subject:", 400, TRUE, MULE_BLUE, MULE_DRED},
        {"From:", 400, TRUE, MULE_BLUE, MULE_BROWN},
        {"To:", 400, FALSE, MULE_BLUE, MULE_PRP},
        {"Cc:", 400, FALSE, MULE_BLUE, MULE_PRP},
        {"Message-ID:", 400, TRUE, MULE_GREEN, MULE_GRAY},
        {"Received:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Mime-Version:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Lines:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Content-Transfer-Encoding:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Content-Type:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Sender:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Reply-To:", 400, TRUE, MULE_GREEN, MULE_GRAY},
        {"In-Reply-To:", 400, TRUE, MULE_GREEN, MULE_GRAY},
        {"References:", 400, TRUE, MULE_GREEN, MULE_GRAY},
        {"X-Mailer:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"Date:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {"X-Mew:", 400, FALSE, MULE_BLUE, MULE_ORG},
        {"Return-Path:", 400, FALSE, MULE_GREEN, MULE_GRAY},
        {NULL, 0, FALSE, 0}};
    BOOL bHeader = TRUE;
    TCHAR szBuf[1024], szLastHeader[128], szLine[8];
    TCHAR szXFace[MAX_PATH];
    LPTSTR p, p1;
    FILE *fp;
    int bTitle = FALSE, c, i;
    int nLine;                                  // �s�ԍ�
    LONG lfHeight;                              // �t�H���g�̍���
    LONG lfWeight;                              // �t�H���g�̃E�G�C�g
    BOOL lfItalic;                              // T: Italic
    BOOL lfUnderline;                           // T: Underline
    COLORREF crLast = RGB(0, 0, 0);             // �Ō�Ɏg�p�����F
    PLOGFONT pLogFont;

    DbgPrint(NULL, 'I', "���[���󎚏����J�n");

    // ���[����������������
    strcpy(g_MailBox.PrtInfo.szTitle, "�ق��ق�");
    strcpy(g_MailBox.szMessageID, "Message-Id: �킩��܂ւ�");
    strcpy(g_MailBox.szReference, " ");
    strcpy(g_MailBox.szFrom, "From: ������]");
    strcpy(g_MailBox.szDate, "Date: �s��");

    szLastHeader[0] = '\0';

    // X-FACE�����Ɏ��o��
    strcpy(szXFace, "X-FACE");
    if (!MakeTempFileAndClose("wt", szXFace)) {
        return;
    }
    if (ExecuteUncompface(g_MailBox.PrtInfo.szUncompPath,
                           g_MailBox.PrtInfo.szFileName,
                           szXFace)) {
        g_MailBox.xFaceInfo.lpData = g_MailBox.xbmBuf;
        g_MailBox.xFaceInfo.cbData = sizeof(g_MailBox.xbmBuf);
        if (!SetXBM(&g_MailBox.xFaceInfo, szXFace, SETXBM_UFACE)) {
            g_MailBox.xFaceInfo.lpData = NULL;
            g_MailBox.xFaceInfo.cbData = 0;
        }
    } else {
        // X-FACE���͖���
        g_MailBox.xFaceInfo.lpData = NULL;
        g_MailBox.xFaceInfo.cbData = 0;
    }
    unlink(szXFace);                         // X-FACE�ꎟ�t�@�C���폜

    // ���[���t�@�C�����J��
    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // �h�L�������g����J�n
        fclose(fp);
        return;
    }

    // �S�Ă̍s����������܂�
    nLine = 0;
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab, fp,
                  g_MailBox.PrtInfo.nTab)) {
        lfHeight = GetPrtBasePoint();
        lfWeight = 400;
        lfItalic = lfUnderline = FALSE;
        pLogFont = &g_MailBox.PrtInfo.lfOPPF;

        // ��s�̏ꍇ�� MHS �I���B�A��, �d�M�����̏ꍇ�̓n�C�t�H�� 8��
        // �Ńw�b�_���I��
        if (('\r' == szBuf[0] || '\n' == szBuf[0])
            || (0 == strncmp("--------", szBuf, 8))) {
            bHeader = FALSE;
            if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                crLast = MULE_BLACK;
            }
        }

        // �w�b�_�̏ꍇ
        if (bHeader) {
            // �^�C�g���̏ꍇ�̓^�C�g���ݒ�
            if (0 == memicmp("Subject: ", szBuf, 9)) {
                strncpy(g_MailBox.PrtInfo.szTitle, szBuf + 9, 76);
                g_MailBox.PrtInfo.szTitle[
                    strlen(g_MailBox.PrtInfo.szTitle) - 1] = '\0';
            } else if (0 == strnicmp("Message-Id:", szBuf, 11)) {
                // ���b�Z�[�WID�̏ꍇ�̓��b�Z�[�WID��ݒ�
                strcpy(g_MailBox.szMessageID, szBuf);
                g_MailBox.szMessageID[
                    strlen(g_MailBox.szMessageID) - 1] = '\0';
            } else if (0 == strnicmp(SZ_FROM, szBuf, strlen(SZ_FROM))) {
                // From�̏ꍇ
                strcpy(g_MailBox.szFrom, szBuf);
                g_MailBox.szFrom[strlen(g_MailBox.szFrom) - 1] = '\0';
            } else if (0 == strnicmp(SZ_DATE, szBuf, strlen(SZ_DATE))) {
                // Date�̏ꍇ
                strcpy(g_MailBox.szDate, szBuf);
                g_MailBox.szDate[strlen(g_MailBox.szDate) - 1] = '\0';
            } else if (0 == strnicmp(SZ_REFERENCES, szBuf,
                                     strlen(SZ_REFERENCES))
                       || 0 == strnicmp(SZ_IN_REPLY_TO, szBuf,
                                        strlen(SZ_IN_REPLY_TO))) {
                // ���t�@�����X�̏ꍇ�̓��t�@�����X��ݒ�
                strcpy(g_MailBox.szReference, szBuf);
                g_MailBox.szReference[
                    strlen(g_MailBox.szReference) - 1] = '\0';
            }

            // �w�b�_�̈�
            p1 = strchr(szBuf, ' ');
            if ((p = strchr(szBuf, ':')) && (NULL == p1 ||
                                             (ULONG)p < (ULONG)p1)) {
                if (' ' == *(p + 1) || '\t' == *(p + 1)) {
                    p++;
                }
                // �w�b�_�^�C�g�����������ׂ� �F �̎����͎��̎�(:��
                // �����󔒂̏ꍇ)�̕�����ޔ���A�X�g�b�p�𖄂ߍ���
                // (���`��A������Ɖ�͂���ς�(^^ 2005/05/08 �Ð�k
                c = *(p + 1);
                *(p + 1) = '\0';
                lfWeight = 700;
                TrimRight(strcpy(szLastHeader, szBuf));
            } else {
                p = szBuf;
                lfWeight = 400;
            }

            // �ȈՃw�b�_����̏ꍇ
            if (g_MailBox.PrtInfo.bNoRcvHeader) {
                // Receive�w�b�_�̏ꍇ�͈�����Ȃ�
                if (0 == stricmp(SZ_RECEIVED, szLastHeader)) {
                    continue;
                }

                // X-Mailer�ȊO��X-�w�b�_�͈�����Ȃ�
                if (0 == memicmp("X-", szLastHeader, 2) &&
                    0 != stricmp(SZ_MAILER, szLastHeader)) {
                    continue;
                }
            }

            // �w�b�_�̃^�C�g����
            if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                COLORREF crTxt = MULE_BLACK;
                for (i = 0; Header[i].szHeader; i++) {
                    if (0 == memicmp(Header[i].szHeader, szLastHeader,
                                     strlen(Header[i].szHeader))) {
                        if (' ' == szBuf[0]) {  // �p���w�b�_
                            crTxt = Header[i].crDetail;
                            pLogFont = &g_MailBox.PrtInfo.lfPPF;
                        } else {
                            crTxt = Header[i].crHead;
                        }
                        break;
                    }
                }
                SetTextColor(g_MailBox.hDC, crTxt);
                crLast = crTxt;
            }

            // �s�ԍ�����̏ꍇ�͍s�ԍ����������
            if (g_MailBox.PrtInfo.bNum) {
                sprintf(szLine, "%4d: ", ++nLine);
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                if (!SetFontAndPrint(szLine, lfHeight, 800, FALSE, FALSE,
                                     FALSE, &g_MailBox.PrtInfo.lfOF)) {
                    goto ErrorExit;
                }
                if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                    // ���̐F�ɖ߂�
                    SetTextColor(g_MailBox.hDC, crLast);
                    crLast = MULE_BLACK;
                }
            }

            // X-Face�����݂��Ă��āAFrom�w�b�_�̏ꍇ�́A�s���肵�Ă���
            if (0 == stricmp(szLastHeader, "From:")
                && g_MailBox.xFaceInfo.lpData) {
                strcpy(szLine, "\n");
                if (!SetFontAndPrint(szLine, lfHeight, lfWeight, lfItalic,
                                     lfUnderline, FALSE, pLogFont)) {
                    goto ErrorExit;
                }
            }

            if (!SetFontAndPrint(szBuf, lfHeight, lfWeight, lfItalic,
                                 lfUnderline, FALSE, pLogFont)) {
                goto ErrorExit;
            }
            // X-Face�����݂��Ă��āAFrom�w�b�_�����������ł���΁A
            // XFACE��`�悷��
            if (0 == stricmp(szLastHeader, "From:")
                && g_MailBox.xFaceInfo.lpData) {
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                DrawXFace(&g_MailBox.xFaceInfo);
            }

            // : ��������Ȃ������ꍇ�͎��̍s��
            if (p == szBuf) {
                // ���t�@�����X�̌p��
                if (0 == strnicmp(SZ_REFERENCES, szLastHeader,
                                  strlen(SZ_REFERENCES)) ||
                    0 == strnicmp(SZ_IN_REPLY_TO, szLastHeader,
                                  strlen(SZ_IN_REPLY_TO))) {
                    strcat(strcat(g_MailBox.szReference, " "),
                           TrimString(szBuf));
                }
                continue;
            }
            // �w�b�_�^�C�g�����������ׂ�NULL�����ɒׂ���Ă�������
            // �𕜌�����B���݂ɂ��̃R�����g�́A������ǂ�����Ȃ��Ȃ�
            // ��2005/05/08�ɒǋL���Ă����肵�܂��B
            *(p + 1) = c;
            p++;
            // �n�C���C�g�w�b�_���܂܂�Ă��邩������
            for (i = 0; Header[i].szHeader; i++) {
                if (0 == memicmp(Header[i].szHeader, szLastHeader,
                                strlen(Header[i].szHeader))) {
                    // �K�v�Ȃ�A���_�[���C����t�����ăw�b�_�̓��e��\��
                    if (g_MailBox.PrtInfo.bColor) {// �J���[����̏ꍇ
                        lfWeight = 400;
                        SetTextColor(g_MailBox.hDC, Header[i].crDetail);
                        crLast = Header[i].crDetail;
                    } else {
                        lfWeight = Header[i].nWeight;
                        lfUnderline = Header[i].bUnderLine;
                    }

                    if (!SetFontAndPrint(p, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE,
                                         &g_MailBox.PrtInfo.lfPPF)) {
                        goto ErrorExit;
                    }

                    // XFace�����ł���΁A��s����
                    if (0 == stricmp(szLastHeader, "From:")
                        && g_MailBox.xFaceInfo.lpData) {
                        strcpy(szLine, "\n");
                        if (!SetFontAndPrint(szLine, lfHeight, lfWeight,
                                             lfItalic, lfUnderline, FALSE,
                                             &g_MailBox.PrtInfo.lfPPF)) {
                            goto ErrorExit;
                        }
                    }

                    break;
                }
            }
            if (!Header[i].szHeader) {          // �n�C���C�g�w�b�_�ł͂Ȃ�����
                if (g_MailBox.PrtInfo.bColor) { // �J���[����̏ꍇ
                    SetTextColor(g_MailBox.hDC, MULE_GRAY);
                    crLast = MULE_GRAY;
                }
                lfWeight = 400;
                if (!SetFontAndPrint(p, lfHeight, lfWeight,
                                     lfItalic, lfUnderline, FALSE,
                                     &g_MailBox.PrtInfo.lfPPF)) {
                    goto ErrorExit;
                }
            }
            continue;                           // ���̍s����������
        }

        /* --------------------
         * �ȉ��̓{�f�B���̈��
         * -------------------- */

        // �s�ԍ�����̏ꍇ�͍s�ԍ����������
        if (g_MailBox.PrtInfo.bNum) {
            sprintf(szLine, "%4d: ", ++nLine);
            SetTextColor(g_MailBox.hDC, MULE_BLACK);
            if (!SetFontAndPrint(szLine, lfHeight, 800,
                                 FALSE, FALSE, FALSE,
                                 &g_MailBox.PrtInfo.lfOF)) {
                goto ErrorExit;
            }
            if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                // ���̐F�ɖ߂�
                SetTextColor(g_MailBox.hDC, crLast);
                crLast = MULE_BLACK;
            }
        }

        // ���p�L�����܂܂�Ă���ꍇ
        if (p1 = _mbschr(szBuf, '>')) {
            p = _mbschr(szBuf, '<');
            if (NULL == p || p1 < p) {
                lfWeight = 700;
                if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                    // ���p�L���̐��𐔂���
                    int num = 0;
                    int i;
                    for (i = 0; szBuf[i]; i++) {
                        if ('>' == szBuf[i]) {
                            num++;
                        }
                        else if ('<' == szBuf[i]) {
                            break;
                        }
                    }
                    SetTextColor(g_MailBox.hDC,
                                 QuoteColor[num % (sizeof(QuoteColor)
                                                   / sizeof(COLORREF))]);
                }
            }
        }

        if ((p = strstr(szBuf, "http:")) || (p = strstr(szBuf, "mailto:"))) {
            p = szBuf;
            while ((p1 = strstr(p, "http:")) ||
                   (p1 = strstr(p, "mailto:"))) {
                if (p != p1) {
                    c = *p1;
                    *p1 = '\0';
                    lfUnderline = FALSE;

                    if (!SetFontAndPrint(p, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE,
                                         &g_MailBox.PrtInfo.lfTHF)) {
                        goto ErrorExit;
                    }
                    *p1 = c;
                }

                if ((p = strchr(p1, ' ')) || (p = strchr(p1, '\t')) ||
                    (p = strchr(p1, '\n'))) {
                    c = *p;
                    *p = '\0';
                    lfUnderline = TRUE;

                    if (!SetFontAndPrint(p1, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE,
                                         &g_MailBox.PrtInfo.lfPPF)) {
                        goto ErrorExit;
                    }
                    *p = c;
                } else {
                    lfUnderline = TRUE;

                    if (!SetFontAndPrint(p1, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE,
                                         &g_MailBox.PrtInfo.lfPPF)) {
                        goto ErrorExit;
                    }
                    break;
                }
            }
            if (*p) {
                lfUnderline = FALSE;

                if (!SetFontAndPrint(p, lfHeight, lfWeight,
                                     lfItalic, lfUnderline, FALSE,
                                     &g_MailBox.PrtInfo.lfTHF)) {
                    goto ErrorExit;
                }
            }
        } else {
            // �ʏ�̃{�f�B��
            if (!SetFontAndPrint(szBuf, lfHeight, lfWeight,
                                 lfItalic, lfUnderline, FALSE,
                                 &g_MailBox.PrtInfo.lfTHF)) {
                goto ErrorExit;
            }
            SetTextColor(g_MailBox.hDC, MULE_BLACK);
        }
    }

ErrorExit:
    EndDocument();
    fclose(fp);

    DbgPrint(NULL, 'I', "���[���󎚏����I��");
}
