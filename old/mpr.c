/* -*- C++ -*- Time-stamp: <99/04/08 20:48:16 tfuruka1>
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
int
main(int argc, char *argv[])
{
    FILE *fp;
    char *p1, *p2, szTemp[MAX_PATH], szBuf[1024];

    if (NULL == (p1 = getenv("TEMP"))) {
        printf("環境変数TEMPが見つかりません\n");
        return 1;
    }
    sprintf(szTemp, "%s/mprXXXXXX", p1);
    if (!_mktemp(szTemp)) {
        printf("作業ファイルが作成できません\n");
        return 2;
    }
    if (NULL == (fp = fopen(szTemp, "wt"))) {
        printf("作業ファイル %s が作成できません\n", szTemp);
        return 3;
    }
    
    while (fgets(szBuf, 1024, stdin)) {
        fprintf(fp, "%s", szBuf);
    }
    fclose(fp);

    // ファイルのフルパスを得る
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        printf("モジュールハンドルの取得に失敗しました -- %d",
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

    strcat(szTemp, " DEL");                     // 印刷後削除
    if (32 >= (int)ShellExecute(NULL, NULL, szBuf, szTemp, NULL, 0)) {
        printf("%sの起動に失敗しました. %d\n", szBuf, GetLastError());
        return 5;
    }

    printf("「%s %s」をバックグラウンドで実行中...\n", szBuf, szTemp);

    return 0;
}
