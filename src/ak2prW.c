/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2000-11-05 11:23:04 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̃N���C�A���g�̃T���v���B���̃N���C�A���g��
 * Emacs��lpr-burrer���璼�ڌĂяo����Ă�����o����悤�ɁC�R�}���h��
 * ���̌`����lpr�ɍ����Ă���B
 *
 * Emacs��lpr-buffer������g�p����ꍇ�́C.emacs�Ɉȉ��̂悤�ɋL�q����
 * �K�v������܂��Bak2prW.exe, ak2prSV.exe, ak2pr.dll���̕K�v�ȃt�@�C
 * ���͑S�� $EMACS/site-bin �ɕۑ����Ă���Ɖߒ����܂��B
 *
 *      (setq lpr-command
 *            (expand-file-name (concat exec-directory
 *                                      "../../site-bin/ak2prW.exe")))
 *      (setq lpr-switches '())
 *      (setq lpr-add-switches t)
 *      (setq lpr-command-switches '())
 *
 * lpr-add-switches��t�ɐݒ肷��ƁC�^�C�g������������M���Ă���܂��B
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
 * RCS�����������Ȃ����̂ŕ�������
 *
 * Revision 1.3  2000/10/22 09:41:30  tfuruka1
 * emacs��print-buffer�Ő���������ł��Ȃ����������C�������B��̓I�ɂ�
 * (page headers are not supported)�̃��b�Z�[�W�𖳎�����悤�ɂ����B�A��
 * ���ۂ̏����ł͐擪��'('�������ꍇ�Ɉ����S�̂𖳎����邾���Ȃ̂����B
 *
 * Revision 1.2  1999/12/16 08:01:51  tfuruka1
 * .emacs�̐ݒ��̃R�����g���L�q�����B
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
