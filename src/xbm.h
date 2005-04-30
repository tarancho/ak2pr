/* -*- mode: c++ -*-
 * $Id: xbm.h,v 1.1 2005/04/30 17:15:12 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm �t�@�C���̓W�J��
 *
 * $Log: xbm.h,v $
 * Revision 1.1  2005/04/30 17:15:12  tfuruka1
 * �V�K�ǉ�
 *
 */
#ifndef _XBM_H_
#define _XBM_H_

#include <windows.h>
#include <stdio.h>

// �t�@�C���̎��(AllocXBM�̈���)
enum {
    AXBM_XBM = 0,                               // ������XBM�t�@�C��
    AXBM_UFACE,                                 // uncompface
    NUM_OF_XBM
};

// xbm���
typedef struct {
    int kind;                                   // XBM, uncompface
    int nWidth;                                 // ���̃s�N�Z����
    int nHeight;                                // �����̃s�N�Z����
    int cbWidth;                                // ���̃o�C�g��
    int cbData;                                 // �f�[�^�T�C�Y
    unsigned char *lpData;                      // �f�[�^�G���A
} XBM_INFO, *PXBM_INFO, *LPXBM_INFO;

// �v���g�^�C�v�錾
LPXBM_INFO AllocXBM(FILE *fp, int kind);
void FreeXBM(LPXBM_INFO lpXbm);
int PeekXBM(LPXBM_INFO lpXbm, int x, int y);

#endif

