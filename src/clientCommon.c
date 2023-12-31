/* -*- mode: c++; coding: sjis; -*-
 * Time-stamp: <2005-05-08 21:58:13 tfuruka1>
 * $Id: clientCommon.c,v 1.18 2005/05/08 12:58:59 tfuruka1 Exp $
 * $Name:  $
 *
 * 「ak2psのようなもの」のクライアントの共通処理部
 *
 * 備忘録
 *
 *   オプションが増えた場合は、このファイルと、
 *   dllmain.c(SendPrintFromStdin 等の関数の引数が変更になる可能性が大
 *   なので) の修正が必要になります。さらに、SendPrintFromStdin等の関
 *   数の引数が変更になった事により、wndproc.c のその関数を呼び出して
 *   いる箇所の変更が必要になります。
 *
 * $Log: clientCommon.c,v $
 * Revision 1.18  2005/05/08 12:58:59  tfuruka1
 * 関数の宣言部分の修正のみです。
 *
 * Revision 1.17  2004/12/23 13:14:24  tfuruka1
 * 折り返し動作をコマンド引数に追加した事と、それに共なう修正。
 *
 * Revision 1.16  2004/06/18 00:57:43  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.15  2004/01/19 05:25:58  tfuruka1
 * [/D:PRN]オプションを固定で無視するようにしました。/をオプションスイッ
 * チとして扱うようにしていたが, その方法だと, コマンドラインから使用した
 * ときに, ルート直下の D というファイルを扱えなくなってしまう。/はディレ
 * クトリセパレータとして使用可能な仕様としている為。
 *
 * Revision 1.14  2004/01/12 09:54:42  tfuruka1
 * ・ 短辺綴じと長辺綴じのコマンドオプションを追加しました。
 *
 * Revision 1.13  2004/01/11 11:28:51  tfuruka1
 * 古いコメントが残っていたのを修正しました。コメントの修正のみです。
 *
 * Revision 1.12  2004/01/11 11:24:43  tfuruka1
 * オプションスイッチとして / を認めた（Emacsがつけてくる場合があるので）。
 * そして、/Dオプションを無視するようにした。
 *
 * Revision 1.11  2003/03/29 12:38:56  tfuruka1
 * ● SendPrintFromStdin関数の仕様追加による修正（標準入力の読み込みと、
 *    クリップボードの読み込みに対応した）
 *
 * Revision 1.10  2003/03/14 14:58:25  tfuruka1
 * ● PostScriptファイルの作業ファイルの名称を元ファイルのTitleフィールド
 *    から取得するようにしました。また、Distillerで処理するファイルの拡張
 *    子を.PSにしました。
 * ● サーバの起動オプション -S を追加しました。サーバを印刷停止状態で起
 *    動する事が可能になりました。
 *
 * Revision 1.9  2003/02/25 15:27:27  tfuruka1
 * 行番号印刷のオプションを追加した。
 *
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
           "\t\t[-Jタイトル] [-s用紙サイズ] [-n[-]] [-P] [-S] [-b{s|l}]\n"
           "\t\t[-l{m|s}]\n"
           "\t\t[ファイル名...]\n\n"
           "\t特に明記していない限り、オプションを省略した場合の動作は\n"
           "\tサーバの設定値を使用します。\n"
           "\t-o 用紙の向きを指定します。デフォルトはサーバの設定。\n"
           "\t\tp PORTRAIT\n"
           "\t\tl LANDSCAPE\n"
           "\t-m ファイルの種類を指定します。デフォルトはプレーンテキスト。\n"
           "\t\tPLAIN      プレーンテキストファイル\n"
           "\t\tMAIL       インターネットメール(SJIS)\n"
           "\t\tPS_ACROBAT PostScriptファイル(Acrobat Distillerで変換)\n"
           "\t\tPS_GHOST   PostScriptファイル(GhostScriptで印刷)\n"
           "\t-f フォントサイズを指定します。\n"
           "\t-t タブ幅を指定します。\n"
           "\t-u 段組数を指定します。\n"
           "\t-T タイトルを指定します(-Jと同じ)。デフォルトはファイル名。\n"
           "\t-J タイトルを指定します(-Tと同じ)。デフォルトはファイル名。\n"
           "\t-s 用紙サイズを指定します。\n"
           "\t-n[-] 行番号を出力します。\n"
           "\t      -n-を指定した場合は行番号を出力しない。\n"
           "\t-P 指定しても何もしません。\n"
           "\t-S サーバを起動する時に印刷停止状態で起動します。\n"
           "\t   サーバが既に起動している場合は意味を持ちません。\n"
           "\t-b 長辺綴じ・短辺綴じを指定します。\n"
           "\t\t s 短辺綴じ\n"
           "\t\t l 長辺綴じ\n"
           "\t-l 文字が右側に食み出した時の動作を指定します。\n"
           "\t\t m マルチライン(右側に食み出した文字は折り返して印刷)\n"
           "\t\t s シングルライン(右側に食み出した文字は印刷しません)\n"
           "\tファイル名 印刷するファイル名を指定します。\n"
           "\t\t指定しなかった場合は、標準入力から読み込みます。\n"
           "\t\t複数ファイル指定できます。\n--\n",
           GetLongBaseName(arg));

    printf("%s", GetPaperSizeUsageMessage());
}

int
ak2prClientCommon(int __argc, char **_argv)
{
    TCHAR szBuf[4096];
    TCHAR szSVOption[128];                      // サーバの起動オプション
    double fFont = 0.0;
    int nTab = 0, nUp = 0, i;
    BYTE szTitle[256], *pszTitle = NULL;
    int nFtype = PT_TEXT;
    int nOrientation = 0;                       // 用紙の向きはデフォルト
    short dmPaperSize = 0;                      // 用紙サイズはデフォルト
    int bNum = -1;                              // 行番号出力のデフォルト
    // ↑booleanではないので注意：最初はbooleanにしていたのだが・・・
    int nBinding = -1;                          // 短辺・長辺綴じはデフォルト
    int nSingleLine = -1;                       // 折り返しはデフォルト

    szSVOption[0] = '\0';
    strcpy(szBuf, "DBG: ");
    for (i = 1; i < __argc; i++) {
        if (4095 < (strlen(szBuf) + strlen(*(__argv + i)) + 1)) {
            break;
        }

        // サーバ起動オプションの場合
        if (0 == strcmp(*(__argv + i), "-S")) {
            strcat(szSVOption, " -S");
        }

        sprintf(szBuf + strlen(szBuf), "argv[%d]=%s", i, *(__argv + i));
        if (*(__argv + i + 1)) {
            if (4095 > (strlen(szBuf) + 2)) {
                strcat(szBuf, ", ");
            } else {
                break;
            }
        }
    }
    Syslog(FALSE, "--<<< COMMAND START >>>--");
    Syslog(FALSE, "%s", szBuf);

    ExecutePrtServer((LPCTSTR)szSVOption);      // サーバを起動する

    for (i = 1; i < __argc; i++) {
        /* (page headers are not supported) を無視する emacs の
           print-buffer対策 */
        if ('(' == **(__argv + i)) {
            continue;
        }
        // EmacsからPostScriptのファイルを印刷するときに[/D:PRN] とい
        // うオプションが追加されてくるが、このオプションは、オプショ
        // ンスイッチが'-'ではなく'/'なので、ややこしい。ディレクトリ
        // セパレータと区別が付かないので、個別でチェックする。
        if (strcmpi("/D:PRN", *(__argv + i)) == 0) {
            Syslog(FALSE, "%s は無視します", *(__argv + i));
            continue;
        }
        if ('-' != **(__argv +i)) {
            break;
        }
        switch (*(*(__argv + i) + 1)) {
        case 'P':                               // このパラメータは無視
            // Emacsが付与するオプションだがデバイスオプションは無視す
            // る。
            Syslog(FALSE, "%s は無視します", *(__argv + i));
            continue;
        case 'S':                               // サーバ起動オプションは無視
            continue;
        case 'n':
            switch (*(*(__argv + i) + 2)) {
            case '\0':
            case '+':
                bNum = 1;
                break;
            case '-':
                bNum = 0;
                break;
            default:
                printf("-nオプションのパラメータが不正です: %s\n",
                       *(__argv + i));
                return 1;
            }
            continue;
        case 'l':
            switch (*(*(__argv + i) + 2)) {
            case 'm': case 'M':
                nSingleLine = 0;
                break;
            case 's': case 'S':
                nSingleLine = 1;
                break;
            case '\0':
            default:
                printf("-lオプションのパラメータが不正です: %s\n",
                       *(__argv + i));
                return 1;
            }
            continue;
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
        case 'b':
            switch (*(*(__argv + i) + 2)) {
            case 's': case 'S':                 // 短辺綴じ
                nBinding = 1;
                break;
            case 'l': case 'L':                 // 長辺綴じ
                nBinding = 0;
                break;
            }
            continue;
        case 'm':
            if (*(*(__argv + i) + 2)) {
                if (0 == stricmp("PLAIN", *(__argv + i) + 2)) {
                    nFtype = PT_TEXT;
                } else if (0 == stricmp("MAIL", *(__argv + i) + 2)) {
                    nFtype = PT_MAIL;
                } else if (0 == stricmp("PS_ACROBAT", *(__argv + i) + 2)) {
                    nFtype = PT_PS_ACROBAT;
                } else if (0 == stricmp("PS_GHOST", *(__argv + i) + 2)) {
                    nFtype = PT_PS_GHOST;
                } else {
                    nFtype = PT_TEXT;
                }
            } else {
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
            } else if (*(__argv+ i + 1)) {
                pszTitle = strncpy(szTitle, *(__argv + i + 1), 128);
                i++;
            }
            continue;
        default:
            Syslog(TRUE, "-ERROR: Invalid Argument (%s)", *(__argv + i));
            Usage(*__argv);
            return 1;
        }
    }

    if (i >= __argc) {                     // ファイル名指定なしの場合
        SendPrintFromStdin(FALSE, NULL, pszTitle, nUp, nTab, fFont, nFtype,
                           nOrientation, dmPaperSize, bNum, nBinding,
                           nSingleLine);
        return 0;
    } else if (i == (__argc - 1)) {    // ファイルが一つだけ指定された
        // PostScriptファイルの場合は、タイトルをファイルから得る
        if ((PT_PS_ACROBAT == nFtype) || (PT_PS_GHOST == nFtype)) {
            (LPCTSTR)pszTitle
                = GetPSTitle((LPCTSTR)*(__argv + i), szTitle, 128);
            if (NULL == pszTitle) {
                Usage(*__argv);
                return 1;
            }
        }
        SendPrintFromFileCopy(NULL, pszTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize,
                              bNum, nBinding, nSingleLine);
        return 0;
    }

    for (; i < __argc; i++) {
        strncpy(szTitle, GetLongBaseName(*(__argv + i)), 255);
        // PostScriptファイルの場合は、タイトルをファイルから得る
        if ((PT_PS_ACROBAT == nFtype) || (PT_PS_GHOST == nFtype)) {
            (LPCTSTR)pszTitle
                = GetPSTitle((LPCTSTR)*(__argv + i), szTitle, 128);
            if (NULL == pszTitle) {
                continue;
            }
        }
        SendPrintFromFileCopy(NULL, szTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize,
                              bNum, nBinding, nSingleLine);
    }
    return 0;
}
