/* -*- C++ -*- Time-stamp: <99/04/08 21:18:20 tfuruka1>
 *
 * コマンドラインから使用できるプリンタ出力コマンド。実はメールを印刷
 * する為だけに作成しました。こちらはコマンドラインから引数を受けませ
 * ん。標準入力から読み込んでファイルを作成してfmprを呼び出します。
 *
 * 例えば、Mewから印刷する時は以下のように .emacsに記述します
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
        printf("環境変数TEMPが見つかりません\n");
        return 1;
    }

    if (NULL == (infp = fopen(argv[1], "rt"))) {
        perror(argv[1]);
        return 2;
    }

    sprintf(szTemp, "%s/mprXXXXXX", p1);
    if (!_mktemp(szTemp)) {
        printf("作業ファイルが作成できません\n");
        return 3;
    }
    if (NULL == (fp = fopen(szTemp, "wt"))) {
        printf("作業ファイル %s が作成できません\n", szTemp);
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
        printf("メモリが足りません\n");
        return 6;
    }

    if (NULL == (fp = fopen(szTemp, "rb"))) {
        printf("作業ファイル %s を開けません\n", szTemp);
        return 7;
    }
    // 全データを読み直す
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

    // ファイルのフルパスを得る
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        printf("モジュールハンドルの取得に失敗しました -- %d",
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

    strcat(szTemp, " DEL");                     // 印刷後削除
    if (32 >= (int)ShellExecute(NULL, NULL, szBuf, szTemp, NULL, 0)) {
        printf("%sの起動に失敗しました. %d\n", szBuf, GetLastError());
        return 20;
    }

    printf("「%s %s」をバックグラウンドで実行中...\n", szBuf, szTemp);

    return 0;
}
