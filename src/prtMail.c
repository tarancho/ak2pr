/* -*- mode: C++; coding: sjis; -*-
 * Time-stamp: <2005-05-08 22:02:25 tfuruka1>
 *
 * 「ak2psのようなもの」のメール印字処理
 *
 * $Id: prtMail.c,v 1.7 2005/05/08 13:02:29 tfuruka1 Exp $
 * $Log: prtMail.c,v $
 * Revision 1.7  2005/05/08 13:02:29  tfuruka1
 * X-Face関連の追加
 *
 * Revision 1.6  2004/08/21 05:35:46  tfuruka1
 * メール印刷で、ヘッダ部の二行目以降のフォントが正しくない問題を修正。
 *
 * Revision 1.5  2004/08/16 14:24:01  tfuruka1
 * ヘッダとボディの区切りをハイフォン8つでも認識するようにしました。
 *
 * Revision 1.4  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.3  2004/01/19 05:37:04  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.2  2003/03/01 09:05:25  tfuruka1
 * ●行番号の印刷に対応した
 * ●e-mail印刷時のカラー印刷を少しカラフルにした。
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
        LPTSTR szHeader;                        // ヘッダ文字列
        int nWeight;                            // ウエイト
        BOOL bUnderLine;                        // 下線
        COLORREF crHead;                        // カラー印刷時の色
        COLORREF crDetail;                      // カラー印刷時の色
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
    int nLine;                                  // 行番号
    LONG lfHeight;                              // フォントの高さ
    LONG lfWeight;                              // フォントのウエイト
    BOOL lfItalic;                              // T: Italic
    BOOL lfUnderline;                           // T: Underline
    COLORREF crLast = RGB(0, 0, 0);             // 最後に使用した色
    PLOGFONT pLogFont;

    DbgPrint(NULL, 'I', "メール印字処理開始");

    // メール情報を初期化する
    strcpy(g_MailBox.PrtInfo.szTitle, "ほげほげ");
    strcpy(g_MailBox.szMessageID, "Message-Id: わかりまへん");
    strcpy(g_MailBox.szReference, " ");
    strcpy(g_MailBox.szFrom, "From: 匿名希望");
    strcpy(g_MailBox.szDate, "Date: 不明");

    szLastHeader[0] = '\0';

    // X-FACE情報を先に取り出す
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
        // X-FACE情報は無し
        g_MailBox.xFaceInfo.lpData = NULL;
        g_MailBox.xFaceInfo.cbData = 0;
    }
    unlink(szXFace);                         // X-FACE一次ファイル削除

    // メールファイルを開く
    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // ドキュメント印刷開始
        fclose(fp);
        return;
    }

    // 全ての行を処理するまで
    nLine = 0;
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab, fp,
                  g_MailBox.PrtInfo.nTab)) {
        lfHeight = GetPrtBasePoint();
        lfWeight = 400;
        lfItalic = lfUnderline = FALSE;
        pLogFont = &g_MailBox.PrtInfo.lfOPPF;

        // 空行の場合は MHS 終了。但し, 電信八号の場合はハイフォン 8つ
        // でヘッダ部終了
        if (('\r' == szBuf[0] || '\n' == szBuf[0])
            || (0 == strncmp("--------", szBuf, 8))) {
            bHeader = FALSE;
            if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                crLast = MULE_BLACK;
            }
        }

        // ヘッダの場合
        if (bHeader) {
            // タイトルの場合はタイトル設定
            if (0 == memicmp("Subject: ", szBuf, 9)) {
                strncpy(g_MailBox.PrtInfo.szTitle, szBuf + 9, 76);
                g_MailBox.PrtInfo.szTitle[
                    strlen(g_MailBox.PrtInfo.szTitle) - 1] = '\0';
            } else if (0 == strnicmp("Message-Id:", szBuf, 11)) {
                // メッセージIDの場合はメッセージIDを設定
                strcpy(g_MailBox.szMessageID, szBuf);
                g_MailBox.szMessageID[
                    strlen(g_MailBox.szMessageID) - 1] = '\0';
            } else if (0 == strnicmp(SZ_FROM, szBuf, strlen(SZ_FROM))) {
                // Fromの場合
                strcpy(g_MailBox.szFrom, szBuf);
                g_MailBox.szFrom[strlen(g_MailBox.szFrom) - 1] = '\0';
            } else if (0 == strnicmp(SZ_DATE, szBuf, strlen(SZ_DATE))) {
                // Dateの場合
                strcpy(g_MailBox.szDate, szBuf);
                g_MailBox.szDate[strlen(g_MailBox.szDate) - 1] = '\0';
            } else if (0 == strnicmp(SZ_REFERENCES, szBuf,
                                     strlen(SZ_REFERENCES))
                       || 0 == strnicmp(SZ_IN_REPLY_TO, szBuf,
                                        strlen(SZ_IN_REPLY_TO))) {
                // リファレンスの場合はリファレンスを設定
                strcpy(g_MailBox.szReference, szBuf);
                g_MailBox.szReference[
                    strlen(g_MailBox.szReference) - 1] = '\0';
            }

            // ヘッダの印字
            p1 = strchr(szBuf, ' ');
            if ((p = strchr(szBuf, ':')) && (NULL == p1 ||
                                             (ULONG)p < (ULONG)p1)) {
                if (' ' == *(p + 1) || '\t' == *(p + 1)) {
                    p++;
                }
                // ヘッダタイトルを印刷する為に ： の次又は次の次(:の
                // 次が空白の場合)の文字を退避後、ストッパを埋め込む
                // (う～ん、今見ると解析が大変だ(^^ 2005/05/08 古川談
                c = *(p + 1);
                *(p + 1) = '\0';
                lfWeight = 700;
                TrimRight(strcpy(szLastHeader, szBuf));
            } else {
                p = szBuf;
                lfWeight = 400;
            }

            // 簡易ヘッダ印刷の場合
            if (g_MailBox.PrtInfo.bNoRcvHeader) {
                // Receiveヘッダの場合は印刷しない
                if (0 == stricmp(SZ_RECEIVED, szLastHeader)) {
                    continue;
                }

                // X-Mailer以外のX-ヘッダは印刷しない
                if (0 == memicmp("X-", szLastHeader, 2) &&
                    0 != stricmp(SZ_MAILER, szLastHeader)) {
                    continue;
                }
            }

            // ヘッダのタイトル印字
            if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                COLORREF crTxt = MULE_BLACK;
                for (i = 0; Header[i].szHeader; i++) {
                    if (0 == memicmp(Header[i].szHeader, szLastHeader,
                                     strlen(Header[i].szHeader))) {
                        if (' ' == szBuf[0]) {  // 継続ヘッダ
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

            // 行番号印刷の場合は行番号を印刷する
            if (g_MailBox.PrtInfo.bNum) {
                sprintf(szLine, "%4d: ", ++nLine);
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                if (!SetFontAndPrint(szLine, lfHeight, 800, FALSE, FALSE,
                                     FALSE, &g_MailBox.PrtInfo.lfOF)) {
                    goto ErrorExit;
                }
                if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                    // 元の色に戻す
                    SetTextColor(g_MailBox.hDC, crLast);
                    crLast = MULE_BLACK;
                }
            }

            // X-Faceが存在していて、Fromヘッダの場合は、行送りしておく
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
            // X-Faceが存在していて、Fromヘッダを印刷した後であれば、
            // XFACEを描画する
            if (0 == stricmp(szLastHeader, "From:")
                && g_MailBox.xFaceInfo.lpData) {
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
                DrawXFace(&g_MailBox.xFaceInfo);
            }

            // : が見つからなかった場合は次の行へ
            if (p == szBuf) {
                // リファレンスの継続
                if (0 == strnicmp(SZ_REFERENCES, szLastHeader,
                                  strlen(SZ_REFERENCES)) ||
                    0 == strnicmp(SZ_IN_REPLY_TO, szLastHeader,
                                  strlen(SZ_IN_REPLY_TO))) {
                    strcat(strcat(g_MailBox.szReference, " "),
                           TrimString(szBuf));
                }
                continue;
            }
            // ヘッダタイトルを印刷する為にNULL文字に潰されていた文字
            // を復元する。因みにこのコメントは、処理を追いきれなくなっ
            // て2005/05/08に追記していたりします。
            *(p + 1) = c;
            p++;
            // ハイライトヘッダが含まれているかを検索
            for (i = 0; Header[i].szHeader; i++) {
                if (0 == memicmp(Header[i].szHeader, szLastHeader,
                                strlen(Header[i].szHeader))) {
                    // 必要ならアンダーラインを付加してヘッダの内容を表示
                    if (g_MailBox.PrtInfo.bColor) {// カラー印刷の場合
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

                    // XFace印刷後であれば、一行送る
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
            if (!Header[i].szHeader) {          // ハイライトヘッダではなかった
                if (g_MailBox.PrtInfo.bColor) { // カラー印刷の場合
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
            continue;                           // 次の行を処理する
        }

        /* --------------------
         * 以下はボディ部の印刷
         * -------------------- */

        // 行番号印刷の場合は行番号を印刷する
        if (g_MailBox.PrtInfo.bNum) {
            sprintf(szLine, "%4d: ", ++nLine);
            SetTextColor(g_MailBox.hDC, MULE_BLACK);
            if (!SetFontAndPrint(szLine, lfHeight, 800,
                                 FALSE, FALSE, FALSE,
                                 &g_MailBox.PrtInfo.lfOF)) {
                goto ErrorExit;
            }
            if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                // 元の色に戻す
                SetTextColor(g_MailBox.hDC, crLast);
                crLast = MULE_BLACK;
            }
        }

        // 引用記号が含まれている場合
        if (p1 = _mbschr(szBuf, '>')) {
            p = _mbschr(szBuf, '<');
            if (NULL == p || p1 < p) {
                lfWeight = 700;
                if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                    // 引用記号の数を数える
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
            // 通常のボディ部
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

    DbgPrint(NULL, 'I', "メール印字処理終了");
}
