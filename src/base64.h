/* -*- mode: c++; coding: sjis; -*-
 * Copyright (C) 2005 Tadamegu Furukawa
 * $Id: base64.h,v 1.1 2005/08/03 01:33:47 tfuruka1 Exp $
 * $Name:  $
 *
 * Base64関連
 *
 * $Log: base64.h,v $
 * Revision 1.1  2005/08/03 01:33:47  tfuruka1
 * Face対応の前準備としてBase64のデコードとImagiMagicのconvert.exeを指定
 * できるように対応。
 *
 */
#ifndef _BASE64_H_
#define _BASE64_H_

#include <stdio.h>
#include <string.h>

/*
 * Base64にエンコードします。エンコード結果格納域は、元データの 4/3 倍
 * のエリアが必要になります(3バイトが4バイトになります)。エンコード後
 * のデータ長は必ずの4の倍数になります。また、最後にnullストップを付与
 * しますので、さらに一バイト余分に必要になります。
 */
char *
EncodeBase64(
    unsigned char *lpData,                      // エンコードデータ
    size_t cbData,                              // バイト数
    char *lpszEncode                         // エンコード結果(文字列)
    );

/*
 * Base64でデコードします。デコードされる元の文字列長は必ず4の倍数であ
 * る必要があります。デコード後のバイト数を返却します。デコード前のエ
 * リアとデコード後のエリアは同じアドレスを指定する事が可能です。
 */
unsigned long
DecodeBase64(
    char *lpszData,                             // 必ず4の倍数である事
    unsigned char *lpData                       // デコード結果
    );

/*
 * Base64でエンコードされたファイルをデコードし、ファイルに出力します。
 * 入力ファイルと出力ファイルを同じにする事はできません。
 */
unsigned long
DecodeBase64File(
    unsigned char *lpszBase64FileName,          // 入力ファイル名
    unsigned char *lpszOutFileName              // 出力ファイル名
    );

#endif
