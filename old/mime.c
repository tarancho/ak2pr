/* -*- C++ -*- Time-stamp: <99/05/18 16:58:15 tfuruka1> 
 *
 * MIMEのデコード関係の処理を行う。但しろくすっぽRFCを読んでいないので、
 * 処理が正しい保証はない。 */
#include <windows.h>
#include <string.h>
/* ------------------------------------------------------------------
 * uCodeで指定されたEUCコードをSHIFT-JISコードへ変換する。実はJISコー
 * ドからSHIFT-JISへ変換する事も可能である。何故ならばEUCはJISコードの
 * 1バイト目と2バイト目の最上位ビットを立てているだけなので、これらの
 * ビットを落とすとJISコードになってしまうからである。この変換関数では
 * 先にこれらのビットを落としているため、EUCでもJISでも処理出来る。但
 * し俗に言う半角カタカナは変換出来ない。
 * *-----------------------------------------------------------------*/
static SHORT
EUCToSjis(USHORT uCode)
{
    USHORT uByte1, uByte2;

    uCode &= 0x7f7f;                            // JISでもEUCでも大丈夫

    uByte1 = (uCode >> 8) & 0xff;
    uByte2 = uCode & 0xff;

    if (uByte1 & 1) 
        uByte2 += (0x40 - 0x21);
    else
        uByte2 += (0x9e - 0x21);

    if (0x7f <= uByte2) {
        uByte2++;
    }
    uByte1 -= 0x21;
    uByte1 >>= 1;
    uByte1 += 0x81;
    if (0xa0 <= uByte1) {
        uByte1 += 0x40;
    }
    return uByte1 * 256 + uByte2;
}

/* ------------------------------------------------------------------
 * Base64の一文字に対応するコードを返却する。Base64の文字は64進数であ
 * り、それぞれの文字が以下のコードに対応している。
 *
 *          ＼下  0 1 2 3 4 5 6 7
 *          上＼_________________
 *          000 | A B C D E F G H
 *          010 | I J K L M N O P
 *          020 | Q R S T U V W X
 *          030 | Y Z a b c d e f
 *          040 | g h i j k l m n
 *          050 | o p q r s t u v
 *          060 | w x y z 0 1 2 3
 *          070 | 4 5 6 7 8 9 + /
 * *-----------------------------------------------------------------*/
static int
GetBase64Code(int c)
{
    char cBase64[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "abcdefghijklmnopqrstuvwxyz"
                      "0123456789+/"};
    int i;
    for (i = 0; i < 64; i++) {
        if (c == cBase64[i]) {
            return i;
        }
    }
    return 0;
}
/* ------------------------------------------------------------------
 * lpszInOutで指定されたBase64文字列をDecodeする。Base64は3文字を4バイ
 * トとして表現している。従って、必ず文字列長は4の倍数になる。Encodeし
 * た結果6の倍数にならなかった場合は '=' を付加して4の倍数になるように
 * なっている。
 *
 * +---------------+---------------+---------------+
 * |7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|
 * +---------------+---------------+---------------+
 *            |           |           |           |
 *            |           |           |           +---------------+
 *            +---+       +-------+   +-----------+               |
 *                |               |               |               |
 * +---------------+---------------+---------------+---------------+
 * |* * 5 4 3 2 1 0|* * 5 4 3 2 1 0|* * 5 4 3 2 1 0|* * 5 4 3 2 1 0|
 * +---------------+---------------+---------------+---------------+
 *
 * *-----------------------------------------------------------------*/
static PUCHAR
DecodeBase64(PUCHAR lpszInOut)
{
    PUCHAR szFrom, szTo;
    ULONG uDecode = 0;
    int nOffset = 0;

    szFrom = szTo = lpszInOut;

    while (*szFrom) {
        if ('\r' == *szFrom || '\n' == *szFrom || ' ' == *szFrom) {
            szFrom++;                           // これらの文字は無視
            continue;
        }

        uDecode = (uDecode << 6) + GetBase64Code(*szFrom);
        nOffset++;
        szFrom++;
        if (3 < nOffset) {
            *szTo++ = (UCHAR)((uDecode >> 16) & 0xff);
            *szTo++ = (UCHAR)((uDecode >> 8) & 0xff);
            *szTo++ = (UCHAR)((uDecode >> 0) & 0xff);
            nOffset = 0;
            uDecode = 0;
        }
    }
    *szTo = '\0';
    return lpszInOut;
}
/* ------------------------------------------------------------------
 * ISO-2022-JPからなる文字列をSHIFT-JIS文字列へ変換する。US-ASCIIが含
 * まれていても処理可能である。ちなみに俗に言う半角カタカナは
 * ISO-2022-JPには存在しないので、変換は行っていない。
 * *-----------------------------------------------------------------*/
