/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:41:42 tfuruka1>
 *
 * 「ak2psのようなもの」の文字列操作関連
 *
 * $Id: string.c,v 1.2 2004/06/18 00:57:56 tfuruka1 Exp $
 * $Log: string.c,v $
 * Revision 1.2  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.1  2001/02/05 17:45:20  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

/* -------------------------------------------------------------------
 * 文字列の右側のスペース文字を取り除く
 * *-----------------------------------------------------------------*/
LPTSTR TrimRight(LPTSTR lpszStr)
{
    size_t len = strlen(lpszStr);

    for (--len; len; len--) {
        if ('\r' == *(lpszStr + len)
            || '\n' == *(lpszStr + len)
            || '\t' == *(lpszStr + len)
            || '\f' == *(lpszStr + len)
            || ' ' == *(lpszStr + len)) {
            *(lpszStr + len) = '\0';
        }
        else {
            break;
        }
    }
    return lpszStr;
}

/* -------------------------------------------------------------------
 * 文字列の左側のスペース文字を取り除く
 * *-----------------------------------------------------------------*/
LPTSTR TrimLeft(LPTSTR lpszStr)
{
    LPTSTR pFrom = lpszStr, pTo = lpszStr;
    
    for (; *pFrom; pFrom++) {
        if ('\r' != *pFrom
            && '\n' != *pFrom
            && '\t' != *pFrom
            && '\f' != *pFrom
            && ' ' != *pFrom) {
            break;
        }
    }

    if (pFrom == pTo) {                         // 余分な文字は無い
        return lpszStr;
    }
    for (; *pFrom; pFrom++, pTo++) {
        *pTo = *pFrom;
    }
    *pTo = '\0';
    return lpszStr;
}

/* -------------------------------------------------------------------
 * 文字列の左右のスペース文字を取り除く
 * *-----------------------------------------------------------------*/
LPTSTR TrimString(LPSTR lpszStr)
{
    return TrimLeft(TrimRight(lpszStr));
}

/* -------------------------------------------------------------------
 * fgetsと同様の処理を行うが、タブをスペース文字列へ展開する。但し、タ
 * ブの展開を途中で止める事は無いので、最大文字列長はタブストップを考
 * 慮する必要がある。
 * *-----------------------------------------------------------------*/
LPTSTR
ufgets(
    LPTSTR szBuf,                               // 格納バッファ
    int cbMax,                                  // 最大文字列長
    FILE *fp,                                   // ファイルポインタ
    int nTab                                    // タブストップ
    )
{
    int i, j, c;

    for (i = 0; i < cbMax - 1; i++) {
        if (EOF == (c = getc(fp))) break;
        if ('\t' == c) {
            for (j = 0; j < (nTab - (i % nTab)); j++) {
                *szBuf++ = ' ';
            }
            i += (nTab - (i % nTab));
            i--;
            continue;
        }
        *szBuf++ = c;
        if ('\n' == c) {
            i++;
            break;
        }
    }
    *szBuf = '\0';

    if (!i) {
        return NULL;
    }
    return szBuf;
}
