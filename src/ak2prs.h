/* -*- mode: C++; coding: sjis-dos; -*-
 *
 * 「ak2psのようなもの」のサーバ側のヘッダファイル
 *
 * $Id: ak2prs.h,v 1.6 2001/09/28 15:49:05 tfuruka1 Exp $
 * $Log: ak2prs.h,v $
 * Revision 1.6  2001/09/28 15:49:05  tfuruka1
 * 【Version 1.4 にしました】ak2pr.exe, ak2prW.exeで引数をデバッグ用にダ
 * ンプしている処理でメモリ破壊をおこしていたおばかなバグを修正しました
 * （引数を沢山指定：例えば、ファイルを999個ぐらい指定すると、メモリを破
 * 壊していた）。
 *
 * Revision 1.5  2001/08/19 09:03:25  tfuruka1
 * ───── Version 1.3にしました  ─────
 * RCSのRevisionと一致させるのは無理が（頻繁に上がり過ぎるので）あるので、
 * やっぱり、手動で上げる事にした。
 *
 * 1.2からの面な差分は
 * ●テンポラリファイルの不足(27個)が発生しないようにした。
 * ●クライアントとサーバでワーキングディレクトリが異なっている場合、印刷
 *   ファイルを相対パスで指定した場合に、サーバ側でファイル未検出になる問
 *   題を修正。
 * ●PostScriptファイルに対応した(といっても、実際は、Acrobat Distillerか、
 *   GhostScriptにフォワードするだけ)。
 *
 * Revision 1.4  2001/08/19 04:37:45  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
 * Revision 1.3  2001/08/18 17:05:03  tfuruka1
 * VERSIONを上げるのを忘れてた・・・面倒なので、RCSのRevisionを使用するよ
 * うにした。
 *
 * Revision 1.2  2001/08/18 16:34:06  tfuruka1
 * ●PRT_INFO構造体からbDeleteメンバを削除した。
 * ●DeleteQueue関数が新規追加されたので、プロトタイプ宣言を追加した。
 *
 * Revision 1.1  2001/02/05 17:16:50  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#ifndef _AK2PRS_H_
#define _AK2PRS_H_

#define TIMESTAMP "Time-stamp: <2001-09-29 00:47:02 tfuruka1>"
#define VERSION   "Version 1.4"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <mbstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "resource.h"

#include "ak2pr.h"

#define IsShellTray() FindWindow("Shell_TrayWnd", NULL)

#define SV_DBGCLASS "ak2psのようなもの - ak2pr(Server-Detail)"
#define SV_CLASS    "ak2psのようなもの - ak2pr(Server)"
#define SV_CAPTION  "ak2pr Server"

#define SV_EXE_NAME "ak2prSV.EXE"

#define COPYRIGHT   "ak2pr " VERSION\
" Copyright 1997-2001 By T.Furukawa\n(tfuruka1@nifty.com)" TIMESTAMP

// 1バイト目が漢字コードか否かチェックするマクロ
#define IsKanjiSJIS(c)  (((c) >= 0x81 && (c) <= 0x9f) ||\
                         ((c) >= 0xe0 && (c) <= 0xfc))

// フォント名
#define FN_MSPM     TEXT("ＭＳ Ｐ明朝")
#define FN_MSM      TEXT("ＭＳ 明朝")
#define FN_MSPG     TEXT("ＭＳ Ｐゴシック")
#define FN_MSG      TEXT("ＭＳ ゴシック")
#define FN_COU      TEXT("Courier New")
#define FN_ARIAL    TEXT("Arial")

// ConvX2Dt, ConvDt2X のフラグ
#define CX_CM       1                           // cmで取得
#define CX_MM       2                           // mmで取得
#define CX_PT       3                           // pointで取得

// GetPrtCenter のフラグ(論理和指定)
#define GPC_W       1                           // 横
#define GPC_H       2                           // 縦
#define GPC_IM      4                           // マージン無視

#define HANDLE_WM_REALCLOSE(hwnd, wParam, lParam, fn) \
    ((BOOL)(fn)((hwnd)))
#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
          (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))
#define HANDLE_WM_SUSPEND(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

// ユーザメッセージ
enum {WM_TASKMENU = WM_USER + 1, WM_REALCLOSE, WM_SUSPEND};

// 印刷情報です。この構造体に値を設定してWM_COPYDATAでSV_CLASS,
// SV_CAPTIONのウインドウを所有しているプロセスにSendMessageすると印刷
// を行います。
typedef struct _PrtInfo{
    CHAR szFileName[MAX_PATH];                  // ファイル名
    CHAR szTitle[256];                          // タイトル文字列
    int nNumOfUp;                               // 段組数
    int nTab;                                   // TAB幅
    int nType;                                  // 印刷タイプ
    CHAR szTimeStamp[64];
    CHAR szTimeStamp1[64];
    int nBaseLine;                              // 0: Non, 1:E, 2:J, 9:A
    BOOL bNum;                                  // T: 行番号印刷
    BOOL bKeisen;                               // T: 罫線連結
    BOOL bNoRcvHeader;                          // Receivedヘッダを印字しない
    BOOL bColor;                                // T: Color印刷
    double fFontSize;                           // フォントサイズ --- Point
} PRT_INFO, *PPRT_INFO;

// nType に設定する値は以下の通り(クライアント側でも参照したいので、
// ak2pr.hに移動), デフォルトはPT_TEXT


// スレッド間メールボックス
typedef struct {
    HWND hWnd;                                  // ハンドル
    HDC hDC;                                    // デバイスハンドル
    HGLOBAL hDevMode;                           // プリンタデバイスモード
    HGLOBAL hDevNames;                          // プリンタデバイスネーム
    HANDLE hThread;                             // スレッドハンドル
    DWORD idThread;                             // スレッドID
    BOOL bRun;                                  // T:スレッド実行状態
    BOOL bStop;                                 // T:印刷停止状態
    PRT_INFO PrtInfo;                           // 印刷情報
    // ----- 以下はメール印刷の情報
    TCHAR szFrom[128];                          // 差出人
    TCHAR szDate[128];                          // 受信日時
    TCHAR szReference[128];                     // リファレンス
    TCHAR szMessageID[128];                     // メッセージID
    // ----- 以下はPostScript印刷時の情報
    TCHAR szAcrobat[MAX_PATH];                  // Distiller in フォルダ
    TCHAR szGsPath[MAX_PATH];                   // GhostScript パス
    TCHAR szGsOpt[512];                         // GhostScript option
} MAILBOX, *PMAILBOX;

// 外部定義
extern PRT_INFO g_PrtInfo;                      // デフォルトの印刷情報
extern MAILBOX g_MailBox;                       // スレッド間メールボックス
extern CRITICAL_SECTION g_csCriticalSection;    // クリティカルセクション

// プロトタイプ
HWND CreateApplication(HANDLE hInstance);

LRESULT CALLBACK
MainWndProc(
    HWND hWnd,                                  // ウインドウハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    );

LRESULT CALLBACK
MainWndProcDetail(
    HWND hWnd,                                  // ウインドウハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // パラメータ
    LPARAM lParam                               // パラメータ
    );

VOID PrintThread(LPDWORD lpIDThread);
VOID SetupPrtStyle(HWND hWnd);

BOOL
EnQueue(
    HWND hWnd,                                  // ハンドル
    PPRT_INFO pPrtInfo                          // 印刷情報
    );

BOOL
DeQueue(
    HWND hWnd,                                  // ハンドル
    PPRT_INFO pPrtInfo                          // 印刷情報
    );
int
DeleteQueue(
    HWND hWnd,                                  // ハンドル
    BOOL bForce                                 // T:全て削除
    );
BOOL
SetupPrinter(
    HWND hWnd,                                  // ハンドル
    HGLOBAL *lphDevNames,
    HGLOBAL *lphDevMode
    );
HPEN
CreatePrtPen(
    UINT nStyle,                                // ペンのスタイル
    LONG nWidth,                                // 太さ
    COLORREF crColor                            // 色
    );
HFONT
CreatePrtFont(
    LPTSTR lpszFontName,                        // フォント名
    int nHeight,                                // フォントの高さ
    int nWeight,                                // フォントの太さ
    BOOL bItalic,                               // T: イタリック
    BOOL bUnderline,                            // T: 下線
    BOOL bStrikeOut,                            // T: 打ち消し線
    BOOL bSjis                                  // T: 日本語
    );
LPTSTR
ufgets(
    LPTSTR szBuf,                               // 格納バッファ
    int cbMax,                                  // 最大文字列長
    FILE *fp,                                   // ファイルポインタ
    int nTab                                    // タブストップ
    );
/* -------------------------------------------------------------------
 * lpszStrプリンタへ出力する。フォントは指定されたパラメータを元に作成
 * する。
 * *-----------------------------------------------------------------*/
