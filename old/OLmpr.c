/* -*- C++ -*- Time-stamp: <99/04/08 21:18:20 tfuruka1>
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
#include <sys/types.h>
#include <sys/stat.h>

int
main(int argc, char *argv[])
{
    FILE *fp, *infp;
    char *p1, *p2, szTemp[MAX_PATH], szBuf[1024];
    char *lpBuf;
    int hFile;
    struct stat buff;

    if (2 != argc) {
        return 0;
    }
    if (NULL == (p1 = getenv("TEMP"))) {
        printf("���ϐ�TEMP��������܂���\n");
        return 1;
    }

    if (NULL == (infp = fopen(argv[1], "rt"))) {
        perror(argv[1]);
        return 2;
    }

    sprintf(szTemp, "%s/mprXXXXXX", p1);
    if (!_mktemp(szTemp)) {
        printf("��ƃt�@�C�����쐬�ł��܂���\n");
        return 3;
    }
    if (NULL == (fp = fopen(szTemp, "wt"))) {
        printf("��ƃt�@�C�� %s ���쐬�ł��܂���\n", szTemp);
        return 4;
    }
    
    while (fgets(szBuf, 1024, infp)) {
        fprintf(fp, "%s", szBuf);
    }
    fclose(fp);
    fclose(infp);

    if (-1 == stat(szTemp, &buff)) {
        perror("stat");
        return 5;
    }
    if (NULL == (lpBuf = malloc(buff.st_size + 1))) {
        printf("������������܂���\n");
        return 6;
    }

    if (NULL == (fp = fopen(szTemp, "rb"))) {
        printf("��ƃt�@�C�� %s ���J���܂���\n", szTemp);
        return 7;
    }
    // �S�f�[�^��ǂݒ���
    fread(lpBuf, buff.st_size, 1, fp);
    *(lpBuf + buff.st_size) = '\0';
    fclose(fp);

    if (p1 = strstr(lpBuf, "\r\n\r\n")) {
        *p1 = '\0';
    }
    DecodeMHS(lpBuf);
    *p1 = '\r';
    Iso2022jpToSjis(p1, 'B');

    if (NULL == (fp = fopen(szTemp, "wb"))) {
        perror(szTemp);
        return 8;
    }
    for (p2 = lpBuf; *p2; p2++) {
        putc(*p2, fp);
    }
    for (p2 = p1; *p2; p2++) {
        putc(*p2, fp);
    }
    fclose(fp);
    free(lpBuf);

    // �t�@�C���̃t���p�X�𓾂�
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        printf("���W���[���n���h���̎擾�Ɏ��s���܂��� -- %d",
               GetLastError());
        return 10;
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
        return 20;
    }

    printf("�u%s %s�v���o�b�N�O���E���h�Ŏ��s��...\n", szBuf, szTemp);

    return 0;
}
