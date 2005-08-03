/* -*- mode: c++; coding: sjis; -*-
 * Copyright(C) 2005 T.Furukawa
 * $Id: base64.c,v 1.1 2005/08/03 01:33:47 tfuruka1 Exp $
 * $Name:  $
 *
 * Base64�֘A
 *
 * RFC 2045 ���낭�ɓǂ�ł��Ȃ��̂ŁA���\�蔲���ł��B�������A�����悤
 * �Ȃ̂����������Ă���悤�ȋC������B
 * 
 * Table 1: The Base64 Alphabet
 * Value Encoding  Value Encoding  Value Encoding  Value Encoding
 *     0 A            17 R            34 i            51 z
 *     1 B            18 S            35 j            52 0
 *     2 C            19 T            36 k            53 1
 *     3 D            20 U            37 l            54 2
 *     4 E            21 V            38 m            55 3
 *     5 F            22 W            39 n            56 4
 *     6 G            23 X            40 o            57 5
 *     7 H            24 Y            41 p            58 6
 *     8 I            25 Z            42 q            59 7
 *     9 J            26 a            43 r            60 8
 *    10 K            27 b            44 s            61 9
 *    11 L            28 c            45 t            62 +
 *    12 M            29 d            46 u            63 /
 *    13 N            30 e            47 v
 *    14 O            31 f            48 w         (pad) =
 *    15 P            32 g            49 x
 *    16 Q            33 h            50 y
 *
 * �P�̃f�o�b�O�̕��@:
 * 
 *     cl /W3 /Zi /DDEBUG base64.c
 *
 *   �ŒP�̂ŃR���p�C���ł��܂��B�������ꂽbase64.exe�����������ŋN��
 *   ���邩�A
 *
 *       base64.exe <base64File> <outputFile>
 *
 *    �̂悤�ɁABase64�ŃG���R�[�h���ꂽ�t�@�C���ƁA�o�̓t�@�C�����w��
 *    ���܂��B
 *
 * $Log: base64.c,v $
 * Revision 1.1  2005/08/03 01:33:47  tfuruka1
 * Face�Ή��̑O�����Ƃ���Base64�̃f�R�[�h��ImagiMagic��convert.exe���w��
 * �ł���悤�ɑΉ��B
 *
 */

#include "base64.h"

#define BASE64_PAD_CHAR '='
#define IS_VALID(code)  ((64 > (code)) && (0 <= (code)))

static const char szBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "abcdefghijklmnopqrstuvwxyz"
                                       "0123456789+/";

static int
ToAlphabet(int code)
{
    return IS_VALID(code) ? szBase64Alphabet[code] : - 1;
}

static int
FromAlphabet(int code)
{
    int i;

    for (i = 0; i < 64; i++) {
        if (code == szBase64Alphabet[i]) {
            return i;
        }
    }
    return -1;
}

static char *
ToAlphabetGroup(const unsigned char *lpData, int cnt, char *lpszGroup)
{
    int i;
    int code;
    unsigned long data
        = *(lpData + 0) * 256 * 256
        + *(lpData + 1) * 256
        + *(lpData + 2);

    memset(lpszGroup, BASE64_PAD_CHAR, 4);
    *(lpszGroup + 4) = '\0';
    // 1Byte �Ȃ�,2char + ==�B2Byte �Ȃ�,3char + =�B3Byte �Ȃ�,4char 
    // �ɂȂ�̂�, ��X cnt+1�ƈ�v����
    for (i = 0; i <= (cnt % 4); i++) {
        code = (data >> (6 * (3 - i))) & 0x3f;
        *(lpszGroup + i) = (char)(ToAlphabet((int)code) & 0xff);
    }
    return lpszGroup;
}

static int                                      // �ϊ���̃o�C�g��
FromAlpabetGroup(
    char *lpAlphabet,                           // 4����
    unsigned char *lpData                       // 3byte
    )
{
    int i;
    int ret;
    unsigned long code;
    unsigned long data = 0;

    for (i = 0; i < 4; i++) {
        if (BASE64_PAD_CHAR == *(lpAlphabet + i)) {
            break;
        }
        code = FromAlphabet(*(lpAlphabet + i));
        data |= (code << (2 + (6 * (4 - i))));
    }
    ret = i - 1;

    for (i = 0; i < ret; i++) {
        *(lpData + i) = (unsigned char)((data >> (8 * (3 - i))) & 0xff);
    }

    return ret;
}

