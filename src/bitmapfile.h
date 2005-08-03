/* -*- mode: c++; coding: sjis; -*-
 * Copyright (C) 2005 Tadamegu Furukawa
 * $Id: bitmapfile.h,v 1.1 2005/08/03 03:43:42 tfuruka1 Exp $
 * $Name:  $
 *
 * �r�b�g�}�b�v�t�@�C���֘A
 *
 * $Log: bitmapfile.h,v $
 * Revision 1.1  2005/08/03 03:43:42  tfuruka1
 * �V�K�ǉ�(Face�Ή��̑O����)
 *
 */
#ifndef _BITMAPFILE_H_
#define _BITMAPFILE_H_

#define WINVER 0x500
#include <windows.h>
#include <stdio.h>

/* 
 * �r�b�g�}�b�v�t�@�C����ǂݍ���, �r�b�g�}�b�v�n���h�����쐬���܂��B
 * ���������ꍇ��, BITMAP �n���h����ԋp���܂��B���s�����ꍇ�� NULL ��
 * �ԋp���܂��B�ԋp�����r�b�g�}�b�v���s�v�ɂȂ�����, DeleteObject �֐�
 * �ō폜���Ă��������B
 *
 * �J���[�p���b�g�ɂ͑Ή����Ă��܂���B
 *
 */
HBITMAP WINAPI
LoadBitMapFile(
    HDC hDC,                                    // ���̃f�o�C�X�ƌ݊�
                                                // ���̂���r�b�g�}�b
                                                // �v���쐬����܂�
    LPCTSTR lpszBmFile                          // �t�@�C����
    );

/*
 * �r�b�g�}�b�v���r�b�g�}�b�v�t�@�C���ɕۑ����܂��B
 */
BOOL WINAPI
SaveBitmapFile(
    HDC hDC,
    HBITMAP hBm,
    LPCTSTR lpszSaveFileName
    );

#endif