PUCHAR
Iso2022jpToSjis(
    PUCHAR szInOut,                             // 入出力バッファ
    int cESC                                    // エスケープ文字 通常は'B'
    )
{
    BOOL bKanji = FALSE;
    PUCHAR szFrom, szTo;
    UCHAR szKI[] = "\033$x", szKO[] = "\033(x";
    USHORT uCode;

    szFrom = szTo = szInOut;
    *(szKI + 2) = cESC;
    while (*szFrom) {
        if ('\033' == *szFrom) {
            if (0 == memcmp(szKI, szFrom, 3)) {
                bKanji = TRUE;
                szFrom += 3;
                continue;
            }
            if (0 == memcmp(szKO, szFrom, 2)) { // KOの最後の文字は見ない
                bKanji = FALSE;
                szFrom += 3;
                continue;
            }
            *szTo++ = *szFrom++;
            continue;
        }
        if (!bKanji) {
            *szTo++ = *szFrom++;
            continue;
        }
        uCode = *szFrom * 256 + *(szFrom + 1);
        uCode = EUCToSjis(uCode);
        *szTo = (uCode >> 8) & 0xff;
        *(szTo + 1) = uCode & 0xff;
        szTo += 2;
        szFrom += 2;
    }
    *szTo = '\0';
    return szInOut;
}
/* -------------------------------------------------------------------
 * MHSヘッダ内のMIMEをSHIFT-JISへ変換する。変換のアルゴリズムは以下の
 * 通りである。例えば
 *
 *     Subject: =?ISO-2022-JP?B?GyRCNEE7ehsoSg==?=
 *              ^ ^           ^ <--- Base64 --->^
 *              | |           |                 |
 *              | |           |                 +- END
 *              | |           +------------------- ESC CHAR
 *              | +------------------------------- CODE
 *              +--------------------------------- START
 *
 * という文字列が存在した場合、MIMEの開始文字列である '=?' を検索し、
 * 存在した場合は終了文字列である '?='を検索し、位置を覚えて置く。そし
 * て、文字セットがISO-2022-JP or iso-2022-jpである事を確認する。それ
 * 以外の文字セットの場合は変換を行わない。次にエスケーブ文字を調べる。
 * 次にBase64の部分をBase64でデコードして、ISO-2022-JPをSHIFT-JISへ変
 * 換する。この時にUS-ASCIIとISO-2022-JPのエスケープセパレータにESC
 * CHARを使用する。
 * *------------------------------------------------------------------*/
#define MSTARTL "=?ISO-2022-JP?"
#define MSTARTS "=?iso-2022-jp?"
#define MEND    "?="
PUCHAR
DecodeMHS(PUCHAR lpszInOut)
{
    PUCHAR lpMStart, lpMEnd, p;
    int cESC;

    // 最初に継続行を判断して、行の連結が必要な場合は結合する。
    p = lpszInOut;
    while ((lpMStart = strstr(p, "\r\n")) ||
           (lpMStart = strstr(p, "\n"))) {
        p = lpMStart;
        p += ('\r' == *lpMStart ? 2 : 1);
        if (' ' != *p && '\t' != *p) {
            continue;                           // 継続行指示ではない。
        }
        for (; ' ' == *p || '\t' == *p; p++);   // 先頭のスペースを読み飛ばす
        if (0 == strncmp("=?", p, 2)) {         // 継続行指示
            strcpy(lpMStart, p);                // 連結
        }
    }

    p = lpszInOut;
    while ((lpMStart = strstr(p, MSTARTL)) ||
           (lpMStart = strstr(p, MSTARTS))) {
        cESC = *(lpMStart + strlen(MSTARTS));
        p = lpMStart + strlen(MSTARTS) + 2;
        if (!(lpMEnd = strstr(p, MEND))) {
            continue;                           // MIME違反
        }
        *lpMEnd = '\0';
        lpMEnd += strlen(MEND);

        Iso2022jpToSjis(DecodeBase64(p), cESC);
        strcpy(lpMStart, p);
        strcat(lpMStart, lpMEnd);
        p = lpMStart;
    }
    // 本来は規約違反ですが、生JISをMHSヘッダ内に書いてある場合につい
    // ても対応する。
    Iso2022jpToSjis(lpszInOut, 'B');

    return lpszInOut;
}

#if 0 // ←1にすると単体で動作する。
/* -------------------------------------------------------------------
 * 以下はデバッグ用のメイン関数である。
 * *------------------------------------------------------------------*/
void main(void)
{
    char szBuf[1024] = "GyRCNEE7ehsoSg==";

    printf("IN >>%s\n", szBuf);
    printf("OUT>>%s\n", Iso2022jpToSjis(DecodeBase64(szBuf), 'B'));

    strcpy(szBuf, "=?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           " =?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           "  =?ISO-2022-JP?B?GyRCNDYkOBsoSg==?=123\n"
           "   =?ISO-2022-JP?B?GyRCNEE7ehsoSg==?=kanji\n"
           "    =?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           "     =?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           "\t\t\t=?ISO-2022-JP?B?GyRCNDYkOBsoSg==?=123\n"
           " =?ISO-2022-JP?B?GyRCNEE7ehsoSg==?=kanji\n"
           " =?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           " =?ISO-2022-JP?B?GyRCNDQ7dhsoSg==?=ABC\n"
           " =?ISO-2022-JP?B?GyRCNDYkOBsoSg==?=123\n"
           "     =?ISO-2022-JP?B?GyRCNEE7ehsoSg==?=kanji");
    printf("IN >>%s\n", szBuf);
    printf("OUT>>%s\n", DecodeMHS(szBuf));

    while (fgets(szBuf, 1024, stdin)) {
        printf("%s", DecodeMHS(szBuf));
    }
}
#endif
