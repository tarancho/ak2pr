/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:45:47 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃e�L�X�g�������
 *
 * $Id: prtText.c,v 1.1 2001/02/05 17:45:47 tfuruka1 Exp $
 * $Log: prtText.c,v $
 * Revision 1.1  2001/02/05 17:45:47  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

void PrintText(void)
{
    TCHAR szBuf[1024], szLine[16];
    FILE *fp;
    int nLine;
    UINT lfHeight;

    DbgPrint(NULL, 'I', "�e�L�X�g�󎚏����J�n");

    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // �h�L�������g����J�n
        fclose(fp);
        return;
    }
    lfHeight = GetPrtBasePoint();
    nLine = 0;
    // �S�Ă̍s����������܂�
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab,
                  fp, g_MailBox.PrtInfo.nTab)) {
        sprintf(szLine, "%4d: ", ++nLine);
        if (!SetFontAndPrint(szLine, FN_COU, lfHeight, 800,
                             FALSE, FALSE, FALSE, FALSE)) {
            goto ErrorExit;
        }
        if (!SetFontAndPrint(szBuf, FN_MSM, lfHeight, 400,
                             FALSE, FALSE, FALSE, TRUE)) {
            goto ErrorExit;
        }
    }
ErrorExit:
    EndDocument();
    fclose(fp);
    DbgPrint(NULL, 'I', "�e�L�X�g�󎚏����I��");
}
