/* -*- mode: c++; coding: sjis; -*-
 * Copyright (C) 2005 Tadamegu Furukawa
 * $Id: bitmapfile.h,v 1.2 2005/08/03 10:09:20 tfuruka1 Exp $
 * $Name:  $
 *
 * ビットマップファイル関連
 *
 * $Log: bitmapfile.h,v $
 * Revision 1.2  2005/08/03 10:09:20  tfuruka1
 * メール印刷でFaceを印刷できるようにしました。
 *
 * Revision 1.1  2005/08/03 03:43:42  tfuruka1
 * 新規追加(Face対応の前準備)
 *
 */
#ifndef _BITMAPFILE_H_
#define _BITMAPFILE_H_

#include <windows.h>
#include <stdio.h>

/* 
 * ビットマップファイルを読み込み, ビットマップハンドルを作成します。
 * 成功した場合は, BITMAP ハンドルを返却します。失敗した場合は NULL を
 * 返却します。返却したビットマップが不要になったら, DeleteObject 関数
 * で削除してください。
 *
 * カラーパレットには対応していません。
 *
 */
HBITMAP WINAPI
LoadBitMapFile(
    HDC hDC,                                    // このデバイスと互換
                                                // 性のあるビットマッ
                                                // プが作成されます
    LPCTSTR lpszBmFile                          // ファイル名
    );

/*
 * ビットマップをビットマップファイルに保存します。
 */
BOOL WINAPI
SaveBitmapFile(
    HDC hDC,
    HBITMAP hBm,
    LPCTSTR lpszSaveFileName
    );

#endif
