/* -*- C++ -*- Time-stamp: <99/05/18 16:58:15 tfuruka1> 
 *
 * MIME�̃f�R�[�h�֌W�̏������s���B�A���낭������RFC��ǂ�ł��Ȃ��̂ŁA
 * �������������ۏ؂͂Ȃ��B */
#include <windows.h>
#include <string.h>
/* ------------------------------------------------------------------
 * uCode�Ŏw�肳�ꂽEUC�R�[�h��SHIFT-JIS�R�[�h�֕ϊ�����B����JIS�R�[
 * �h����SHIFT-JIS�֕ϊ����鎖���\�ł���B���̂Ȃ��EUC��JIS�R�[�h��
 * 1�o�C�g�ڂ�2�o�C�g�ڂ̍ŏ�ʃr�b�g�𗧂ĂĂ��邾���Ȃ̂ŁA������
 * �r�b�g�𗎂Ƃ���JIS�R�[�h�ɂȂ��Ă��܂�����ł���B���̕ϊ��֐��ł�
 * ��ɂ����̃r�b�g�𗎂Ƃ��Ă��邽�߁AEUC�ł�JIS�ł������o����B�A
 * �����Ɍ������p�J�^�J�i�͕ϊ��o���Ȃ��B
 * *-----------------------------------------------------------------*/
static SHORT
EUCToSjis(USHORT uCode)
{
    USHORT uByte1, uByte2;

    uCode &= 0x7f7f;                            // JIS�ł�EUC�ł����v

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
 * Base64�̈ꕶ���ɑΉ�����R�[�h��ԋp����BBase64�̕�����64�i���ł�
 * ��A���ꂼ��̕������ȉ��̃R�[�h�ɑΉ����Ă���B
 *
 *          �_��  0 1 2 3 4 5 6 7
 *          ��__________________
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
 * lpszInOut�Ŏw�肳�ꂽBase64�������Decode����BBase64��3������4�o�C
 * �g�Ƃ��ĕ\�����Ă���B�]���āA�K�������񒷂�4�̔{���ɂȂ�BEncode��
 * ������6�̔{���ɂȂ�Ȃ������ꍇ�� '=' ��t������4�̔{���ɂȂ�悤��
 * �Ȃ��Ă���B
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
            szFrom++;                           // �����̕����͖���
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
 * ISO-2022-JP����Ȃ镶�����SHIFT-JIS������֕ϊ�����BUS-ASCII����
 * �܂�Ă��Ă������\�ł���B���Ȃ݂ɑ��Ɍ������p�J�^�J�i��
 * ISO-2022-JP�ɂ͑��݂��Ȃ��̂ŁA�ϊ��͍s���Ă��Ȃ��B
 * *-----------------------------------------------------------------*/
PUCHAR
Iso2022jpToSjis(
    PUCHAR szInOut,                             // ���o�̓o�b�t�@
    int cESC                                    // �G�X�P�[�v���� �ʏ��'B'
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
            if (0 == memcmp(szKO, szFrom, 2)) { // KO�̍Ō�̕����͌��Ȃ�
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
 * MHS�w�b�_����MIME��SHIFT-JIS�֕ϊ�����B�ϊ��̃A���S���Y���͈ȉ���
 * �ʂ�ł���B�Ⴆ��
 *
 *     Subject: =?ISO-2022-JP?B?GyRCNEE7ehsoSg==?=
 *              ^ ^           ^ <--- Base64 --->^
 *              | |           |                 |
 *              | |           |                 +- END
 *              | |           +------------------- ESC CHAR
 *              | +------------------------------- CODE
 *              +--------------------------------- START
 *
 * �Ƃ��������񂪑��݂����ꍇ�AMIME�̊J�n������ł��� '=?' ���������A
 * ���݂����ꍇ�͏I��������ł��� '?='���������A�ʒu���o���Ēu���B����
 * �āA�����Z�b�g��ISO-2022-JP or iso-2022-jp�ł��鎖���m�F����B����
 * �ȊO�̕����Z�b�g�̏ꍇ�͕ϊ����s��Ȃ��B���ɃG�X�P�[�u�����𒲂ׂ�B
 * ����Base64�̕�����Base64�Ńf�R�[�h���āAISO-2022-JP��SHIFT-JIS�֕�
 * ������B���̎���US-ASCII��ISO-2022-JP�̃G�X�P�[�v�Z�p���[�^��ESC
 * CHAR���g�p����B
 * *------------------------------------------------------------------*/
#define MSTARTL "=?ISO-2022-JP?"
#define MSTARTS "=?iso-2022-jp?"
#define MEND    "?="
PUCHAR
DecodeMHS(PUCHAR lpszInOut)
{
    PUCHAR lpMStart, lpMEnd, p;
    int cESC;

    // �ŏ��Ɍp���s�𔻒f���āA�s�̘A�����K�v�ȏꍇ�͌�������B
    p = lpszInOut;
    while ((lpMStart = strstr(p, "\r\n")) ||
           (lpMStart = strstr(p, "\n"))) {
        p = lpMStart;
        p += ('\r' == *lpMStart ? 2 : 1);
        if (' ' != *p && '\t' != *p) {
            continue;                           // �p���s�w���ł͂Ȃ��B
        }
        for (; ' ' == *p || '\t' == *p; p++);   // �擪�̃X�y�[�X��ǂݔ�΂�
        if (0 == strncmp("=?", p, 2)) {         // �p���s�w��
            strcpy(lpMStart, p);                // �A��
        }
    }

    p = lpszInOut;
    while ((lpMStart = strstr(p, MSTARTL)) ||
           (lpMStart = strstr(p, MSTARTS))) {
        cESC = *(lpMStart + strlen(MSTARTS));
        p = lpMStart + strlen(MSTARTS) + 2;
        if (!(lpMEnd = strstr(p, MEND))) {
            continue;                           // MIME�ᔽ
        }
        *lpMEnd = '\0';
        lpMEnd += strlen(MEND);

        Iso2022jpToSjis(DecodeBase64(p), cESC);
        strcpy(lpMStart, p);
        strcat(lpMStart, lpMEnd);
        p = lpMStart;
    }
    // �{���͋K��ᔽ�ł����A��JIS��MHS�w�b�_���ɏ����Ă���ꍇ�ɂ�
    // �Ă��Ή�����B
    Iso2022jpToSjis(lpszInOut, 'B');

    return lpszInOut;
}

#if 0 // ��1�ɂ���ƒP�̂œ��삷��B
/* -------------------------------------------------------------------
 * �ȉ��̓f�o�b�O�p�̃��C���֐��ł���B
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
