/* -*- mode: c++ -*-
 * $Id: xbm.h,v 1.2 2005/05/08 13:03:39 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm ファイルの展開等
 *
 * $Log: xbm.h,v $
 * Revision 1.2  2005/05/08 13:03:39  tfuruka1
 * X-Face関連の追加
 *
 * Revision 1.1  2005/04/30 17:15:12  tfuruka1
 * 新規追加
 *
 */
#ifndef _XBM_H_
#define _XBM_H_

#include <windows.h>
#include <stdio.h>
#include <io.h>

// ファイルの種類(SetXBMの引数)
enum {
    SETXBM_XBM = 0,                             // 純粋なXBMファイル
    SETXBM_UFACE,                               // uncompface
    NUM_OF_XBM
};

// xbm情報
typedef struct {
    int kind;                                   // XBM, uncompface
    int nWidth;                                 // 幅のピクセル数
    int nHeight;                                // 高さのピクセル数
    int cbWidth;                                // 幅のバイト数
    size_t cbData;                              // データサイズ
    unsigned char *lpData;                      // データエリア
} XBM_INFO, *PXBM_INFO, *LPXBM_INFO;

// プロトタイプ宣言
BOOL WINAPI SetXBM(LPXBM_INFO lpXbm, LPCTSTR lpszFileName, int kind);
int PeekXBM(LPXBM_INFO lpXbm, int x, int y);
BOOL WINAPI
ExecuteUncompface(
    LPTSTR lpszCmdPath,                         // uncompfaceパス
    LPTSTR lpszInFile,                          // メールファイル
    LPTSTR lpszOutFile                          // 出力ファイル
    );
/*
 * XBM情報の内容を描画します。
 */
BOOL WINAPI
DrawXBM(
    LPXBM_INFO lpXbm,                           // xbm情報
    HDC hDC,                                    // 描画するDC
    int x,                                      // 描画座標
    int y,                                      // 描画座標
    int nWidth,                                 // 描画幅
    int nHeight,                                // 描画高
    COLORREF crFg,                              // 全景色
    COLORREF crBg,                              // 背景色
    DWORD dwRop                              // ラスターオペレーション
    );

#endif

