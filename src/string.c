/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:41:42 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̕����񑀍�֘A
 *
 * $Id: string.c,v 1.2 2004/06/18 00:57:56 tfuruka1 Exp $
 * $Log: string.c,v $
 * Revision 1.2  2004/06/18 00:57:56  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
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
 * ������̉E���̃X�y�[�X��������菜��
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
 * ������̍����̃X�y�[�X��������菜��
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

    if (pFrom == pTo) {                         // �]���ȕ����͖���
        return lpszStr;
    }
    for (; *pFrom; pFrom++, pTo++) {
        *pTo = *pFrom;
    }
    *pTo = '\0';
    return lpszStr;
}

/* -------------------------------------------------------------------
 * ������̍��E�̃X�y�[�X��������菜��
 * *-----------------------------------------------------------------*/
LPTSTR TrimString(LPSTR lpszStr)
{
    return TrimLeft(TrimRight(lpszStr));
}

/* -------------------------------------------------------------------
 * fgets�Ɠ��l�̏������s�����A�^�u���X�y�[�X������֓W�J����B�A���A�^
 * �u�̓W�J��r���Ŏ~�߂鎖�͖����̂ŁA�ő啶���񒷂̓^�u�X�g�b�v���l
 * ������K�v������B
 * *-----------------------------------------------------------------*/
LPTSTR
ufgets(
    LPTSTR szBuf,                               // �i�[�o�b�t�@
    int cbMax,                                  // �ő啶����
    FILE *fp,                                   // �t�@�C���|�C���^
    int nTab                                    // �^�u�X�g�b�v
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