BOOL SetFontAndPrint(
    LPTSTR lpszStr,                             // 出力文字列
    LPTSTR lfFaceName,                          // フォント名
    UINT lfHeight,                              // フォントの高さ
    UINT lfWeight,                              // フォントのウエイト
    BOOL lfItalic,                              // T:イタリック
    BOOL lfUnderline,                           // T:下線
    BOOL lfStrikeout,                           // T:打ち消し線
    BOOL bJapan                                 // T:日本語フォント
    );
LPTSTR TrimString(LPSTR lpszStr);
LPTSTR TrimLeft(LPTSTR lpszStr);
LPTSTR TrimRight(LPTSTR lpszStr);
void DrawTombow(void);
void DoTestPrint(void);
void GetDefaultPrtInfo(void);
void SetDefaultPrtInfo(void);
int GetPrtWd(void);
int GetPrtHt(void);
int GetPrtMgW(void);
int GetPrtMgH(void);
int GetPrtDpiW(void);
int GetPrtDpiH(void);
int GetPrtCenter(int flg);
int GetPrtBasePoint(void);
double ConvDt2X(int value, int nDpi, int flg);
int ConvX2Dt(double value, int nDpi, int flg);
BOOL BeginDocument(void);
BOOL BeginPage(void);
BOOL EndPageDocument(void);
BOOL EndDocument(void);
BOOL PutsPrinter(LPTSTR szBuf);
void PrintMail(void);
void PrintText(void);
#endif
