/* -*- C++ -*- Time-stamp: <99/04/08 20:48:16 tfuruka1>
 *
 * �R�}���h���C������g�p�ł���v�����^�o�̓R�}���h�B���̓��[�������
 * ����ׂ����ɍ쐬���܂����B������̓R�}���h���C������������󂯂܂�
 * ��B�W�����͂���ǂݍ���Ńt�@�C�����쐬����fmpr���Ăяo���܂��B
 *
 * �Ⴆ�΁AMew���������鎞�͈ȉ��̂悤�� .emacs�ɋL�q���܂�
 *
 * (setq mew-print-command-format "mpr")
 *
 * (defun mew-print-region (begin end)
 *  (interactive "r")
 *  (shell-command-on-region begin end mew-print-command-format))
 *
 * (defun mew-print-buffer ()
 *  (interactive)
 *  (mew-print-region (point-min) (point-max)))
 *
 * (setq mew-print-function (function mew-print-buffer)) */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
int
main(int argc, char *argv[])
{
    FILE *fp;
    char *p1, *p2, szTemp[MAX_PATH], szBuf[1024];

    if (NULL == (p1 = getenv("TEMP"))) {
        printf("���ϐ�TEMP��������܂���\n");
        return 1;
    }
    sprintf(szTemp, "%s/mprXXXXXX", p1);
    if (!_mktemp(szTemp)) {
        printf("��ƃt�@�C�����쐬�ł��܂���\n");
        return 2;
    }
    if (NULL == (fp = fopen(szTemp, "wt"))) {
        printf("��ƃt�@�C�� %s ���쐬�ł��܂���\n", szTemp);
        return 3;
    }
    
    while (fgets(szBuf, 1024, stdin)) {
        fprintf(fp, "%s", szBuf);
    }
    fclose(fp);

    // �t�@�C���̃t���p�X�𓾂�
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        printf("���W���[���n���h���̎擾�Ɏ��s���܂��� -- %d",
               GetLastError());
        return 4;
    }
    
    p1 = strrchr(szBuf, '\\');
    p2 = strrchr(szBuf, '/');
    p1 = ((ULONG)p1 > (ULONG)p2) ? p1 : p2;
    if (!p1) {
        strcpy(szBuf, "fmpr.exe");
    }
    else {
        strcpy(p1, "/fmpr.exe");
    }

    strcat(szTemp, " DEL");                     // �����폜
    if (32 >= (int)ShellExecute(NULL, NULL, szBuf, szTemp, NULL, 0)) {
        printf("%s�̋N���Ɏ��s���܂���. %d\n", szBuf, GetLastError());
        return 5;
    }

    printf("�u%s %s�v���o�b�N�O���E���h�Ŏ��s��...\n", szBuf, szTemp);

    return 0;
}
