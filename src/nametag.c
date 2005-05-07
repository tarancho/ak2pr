/* -*- mode: c++ -*-
 * $Id: nametag.c,v 1.1 2005/05/07 12:19:50 tfuruka1 Exp $
 * $Name:  $
 *
 * CVS��Name�L�[���[�h�̒u��
 *
 * $Log: nametag.c,v $
 * Revision 1.1  2005/05/07 12:19:50  tfuruka1
 * CVS��Name�L�[���[�h��a���ɕϊ����鏈��������Ă݂��B
 *
 */
#include <windows.h>
#include <stdio.h>
#include <mbstring.h>

#define MAX_SIZE 1024

static struct {
    LPCTSTR lpszName;
    LPCTSTR lpszJapaneseName;
} repTable[] = {
    {"KOMA", "�D��(������)"},
    {"HIGASHI-OBUKE", "����X(�Ђ��������Ԃ�)"},
    {"OBUKE", "��X(�����Ԃ�)"},
    {"TAIRADATE", "����(�����炾��)"},
    {"KITAMORI", "�k�X(��������)"},
    {"MATSUO-HACHIMANTAI", "����������(�܂��͂��܂񂽂�)"},
    {"APPI-KOGEN", "���䍂��(�����҂�������)"},
    {"AKASAKATA", "�ԍ�c(����������)"},
    {"KOYANOHATA", "�����̔�(����̂͂�)"},
    {"ARAYASHIMMACHI", "�r���V��(����₵��܂�)"},
    {"YOKOMA", "����(�悱��)"},
    {"TAYAMA", "�c�R(�����)"},
    {"ANIHATA", "�Z��(���ɂ͂�)"},
    {"YUZE-ONSEN", "��������(�䂺���񂹂�)"},
    {"HACHIMANTAI", "������(�͂��܂񂽂�)"},
    {"RIKUCHU-OSATO", "�����嗢(�肭���イ��������)"},
    {"KAZUNO-HANAWA", "���p�ԗ�(���Â̂͂Ȃ�)"},
    {"SHIBAHARA", "�ĕ�(���΂Ђ�)"},
    {"TOWADA-MINAMI", "�\�a�c��(�Ƃ킾�݂Ȃ�)"},
    {"SUEHIRO", "���L(�����Ђ�)"},
    {"DOBUKAI", "�y�[��(�ǂԂ���)"},
    {"SAWAJIRI", "��K(���킶��)"},
    {"JUNISHO", "�\��(���イ�ɂ���)"},
    {"OTAKI-ONSEN", "��ꉷ��(�����������񂹂�)"},
    {"OGITA", "��c(��������)"},
    {"HIGASHI-ODATE", "�����(�Ђ�����������)"},
    {"ODATE", "���(��������)"},
    {NULL, NULL}
};

static LPCTSTR
NewFormat(
    LPCTSTR lpszStr,                            // ���̕�����
    LPCTSTR lpszReplaceStart,                   // �u�����J�n�ʒu
    LPCTSTR lpszEndStart,                       // �c��̊J�n�ʒu
    LPCTSTR lpszReplaceStr                      // �u����������
    )
{
    static TCHAR szReturn[MAX_SIZE];
    size_t len = lpszReplaceStart - lpszStr;

    strncpy(szReturn, lpszStr, MAX_SIZE);
    _snprintf(szReturn + len, MAX_SIZE - len,
              "%s%s", lpszReplaceStr, lpszEndStart);
    return szReturn;
}

/*
 * CVS��Name�L�[���[�h���܂񂾕������a���ɕϊ����ĕԋp���܂��B���̕�
 * ����ɂ͕ύX�������܂���B
 */
LPCTSTR WINAPI
ReplaceNameTag(LPCTSTR lpszNameTag)
{
    int i;
    TCHAR szFrom[256];
    LPTSTR p;

    for (i = 0; repTable[i].lpszName; i++) {
        _snprintf(szFrom, 255, "$" "Name: %s" " $", repTable[i].lpszName);
        if (p = _mbsstr(lpszNameTag, szFrom)) {
            return NewFormat(lpszNameTag, p, p + strlen(szFrom),
                             repTable[i].lpszJapaneseName);
        }
        if (p = _mbsstr(lpszNameTag, repTable[i].lpszName)) {
            return NewFormat(lpszNameTag, p, p + strlen(repTable[i].lpszName),
                             repTable[i].lpszJapaneseName);
        }
    }
    return lpszNameTag;
}

#if 0            
int main(int argc, char *argv[])
{
    printf("%s\n", ReplaceNameTag(argv[1]));
}
#endif
