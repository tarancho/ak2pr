/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:46:01 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃��[���󎚏���
 *
 * $Id: prtMail.c,v 1.1 2001/02/05 17:46:02 tfuruka1 Exp $
 * $Log: prtMail.c,v $
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

#define MULE_GREEN RGB(33, 138, 33)
#define MULE_BLUE  RGB(0, 0, 255)
#define MULE_DRED  RGB(181, 32, 33)
#define MULE_PRP   RGB(165, 32, 247)
#define MULE_ORG   RGB(239, 177, 0)
#define MULE_GRAY  RGB(123, 125, 123)
#define MULE_BLACK RGB(0, 0, 0)

VOID PrintMail(void)
{
    static struct {
        LPTSTR szHeader;                        // �w�b�_������
        int nWeight;                            // �E�G�C�g
        BOOL bUnderLine;                        // ����
        COLORREF crHead;                        // �J���[������̐F
        COLORREF crDetail;                      // �J���[������̐F
    } Header[] = {
        {"Subject:", 400, TRUE, MULE_BLUE, MULE_DRED},
        {"From:", 400, TRUE, MULE_BLUE, MULE_PRP},
        {"To:", 400, FALSE, MULE_BLUE, MULE_ORG},
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
        {NULL, 0, FALSE, 0}};
    BOOL bHeader = TRUE;
    TCHAR szBuf[1024], szLastHeader[128];
    LPTSTR p, p1;
    FILE *fp;
    int bTitle = FALSE, c, i;
    LONG lfHeight;                              // �t�H���g�̍���
    LONG lfWeight;                              // �t�H���g�̃E�G�C�g
    BOOL lfItalic;                              // T: Italic
    BOOL lfUnderline;                           // T: Underline
    TCHAR lfFaceName[LF_FACESIZE];              // �t�H���g�t�F�C�X��

    DbgPrint(NULL, 'I', "���[���󎚏����J�n");

    // ���[����������������
    strcpy(g_MailBox.PrtInfo.szTitle, "�ق��ق�");
    strcpy(g_MailBox.szMessageID, "Message-Id: �킩��܂ւ�");
    strcpy(g_MailBox.szReference, " ");
    strcpy(g_MailBox.szFrom, "From: ������]");
    strcpy(g_MailBox.szDate, "Date: ���a40�N10��28��");

    szLastHeader[0] = '\0';

    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // �h�L�������g����J�n
        fclose(fp);
        return;
    }

    // �S�Ă̍s����������܂�
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab, fp,
                  g_MailBox.PrtInfo.nTab)) {
        lfHeight = GetPrtBasePoint();
        lfWeight = 400;
        lfItalic = lfUnderline = FALSE;
        strcpy(lfFaceName, FN_MSM);

        // ��s�̏ꍇ��MHS�I��
        if ('\r' == szBuf[0] || '\n' == szBuf[0]) {
            bHeader = FALSE;
            if (g_MailBox.PrtInfo.bColor) {     // �J���[����̏ꍇ
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
            }
        }

        // �w�b�_�̏ꍇ
        if (bHeader) {
            // �^�C�g���̏ꍇ�̓^�C�g���ݒ�
            if (0 == memicmp("Subject: ", szBuf, 9)) {
                strncpy(g_MailBox.PrtInfo.szTitle, szBuf + 9, 76);
                g_MailBox.PrtInfo.szTitle[
                    strlen(g_MailBox.PrtInfo.szTitle) - 1] = '\0';
            }

            // ���b�Z�[�WID�̏ꍇ�̓��b�Z�[�WID��ݒ�
            else if (0 == strnicmp("Message-Id:", szBuf, 11)) {
                strcpy(g_MailBox.szMessageID, szBuf);
                g_MailBox.szMessageID[
                    strlen(g_MailBox.szMessageID) - 1] = '\0';
            }

            // From�̏ꍇ
            else if (0 == strnicmp(SZ_FROM, szBuf, strlen(SZ_FROM))) {
                strcpy(g_MailBox.szFrom, szBuf);
                g_MailBox.szFrom[strlen(g_MailBox.szFrom) - 1] = '\0';
            }
            // Date�̏ꍇ
            else if (0 == strnicmp(SZ_DATE, szBuf, strlen(SZ_DATE))) {
                strcpy(g_MailBox.szDate, szBuf);
                g_MailBox.szDate[strlen(g_MailBox.szDate) - 1] = '\0';
            }

            // ���t�@�����X�̏ꍇ�̓��t�@�����X��ݒ�
            else if (0 == strnicmp(SZ_REFERENCES, szBuf,
                                   strlen(SZ_REFERENCES)) ||
                     0 == strnicmp(SZ_IN_REPLY_TO, szBuf,
                                   strlen(SZ_IN_REPLY_TO))) {
                strcpy(g_MailBox.szReference, szBuf);
                g_MailBox.szReference[
                    strlen(g_MailBox.szReference) - 1] = '\0';
            }

            // �w�b�_�̈�
            strcpy(lfFaceName, FN_MSPG);
            p1 = strchr(szBuf, ' ');
            if ((p = strchr(szBuf, ':')) && (NULL == p1 ||
                                             (ULONG)p < (ULONG)p1)) {
                if (' ' == *(p + 1) || '\t' == *(p + 1)) {
                    p++;
                }
                c = *(p + 1);
                *(p + 1) = '\0';
                lfWeight = 700;
                TrimRight(strcpy(szLastHeader, szBuf));
            }
            else {
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
                        }
                        else {
                            crTxt = Header[i].crHead;
                        }
                        break;
                    }
                }
                SetTextColor(g_MailBox.hDC, crTxt);
            }
            if (!SetFontAndPrint(szBuf, lfFaceName, lfHeight, lfWeight,
                                 lfItalic, lfUnderline, FALSE, TRUE)) {
                goto ErrorExit;
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
                    }
                    else {
                        lfWeight = Header[i].nWeight;
                        lfUnderline = Header[i].bUnderLine;
                    }

                    if (!SetFontAndPrint(p, lfFaceName, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE, TRUE)) {
                        goto ErrorExit;
                    }
                    break;
                }
            }
            if (!Header[i].szHeader) {          // �n�C���C�g�w�b�_�ł͂Ȃ�����
                if (g_MailBox.PrtInfo.bColor) { // �J���[����̏ꍇ
                    SetTextColor(g_MailBox.hDC, MULE_GRAY);
                }
                lfWeight = 400;
                if (!SetFontAndPrint(p, lfFaceName, lfHeight, lfWeight,
                                     lfItalic, lfUnderline, FALSE, TRUE)) {
                    goto ErrorExit;
                }
            }
            continue;                           // ���̍s����������
        }

        // ���p�L�����܂܂�Ă���ꍇ
        if (p1 = _mbschr(szBuf, '>')) {
            p = _mbschr(szBuf, '<');
            if (NULL == p || p1 < p) {
                lfWeight = 700;
            }
        }

        if ((p = strstr(szBuf, "http:")) || (p = strstr(szBuf, "mailto:"))) {
            p = szBuf;
            while ((p1 = strstr(p, "http:")) ||
                   (p1 = strstr(p, "mailto:"))) {
                if (p != p1) {
                    c = *p1;
                    *p1 = '\0';
                    strcpy(lfFaceName, FN_MSM);
                    lfUnderline = FALSE;

                    if (!SetFontAndPrint(p, lfFaceName, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE, TRUE)) {
                        goto ErrorExit;
                    }
                    *p1 = c;
                }

                if ((p = strchr(p1, ' ')) || (p = strchr(p1, '\t')) ||
                    (p = strchr(p1, '\n'))) {
                    c = *p;
                    *p = '\0';
                    lfUnderline = TRUE;
                    strcpy(lfFaceName, FN_MSPG);

                    if (!SetFontAndPrint(p1, lfFaceName, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE, TRUE)) {
                        goto ErrorExit;
                    }
                    *p = c;
                }
                else {
                    lfUnderline = TRUE;
                    strcpy(lfFaceName, FN_MSPG);

                    if (!SetFontAndPrint(p1, lfFaceName, lfHeight, lfWeight,
                                         lfItalic, lfUnderline, FALSE, TRUE)) {
                        goto ErrorExit;
                    }
                    break;
                }
            }
            if (*p) {
                strcpy(lfFaceName, FN_MSM);
                lfUnderline = FALSE;

                if (!SetFontAndPrint(p, lfFaceName, lfHeight, lfWeight,
                                     lfItalic, lfUnderline, FALSE, TRUE)) {
                    goto ErrorExit;
                }
            }
        }
        else {
            if (!SetFontAndPrint(szBuf, lfFaceName, lfHeight, lfWeight,
                                 lfItalic, lfUnderline, FALSE, TRUE)) {
                goto ErrorExit;
            }
        }
    }

ErrorExit:
    EndDocument();
    fclose(fp);

    DbgPrint(NULL, 'I', "���[���󎚏����I��");
}
