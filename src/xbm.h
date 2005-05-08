/* -*- mode: c++ -*-
 * $Id: xbm.h,v 1.2 2005/05/08 13:03:39 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm �t�@�C���̓W�J��
 *
 * $Log: xbm.h,v $
 * Revision 1.2  2005/05/08 13:03:39  tfuruka1
 * X-Face�֘A�̒ǉ�
 *
 * Revision 1.1  2005/04/30 17:15:12  tfuruka1
 * �V�K�ǉ�
 *
 */
#ifndef _XBM_H_
#define _XBM_H_

#include <windows.h>
#include <stdio.h>
#include <io.h>

// �t�@�C���̎��(SetXBM�̈���)
enum {
    SETXBM_XBM = 0,                             // ������XBM�t�@�C��
    SETXBM_UFACE,                               // uncompface
    NUM_OF_XBM
};

// xbm���
typedef struct {
    int kind;                                   // XBM, uncompface
    int nWidth;                                 // ���̃s�N�Z����
    int nHeight;                                // �����̃s�N�Z����
    int cbWidth;                                // ���̃o�C�g��
    size_t cbData;                              // �f�[�^�T�C�Y
    unsigned char *lpData;                      // �f�[�^�G���A
} XBM_INFO, *PXBM_INFO, *LPXBM_INFO;

// �v���g�^�C�v�錾
BOOL WINAPI SetXBM(LPXBM_INFO lpXbm, LPCTSTR lpszFileName, int kind);
int PeekXBM(LPXBM_INFO lpXbm, int x, int y);
BOOL WINAPI
ExecuteUncompface(
    LPTSTR lpszCmdPath,                         // uncompface�p�X
    LPTSTR lpszInFile,                          // ���[���t�@�C��
    LPTSTR lpszOutFile                          // �o�̓t�@�C��
    );
/*
 * XBM���̓��e��`�悵�܂��B
 */
BOOL WINAPI
DrawXBM(
    LPXBM_INFO lpXbm,                           // xbm���
    HDC hDC,                                    // �`�悷��DC
    int x,                                      // �`����W
    int y,                                      // �`����W
    int nWidth,                                 // �`�敝
    int nHeight,                                // �`�捂
    COLORREF crFg,                              // �S�i�F
    COLORREF crBg,                              // �w�i�F
    DWORD dwRop                              // ���X�^�[�I�y���[�V����
    );

#endif

