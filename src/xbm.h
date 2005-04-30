/* -*- mode: c++ -*-
 * $Id: xbm.h,v 1.1 2005/04/30 17:15:12 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm ファイルの展開等
 *
 * $Log: xbm.h,v $
 * Revision 1.1  2005/04/30 17:15:12  tfuruka1
 * 新規追加
 *
 */
#ifndef _XBM_H_
#define _XBM_H_

#include <windows.h>
#include <stdio.h>

// ファイルの種類(AllocXBMの引数)
enum {
    AXBM_XBM = 0,                               // 純粋なXBMファイル
    AXBM_UFACE,                                 // uncompface
    NUM_OF_XBM
};

// xbm情報
typedef struct {
    int kind;                                   // XBM, uncompface
    int nWidth;                                 // 幅のピクセル数
    int nHeight;                                // 高さのピクセル数
    int cbWidth;                                // 幅のバイト数
    int cbData;                                 // データサイズ
    unsigned char *lpData;                      // データエリア
} XBM_INFO, *PXBM_INFO, *LPXBM_INFO;

// プロトタイプ宣言
LPXBM_INFO AllocXBM(FILE *fp, int kind);
void FreeXBM(LPXBM_INFO lpXbm);
int PeekXBM(LPXBM_INFO lpXbm, int x, int y);

#endif

