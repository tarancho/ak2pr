/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-01-11 20:22:21 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃N���C�A���g�̋��ʏ�����
 *
 * $Id: clientCommon.c,v 1.12 2004/01/11 11:24:43 tfuruka1 Exp $
 * $Log: clientCommon.c,v $
 * Revision 1.12  2004/01/11 11:24:43  tfuruka1
 * �I�v�V�����X�C�b�`�Ƃ��� / ��F�߂��iEmacs�����Ă���ꍇ������̂Łj�B
 * �����āA/D�I�v�V�����𖳎�����悤�ɂ����B
 *
 * Revision 1.11  2003/03/29 12:38:56  tfuruka1
 * �� SendPrintFromStdin�֐��̎d�l�ǉ��ɂ��C���i�W�����͂̓ǂݍ��݂ƁA
 *    �N���b�v�{�[�h�̓ǂݍ��݂ɑΉ������j
 *
 * Revision 1.10  2003/03/14 14:58:25  tfuruka1
 * �� PostScript�t�@�C���̍�ƃt�@�C���̖��̂����t�@�C����Title�t�B�[���h
 *    ����擾����悤�ɂ��܂����B�܂��ADistiller�ŏ�������t�@�C���̊g��
 *    �q��.PS�ɂ��܂����B
 * �� �T�[�o�̋N���I�v�V���� -S ��ǉ����܂����B�T�[�o�������~��ԂŋN
 *    �����鎖���\�ɂȂ�܂����B
 *
 * Revision 1.9  2003/02/25 15:27:27  tfuruka1
 * �s�ԍ�����̃I�v�V������ǉ������B
 *
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
           "\t\t[-J�^�C�g��] [-s�p���T�C�Y] [-n[-]] [-P] [-S]\n"
           "\t\t[�t�@�C����...]\n\n"
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
           "\t-n[-] �s�ԍ����o�͂��܂��B�w�肵�Ȃ��ꍇ�̓T�[�o�̐ݒ�B\n"
           "\t      -n-���w�肵���ꍇ�͍s�ԍ����o�͂��Ȃ��B\n"
           "\t-P �w�肵�Ă��������܂���B\n"
           "\t-S �T�[�o���N�����鎞�Ɉ����~��ԂŋN�����܂��B\n"
           "\t   �T�[�o�����ɋN�����Ă���ꍇ�͈Ӗ��������܂���B\n"
           "\t�t�@�C���� �������t�@�C�������w�肵�܂��B\n"
           "\t\t�w�肵�Ȃ������ꍇ�́A�W�����͂���ǂݍ��݂܂��B\n"
           "\t\t�����t�@�C���w��ł��܂��B\n--\n",
           GetLongBaseName(arg));

    printf("%s", GetPaperSizeUsageMessage());
}

int ak2prClientCommon(int __argc, char **_argv)
{
    TCHAR szBuf[4096];
    TCHAR szSVOption[128];                      // �T�[�o�̋N���I�v�V����
    double fFont = 0.0;
    int nTab = 0, nUp = 0, i;
    BYTE szTitle[256], *pszTitle = NULL;
    int nFtype = PT_TEXT;
    int nOrientation = 0;                       // �p���̌����̓f�t�H���g
    short dmPaperSize = 0;                      // �p���T�C�Y�̓f�t�H���g
    int bNum = -1;                              // �s�ԍ��o�͂̃f�t�H���g
    // ��boolean�ł͂Ȃ��̂Œ��ӁF�ŏ���boolean�ɂ��Ă����̂����E�E�E

    szSVOption[0] = '\0';
    strcpy(szBuf, "DBG: ");
    for (i = 1; i < __argc; i++) {
        if (4095 < (strlen(szBuf) + strlen(*(__argv + i)) + 1)) {
            break;
        }

        // �T�[�o�N���I�v�V�����̏ꍇ
        if (0 == strcmp(*(__argv + i), "-S")) {
            strcat(szSVOption, " -S");
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
    Syslog(FALSE, "--<<< COMMAND START >>>--");
    Syslog(FALSE, "%s", szBuf);

    ExecutePrtServer((LPCTSTR)szSVOption);      // �T�[�o���N������

    for (i = 1; i < __argc; i++) {
        /* (page headers are not supported) �𖳎����� emacs ��
           print-buffer�΍� */
        if ('(' == **(__argv + i)) {
            continue;
        }
        if (('-' != **(__argv +i)) && ('/' != **(__argv +i))) {
            break;
        }
        switch (*(*(__argv + i) + 1)) {
        case 'D':
            // Emacs���t�^����I�v�V���������f�o�C�X�I�v�V�����͖�����
            // ��
            Syslog(FALSE, "%s �͖������܂�", *(__argv + i));
            continue;
        case 'S':                               // �T�[�o�N���I�v�V�����͖���
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
                printf("-n�I�v�V�����̃p�����[�^���s���ł�: %s\n",
                       *(__argv + i));
                return 1;
            }
            continue;
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
            Syslog(FALSE, "%s �͖������܂�", *(__argv + i));
            continue;
        default:
            Syslog(TRUE, "-ERROR: Invalid Argument (%s)", *(__argv + i));
            Usage(*__argv);
            return 1;
        }
    }

    if (i >= __argc) {                          // �t�@�C�����w��Ȃ��̏ꍇ
        SendPrintFromStdin(FALSE, NULL, pszTitle, nUp, nTab, fFont, nFtype,
                           nOrientation, dmPaperSize, bNum);
        return 0;
    }
    else if (i == (__argc - 1)) {               // �t�@�C����������w�肳�ꂽ
        // PostScript�t�@�C���̏ꍇ�́A�^�C�g�����t�@�C�����瓾��
        if ((PT_PS_ACROBAT == nFtype) || (PT_PS_GHOST == nFtype)) {
            (LPCTSTR)pszTitle
                = GetPSTitle((LPCTSTR)*(__argv + i), szTitle, 128);
            if (NULL == pszTitle) {
                Usage(*__argv);
                return 1;
            }
        }
        SendPrintFromFileCopy(NULL, pszTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize, bNum);
        return 0;
    }

    for (; i < __argc; i++) {
        strncpy(szTitle, GetLongBaseName(*(__argv + i)), 255);

        // PostScript�t�@�C���̏ꍇ�́A�^�C�g�����t�@�C�����瓾�遦
        // Emacs����PostScript�̃t�@�C�����������Ƃ���[/D:PRN]�Ƃ���
        // �I�v�V�������ǉ�����Ă��邪�A���݂̏������Ƃ��̃I�v�V����
        // ���t�@�C���ƌ�F�����Ă��܂��܂��B���R�A���̂悤�ȃt�@�C��
        // �͑��݂��Ȃ��̂ŁA�������āA���ʓI�ɂ��܂������ł��Ă��܂��B
        // ��肠�����A�����͂��̂܂܂̏����ɂ��Ă����܂��B
        if ((PT_PS_ACROBAT == nFtype) || (PT_PS_GHOST == nFtype)) {
            (LPCTSTR)pszTitle
                = GetPSTitle((LPCTSTR)*(__argv + i), szTitle, 128);
            if (NULL == pszTitle) {
                continue;
            }
        }
        SendPrintFromFileCopy(NULL, szTitle, *(__argv + i), nUp, nTab,
                              fFont, nFtype, nOrientation, dmPaperSize, bNum);
    }
    return 0;
}
