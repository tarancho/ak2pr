/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-01-18 19:44:56 tfuruka1>
 *
 * 「ak2psのようなもの」のテキスト印刷処理
 *
 * $Id: prtText.c,v 1.3 2004/01/19 05:37:29 tfuruka1 Exp $
 * $Log: prtText.c,v $
 * Revision 1.3  2004/01/19 05:37:29  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.2  2003/02/25 15:29:43  tfuruka1
 * 行番号出力の制御追加による修正。
 *
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

    DbgPrint(NULL, 'I', "テキスト印字処理開始");

    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // ドキュメント印刷開始
        fclose(fp);
        return;
    }
    lfHeight = GetPrtBasePoint();
    nLine = 0;
    // 全ての行を処理するまで
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab,
                  fp, g_MailBox.PrtInfo.nTab)) {
        // 行番号印刷の場合は行番号を印刷する
        if (g_MailBox.PrtInfo.bNum) {
            sprintf(szLine, "%4d: ", ++nLine);
            if (!SetFontAndPrint(szLine, lfHeight, 800,
                                 FALSE, FALSE, FALSE,
                                 &g_MailBox.PrtInfo.lfOF)) {
                goto ErrorExit;
            }
        }
        if (!SetFontAndPrint(szBuf, lfHeight, 400,
                             FALSE, FALSE, FALSE, &g_MailBox.PrtInfo.lfTHF)) {
            goto ErrorExit;
        }
    }
ErrorExit:
    EndDocument();
    fclose(fp);
    DbgPrint(NULL, 'I', "テキスト印字処理終了");
}
