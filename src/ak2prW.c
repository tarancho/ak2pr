/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2000-11-05 11:23:04 tfuruka1>
 *
 * 「ak2psのようなもの」のクライアントのサンプル。このクライアントは
 * Emacsのlpr-burrerから直接呼び出されても動作出来るように，コマンド引
 * 数の形式をlprに合せている。
 *
 * Emacsのlpr-buffer等から使用する場合は，.emacsに以下のように記述する
 * 必要があります。ak2prW.exe, ak2prSV.exe, ak2pr.dll等の必要なファイ
 * ルは全て $EMACS/site-bin に保存していると過程します。
 *
 *      (setq lpr-command
 *            (expand-file-name (concat exec-directory
 *                                      "../../site-bin/ak2prW.exe")))
 *      (setq lpr-switches '())
 *      (setq lpr-add-switches t)
 *      (setq lpr-command-switches '())
 *
 * lpr-add-switchesをtに設定すると，タイトル文字列も送信してくれます。
 *
 * $Id: ak2prW.c,v 1.1 2001/02/05 17:50:39 tfuruka1 Exp $
 * $Log: ak2prW.c,v $
 * Revision 1.1  2001/02/05 17:50:39  tfuruka1
 * Initial revision
 *
 * Revision 1.1  2000/11/05 03:51:24  tfuruka1
 * Initial revision
 *
 * Revision 1.4  2000/10/22 11:22:34  tfuruka1
 * RCSがおかしくなったので復旧した
 *
 * Revision 1.3  2000/10/22 09:41:30  tfuruka1
 * emacsのprint-bufferで正しく印刷できなかった問題を修正した。具体的には
 * (page headers are not supported)のメッセージを無視するようにした。但し
 * 実際の処理では先頭が'('だった場合に引数全体を無視するだけなのだが。
 *
 * Revision 1.2  1999/12/16 08:01:51  tfuruka1
 * .emacsの設定例のコメントを記述した。
 *
 * Revision 1.1 1999/12/16 04:30:16 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "ak2pr.h"

extern int ak2prClientCommon(int __argc, char **_argv);

APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR lpszCmdLine,
    int nCmdShow)
{
    return ak2prClientCommon(__argc, __argv);
}
