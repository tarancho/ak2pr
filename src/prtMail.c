/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2001-02-06 02:46:01 tfuruka1>
 *
 * 「ak2psのようなもの」のメール印字処理
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
        LPTSTR szHeader;                        // ヘッダ文字列
        int nWeight;                            // ウエイト
        BOOL bUnderLine;                        // 下線
        COLORREF crHead;                        // カラー印刷時の色
        COLORREF crDetail;                      // カラー印刷時の色
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
    LONG lfHeight;                              // フォントの高さ
    LONG lfWeight;                              // フォントのウエイト
    BOOL lfItalic;                              // T: Italic
    BOOL lfUnderline;                           // T: Underline
    TCHAR lfFaceName[LF_FACESIZE];              // フォントフェイス名

    DbgPrint(NULL, 'I', "メール印字処理開始");

    // メール情報を初期化する
    strcpy(g_MailBox.PrtInfo.szTitle, "ほげほげ");
    strcpy(g_MailBox.szMessageID, "Message-Id: わかりまへん");
    strcpy(g_MailBox.szReference, " ");
    strcpy(g_MailBox.szFrom, "From: 匿名希望");
    strcpy(g_MailBox.szDate, "Date: 昭和40年10月28日");

    szLastHeader[0] = '\0';

    if (NULL == (fp = fopen(g_MailBox.PrtInfo.szFileName, "rt"))) {
        DbgPrint(NULL, 'E', "%s", _strerror(g_MailBox.PrtInfo.szFileName));
        return;
    }

    if (!BeginDocument()) {                     // ドキュメント印刷開始
        fclose(fp);
        return;
    }

    // 全ての行を処理するまで
    while (ufgets(szBuf, 1024 - g_MailBox.PrtInfo.nTab, fp,
                  g_MailBox.PrtInfo.nTab)) {
        lfHeight = GetPrtBasePoint();
        lfWeight = 400;
        lfItalic = lfUnderline = FALSE;
        strcpy(lfFaceName, FN_MSM);

        // 空行の場合はMHS終了
        if ('\r' == szBuf[0] || '\n' == szBuf[0]) {
            bHeader = FALSE;
            if (g_MailBox.PrtInfo.bColor) {     // カラー印刷の場合
                SetTextColor(g_MailBox.hDC, MULE_BLACK);
            }
        }

        // ヘッダの場合
        if (bHeader) {
            // タイトルの場合はタイトル設定
            if (0 == memicmp("Subject: ", szBuf, 9)) {
                strncpy(g_MailBox.PrtInfo.szTitle, szBuf + 9, 76);
                g_MailBox.PrtInfo.szTitle[
                    strlen(g_MailBox.PrtInfo.szTitle) - 1] = '\0';
            }

            // メッセージIDの場合はメッセージIDを設定
            else if (0 == strnicmp("Message-Id:", szBuf, 11)) {
                strcpy(g_MailBox.szMessageID, szBuf);
                g_MailBox.szMessageID[
                    strlen(g_MailBox.szMessageID) - 1] = '\0';
            }

            // Fromの場合
            else if (0 == strnicmp(SZ_FROM, szBuf, strlen(SZ_FROM))) {
                strcpy(g_MailBox.szFrom, szBuf);
                g_MailBox.szFrom[strlen(g_MailBox.szFrom) - 1] = '\0';
            }
            // Dateの場合
            else if (0 == strnicmp(SZ_DATE, szBuf, strlen(SZ_DATE))) {
                strcpy(g_MailBox.szDate, szBuf);
                g_MailBox.szDate[strlen(g_MailBox.szDate) - 1] = '\0';
            }

            // リファレンスの場合はリファレンスを設定
            else if (0 == strnicmp(SZ_REFERENCES, szBuf,
                                   strlen(SZ_REFERENCES)) ||
                     0 == strnicmp(SZ_IN_REPLY_TO, szBuf,
                                   strlen(SZ_IN_REPLY_TO))) {
                strcpy(g_MailBox.szReference, szBuf);
                g_MailBox.szReference[
                    strlen(g_MailBox.szReference) - 1] = '\0';
            }

            // ヘッダの印字
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
            if (!Header[i].szHeader) {          // ハイライトヘッダではなかった
                if (g_MailBox.PrtInfo.bColor) { // カラー印刷の場合
                    SetTextColor(g_MailBox.hDC, MULE_GRAY);
                }
                lfWeight = 400;
                if (!SetFontAndPrint(p, lfFaceName, lfHeight, lfWeight,
                                     lfItalic, lfUnderline, FALSE, TRUE)) {
                    goto ErrorExit;
                }
            }
            continue;                           // 次の行を処理する
        }

        // 引用記号が含まれている場合
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

    DbgPrint(NULL, 'I', "メール印字処理終了");
}
