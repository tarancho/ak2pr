/* -*- mode: c; coding: sjis-dos; -*-
 * Time-stamp: <2001-08-19 01:42:00 tfuruka1>
 *
 * 「ak2psのようなもの」のクライアントの共通処理部
 *
 * $Id: clientCommon.c,v 1.3 2001/08/18 16:44:17 tfuruka1 Exp $
 * $Log: clientCommon.c,v $
 * Revision 1.3  2001/08/18 16:44:17  tfuruka1
 * ●SendPrintFromFileが廃止され、SendPrintFromFileCopyに統合された為、
 *   SendPrintFromFileCopy関数を使用するように修正(複数ファイル指定された
 *   時)。
 *
 * Revision 1.2  2001/02/05 17:49:24  tfuruka1
 * 改行コードの修正
 *
 * Revision 1.1  2001/02/05 17:47:43  tfuruka1
 * Initial revision
 *
 */

#include <windows.h>
#include <stdio.h>
#include "ak2pr.h"

int ak2prClientCommon(int __argc, char **_argv)
{
    double fFont = 0.0;
    int nTab = 0, nUp = 0, i;
    BYTE szTitle[256], *pszTitle = NULL;
    BOOL bMail = FALSE;
    {
        TCHAR szBuf[4096];
        szBuf[0] = '\0';
        for (i = 1; i < __argc; i++) {
            sprintf(szBuf + strlen(szBuf), "argv[%d]=%s, ", i, *(__argv + i));
        }
        Syslog("%s", szBuf);
    }

    for (i = 1; i < __argc; i++) {
        /* (page headers are not supported) を無視する emacs の 
           print-buffer対策 */
        if ('(' == **(__argv + i)) {
            continue;
        }
        if ('-' != **(__argv +i)) {
            break;
        }
        switch (*(*(__argv + i) + 1)) {
        case 'm':
            bMail = TRUE;
            continue;
        case 'f':
            if (*(*(__argv + i) + 2)) {
                fFont = strtod(*(__argv + i) + 2, NULL);
            }
            continue;
        case 't':
            if (*(*(__argv + i) + 2)) {
                nTab = atoi(*(__argv + i) + 2);
            }
            continue;
        case 'u':
            if (*(*(__argv + i) + 2)) {
                nUp = atoi(*(__argv + i) + 2);
            }
            continue;
        case 'T': case 'J':
            if (*(*(__argv + i) + 2)) {
                pszTitle = strncpy(szTitle, *(__argv + i) + 2, 128);
            }
            else if (*(__argv+ i + 1)) {
                pszTitle = strncpy(szTitle, *(__argv + i + 1), 128);
                i++;
            }
            continue;
        case 'P':                               // このパラメータは無視
            continue;
        default:
            Syslog("-ERROR: Invalid Argument (%s)", *(__argv + i));
            return 1;
        }
    }

    if (i >= __argc) {                          // ファイル名指定なしの場合
        SendPrintFromStdin(NULL, pszTitle, nUp, nTab, fFont, bMail);
        return 0;
    }
    else if (i == (__argc - 1)) {               // ファイルが一つだけ指定された
        SendPrintFromFileCopy(NULL, pszTitle, *(__argv + i), nUp, nTab,
                              fFont, bMail);
        return 0;
    }

    for (; i < __argc; i++) {
        strncpy(szTitle, GetLongBaseName(*(__argv + i)), 255);
        SendPrintFromFileCopy(NULL, szTitle, *(__argv + i), nUp, nTab,
                              fFont, bMail);
    }
    return 0;
}