/*
 * Base64�ɃG���R�[�h���܂��B�G���R�[�h���ʊi�[��́A���f�[�^�� 4/3 �{
 * �̃G���A���K�v�ɂȂ�܂�(3�o�C�g��4�o�C�g�ɂȂ�܂�)�B�G���R�[�h��
 * �̃f�[�^���͕K����4�̔{���ɂȂ�܂��B�܂��A�Ō��null�X�g�b�v��t�^
 * ���܂��̂ŁA����Ɉ�o�C�g�]���ɕK�v�ɂȂ�܂��B
 */
char *
EncodeBase64(
    unsigned char *lpData,                      // �G���R�[�h�f�[�^
    size_t cbData,                              // �o�C�g��
    char *lpszEncode                         // �G���R�[�h����(������)
    )
{
    char *lpszSave = lpszEncode;
    size_t i;

    for (i = 0; i < cbData; i += 3, lpszEncode += 4) {
        ToAlphabetGroup(lpData + i,
                        ((cbData - i) > 3) ? 3 : (cbData - i),
                        lpszEncode);
    }
    *lpszEncode = '\0';
    return lpszSave;
}

/*
 * Base64�Ńf�R�[�h���܂��B�f�R�[�h����錳�̕����񒷂͕K��4�̔{���ł�
 * ��K�v������܂��B�f�R�[�h��̃o�C�g����ԋp���܂��B�f�R�[�h�O�̃G
 * ���A�ƃf�R�[�h��̃G���A�͓����A�h���X���w�肷�鎖���\�ł��B
 */
unsigned long
DecodeBase64(
    char *lpszData,                             // �K��4�̔{���ł��鎖
    unsigned char *lpData                       // �f�R�[�h����
    )
{
    char szAlphabet[8];
    unsigned long cbData = 0;
    int i = 0;
    int j;

    for (; *lpszData; lpszData++) {
        if ((' ' == *lpszData) || ('\t' == *lpszData)
            || ('\r' == *lpszData) || ('\n' == *lpszData)) {
            continue;
        }
        szAlphabet[i] = *lpszData;
        i++;
        if (i >= 4) {
            szAlphabet[4] = '\0';
            j = FromAlpabetGroup(szAlphabet, lpData);
            cbData += j;
            lpData += j;
            i = 0;
        }
    }
    return cbData;
}

/*
 * Base64�ŃG���R�[�h���ꂽ�t�@�C�����f�R�[�h���A�t�@�C���ɏo�͂��܂��B
 * ���̓t�@�C���Əo�̓t�@�C���𓯂��ɂ��鎖�͂ł��܂���B
 */
unsigned long
DecodeBase64File(
    unsigned char *lpszBase64FileName,          // ���̓t�@�C����
    unsigned char *lpszOutFileName              // �o�̓t�@�C����
    )
{
    FILE *fpIn;
    FILE *fpOut;
    char szAlphabet[8];
    unsigned char szBuf[4];
    unsigned long cbData = 0;
    int i = 0;
    int j;
    int c;

    if (!(fpIn = fopen(lpszBase64FileName, "rb"))) {
        return -1;
    }
    if (!(fpOut = fopen(lpszOutFileName, "wb"))) {
        fclose(fpIn);
        return -2;
    }

    while (EOF != (c = getc(fpIn))) {
        if ((' ' == c) || ('\t' == c) || ('\r' == c) || ('\n' == c)) {
            continue;
        }
        szAlphabet[i] = (char)c;
        i++;
        if (i >= 4) {
            szAlphabet[4] = '\0';
            j = (int)DecodeBase64(szAlphabet, szBuf);
            for (i = 0; i < j; i++) {
                putc(szBuf[i], fpOut);
            }
            i = 0;
            cbData += j;
        }
    }

    fclose(fpIn);
    fclose(fpOut);
    return cbData;
}

//�����������������������������������������������ȉ��̓f�o�b�O���[�`��
#if defined(DEBUG)
#include <stdio.h>
int
main(int argc, char *argv[])
{
    char *lpData = "0123456789ABCDEF";
    char szGroup[128];
    int i;
    int cbData;
    
    for (i = 1; i < 4; i++) {
        printf("%d: %s\n", i, ToAlphabetGroup(lpData, i, szGroup));
        i = FromAlpabetGroup(szGroup, szGroup);
        szGroup[i] = '\0';
        printf("%d: %s\n", i, szGroup);
    }
    for (i = 1; i <= 16; i++) {
        printf("%d: Encode: %s\n", i, EncodeBase64(lpData, i, szGroup));
        cbData = DecodeBase64(szGroup, szGroup);
        szGroup[cbData] = '\0';
        printf("%d: Decode: (LEN=%d) %s\n", i, cbData, szGroup);
    }

    if (argc > 1) {
        printf("%s ---> %s : %d\n", argv[1], argv[2],
               DecodeBase64File(argv[1], argv[2]));
    }
    return 0;
}
#endif
