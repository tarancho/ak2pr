/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-08 01:11:29 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃N���C�A���g�̋��ʏ�����
 *
 * $Id: clientCommon.c,v 1.8 2001/12/07 18:24:29 tfuruka1 Exp $
 * $Log: clientCommon.c,v $
 * Revision 1.8  2001/12/07 18:24:29  tfuruka1
 * �p���T�C�Y�̎w����o����悤�ɂ����B
 *
 * Revision 1.7  2001/10/12 14:07:34  tfuruka1
 * �ŏ��ɖ������ɃT�[�o���N������悤�ɂ����B���������ŃT�[�o���N��������
 * ����z�肵�Ă���B�܂��A�s���ȃI�v�V�������w�肵���ꍇ�ɁAUsage���b�Z�[
 * �W���o�͂���悤�ɂ����B
 *
 * Revision 1.6  2001/10/01 13:19:42  tfuruka1
 * -o�I�v�V������ǉ��B�p���̌������w��o����悤�ɂ����B
 *
 * Revision 1.5  2001/09/28 15:45:54  tfuruka1
 * �������f�o�b�O�p�Ƀ_���v���Ă���ꏊ�ŁA�������j����N�����Ă����i����
 * ���ȁj�o�O���C���B
 *
 * Revision 1.4  2001/08/19 04:39:54  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.3  2001/08/18 16:44:17  tfuruka1
 * ��SendPrintFromFile���p�~����ASendPrintFromFileCopy�ɓ������ꂽ�ׁA
 *   SendPrintFromFileCopy�֐����g�p����悤�ɏC��(�����t�@�C���w�肳�ꂽ
 *   ��)�B
 *
 * Revision 1.2  2001/02/05 17:49:24  tfuruka1
 * ���s�R�[�h�̏C��
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
           "\t\t[-f�t�H���g�T�C�Y] [-t�^�u��] [-u�i�g��] [-T�^�C�g��]\n"
           "\t\t[-J�^�C�g��] [-s�p���T�C�Y] [-P] [�t�@�C����...]\n\n"
           "\t-o �p���̌������w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t\tp PORTRAIT\n"
           "\t\tl LANDSCAPE\n"
           "\t-m �t�@�C���̎�ނ��w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t\tPLAIN      �v���[���e�L�X�g�t�@�C��\n"
           "\t\tMAIL       �C���^�[�l�b�g���[��(SJIS)\n"
           "\t\tPS_ACROBAT PostScript�t�@�C��(Acrobat Distiller�ŕϊ�)\n"
           "\t\tPS_GHOST   PostScript�t�@�C��(GhostScript�ň��)\n"
           "\t-f �t�H���g�T�C�Y���w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t-t �^�u�����w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t-u �i�g�����w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t-T �^�C�g�����w�肵�܂�(-J�Ɠ���)�B�f�t�H���g�̓t�@�C�����B\n"
           "\t-J �^�C�g�����w�肵�܂�(-T�Ɠ���)�B�f�t�H���g�̓t�@�C�����B\n"
           "\t-s �p���T�C�Y���w�肵�܂��B�f�t�H���g�̓T�[�o�̐ݒ�B\n"
           "\t-P �w�肵�Ă��������܂���B\n"
           "\t�t�@�C���� �������t�@�C�������w�肵�܂��B\n"
           "\t\t�w�肵�Ȃ������ꍇ�́A�W�����͂���ǂݍ��݂܂��B\n"
           "\t\t�����t�@�C���w��ł��܂��B\n--\n",
           GetLongBaseName(arg));

    printf("%s", GetPaperSizeUsageMessage());
}

int ak2prClientCommon(int __argc, char **_argv)
{
    double fFont = 0.0;
    int nTab = 0, nUp = 0, i;
    BYTE szTitle[256], *pszTitle = NULL;
    int nFtype = PT_TEXT;
    int nOrientation = 0;                       // �p���̌����̓f�t�H���g
    short dmPaperSize = 0;                      // �p���T�C�Y�̓f�t�H���g

    ExecutePrtServer();                         // �T�[�o���N������
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
        /* (page headers are not supported) �𖳎����� emacs ��
           print-buffer�΍� */
        if ('(' == **(__argv + i)) {
            continue;
        }
        if ('-' != **(__argv +i)) {
            break;
        }
        switch (*(*(__argv + i) + 1)) {
        case 's':
            if (!(dmPaperSize = GetPaperSizeDevMode(*(__argv + i) + 2))) {
                printf("�p���T�C�Y���s���ł�: %s\n%s", *(__argv + i) + 2,
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
        case 'P':                               // ���̃p�����[�^�͖���
            continue;
        default:
            Syslog("-ERROR: Invalid Argument (%s)", *(__argv + i));
            Usage(*__argv);
            return 1;
        }
    }

    if (i >= __argc) {                          // �t�@�C�����w��Ȃ��̏ꍇ
        SendPrintFromStdin(NULL, pszTitle, nUp, nTab, fFont, nFtype,
                           nOrientation, dmPaperSize);
        return 0;
    }
    else if (i == (__argc - 1)) {               // �t�@�C����������w�肳�ꂽ
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
