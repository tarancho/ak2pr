/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-08 01:11:29 tfuruka1>
 *
 * 「ak2psのようなもの」のクライアントの共通処理部
 *
 * $Id: clientCommon.c,v 1.8 2001/12/07 18:24:29 tfuruka1 Exp $
 * $Log: clientCommon.c,v $
 * Revision 1.8  2001/12/07 18:24:29  tfuruka1
 * 用紙サイズの指定を出来るようにした。
 *
 * Revision 1.7  2001/10/12 14:07:34  tfuruka1
 * 最初に無条件にサーバを起動するようにした。引数無しでサーバを起動させる
 * 事を想定している。また、不明なオプションを指定した場合に、Usageメッセー
 * ジを出力するようにした。
 *
 * Revision 1.6  2001/10/01 13:19:42  tfuruka1
 * -oオプションを追加。用紙の向きを指定出来るようにした。
 *
 * Revision 1.5  2001/09/28 15:45:54  tfuruka1
 * 引数をデバッグ用にダンプしている場所で、メモリ破壊を起こしていた（おば
 * かな）バグを修正。
 *
 * Revision 1.4  2001/08/19 04:39:54  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
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
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include <windows.h>
#include <stdio.h>
#include "ak2pr.h"

static void Usage(LPTSTR arg)
{
    printf("Usage: %s [-o{p|l}] [-m{PLAIN|MAIL|PS_ACROBAT|PS_GHOST}]\n"
           "\t\t[-fフォントサイズ] [-tタブ幅] [-u段組数] [-Tタイトル]\n"
           "\t\t[-Jタイトル] [-s用紙サイズ] [-P] [ファイル名...]\n\n"
           "\t-o 用紙の向きを指定します。デフォルトはサーバの設定。\n"
           "\t\tp PORTRAIT\n"
           "\t\tl LANDSCAPE\n"
           "\t-m ファイルの種類を指定します。デフォルトはサーバの設定。\n"
           "\t\tPLAIN      プレーンテキストファイル\n"
           "\t\tMAIL       インターネットメール(SJIS)\n"
           "\t\tPS_ACROBAT PostScriptファイル(Acrobat Distillerで変換)\n"
           "\t\tPS_GHOST   PostScriptファイル(GhostScriptで印刷)\n"
           "\t-f フォントサイズを指定します。デフォルトはサーバの設定。\n"
           "\t-t タブ幅を指定します。デフォルトはサーバの設定。\n"
           "\t-u 段組数を指定します。デフォルトはサーバの設定。\n"
           "\t-T タイトルを指定します(-Jと同じ)。デフォルトはファイル名。\n"
           "\t-J タイトルを指定します(-Tと同じ)。デフォルトはファイル名。\n"
           "\t-s 用紙サイズを指定します。デフォルトはサーバの設定。\n"
           "\t-P 指定しても何もしません。\n"
           "\tファイル名 印刷するファイル名を指定します。\n"
           "\t\t指定しなかった場合は、標準入力から読み込みます。\n"
           "\t\t複数ファイル指定できます。\n--\n",
           GetLongBaseName(arg));

    printf("%s", GetPaperSizeUsageMessage());
}

int ak2prClientCommon(int __argc, char **_argv)
{
    double fFont = 0.0;
    int nTab = 0, nUp = 0, i;
    BYTE szTitle[256], *pszTitle = NULL;
    int nFtype = PT_TEXT;
    int nOrientation = 0;                       // 用紙の向きはデフォルト
    short dmPaperSize = 0;                      // 用紙サイズはデフォルト

    ExecutePrtServer();                         // サーバを起動する
    {                                           // For Debug
        TCHAR szBuf[4096];
        strcpy(szBuf, "DBG: ");
        for (i = 1; i < __argc; i++) {
            if (4095 < (strlen(szBuf) + strlen(*(__argv + i)) + 1)) {
                break;
            }
            sprintf(szBuf + strlen(szBuf), "argv[%d]=%s", i, *(__argv + i));
            if (*(__argv + i + 1)) {
                if (4095 > (strlen(szBuf) + 2)) {
                    strcat(szBuf, ", ");
                }
                else {
                    break;
                }
            }
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
        case 's':
            if (!(dmPaperSize = GetPaperSizeDevMode(*(__argv + i) + 2))) {
                printf("用紙サイズが不正です: %s\n%s", *(__argv + i) + 2,
                       GetPaperSizeUsageMessage());
                return 1;
            }
            continue;
        case 'o':
            switch (*(*(__argv + i) + 2)) {
            case 'p': case 'P':
                nOrientation = DMORIENT_PORTRAIT;
                break;
            case 'l': case 'L':
                nOrientation = DMORIENT_LANDSCAPE;
                break;
            }
            continue;
        case 'm':
            if (*(*(__argv + i) + 2)) {
                if (0 == stricmp("PLAIN", *(__argv + i) + 2)) {
                    nFtype = PT_TEXT;
                }
                else if (0 == stricmp("MAIL", *(__argv + i) + 2)) {
                    nFtype = PT_MAIL;
                }
                else if (0 == stricmp("PS_ACROBAT", *(__argv + i) + 2)) {
                    nFtype = PT_PS_ACROBAT;
                }
                else if (0 == stricmp("PS_GHOST", *(__argv + i) + 2)) {
                    nFtype = PT_PS_GHOST;
                }
                else {
                    nFtype = PT_TEXT;
                }
            }
            else {
                nFtype = PT_MAIL;
            }
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
            Usage(*__argv);
            return 1;
        }
    }

    if (i >= __argc) {                          // ファイル名指定なしの場合
        SendPrintFromStdin(NULL, pszTitle, nUp, nTab, fFont, nFtype,
                           nOrientation, dmPaperSize);
        return 0;
    }
    else if (i == (__argc - 1)) {               // ファイルが一つだけ指定された
        SendPrintFromFileCopy(NULL, pszTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize);
        return 0;
    }

    for (; i < __argc; i++) {
        strncpy(szTitle, GetLongBaseName(*(__argv + i)), 255);
        SendPrintFromFileCopy(NULL, szTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize);
    }
    return 0;
}
