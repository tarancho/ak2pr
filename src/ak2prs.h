/* -*- mode: C++; coding: sjis-dos; -*-
 *
 * 「ak2psのようなもの」のサーバ側のヘッダファイル
 *
 * $Id: ak2prs.h,v 1.25 2004/12/23 08:11:56 tfuruka1 Exp $
 * $Log: ak2prs.h,v $
 * Revision 1.25  2004/12/23 08:11:56  tfuruka1
 * シングルライン印刷(食ミ出した部分を印刷しない)に対応しました。とりあえ
 * ず、サーバ側の設定のみです。
 *
 * Revision 1.24  2004/08/21 11:12:49  tfuruka1
 * VersionをVersionファイルから取得するようにしました。
 *
 * Revision 1.23  2004/08/21 01:01:01  tfuruka1
 * テキスト印刷に於いて「行間」と「罫線連結」が有効になるようにしました。
 *
 * Revision 1.22  2004/06/18 00:57:43  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.21  2004/01/19 05:40:05  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.20  2004/01/12 09:57:53  tfuruka1
 * 短辺綴じと長辺綴じに対応しました。それに合わせてVerison番号も変更しま
 * した。
 *
 * Revision 1.19  2004/01/11 11:07:35  tfuruka1
 * Version番号の変更のみ。バッファ名にリダイレクト記号が含まれていた場合
 * に作業ファイルの作成に失敗する問題を修正(dllmain.c)。
 *
 * Revision 1.18  2003/04/11 21:38:02  tfuruka1
 * ● Version番号の変更のみ --- 露見していなかったが、メモリ破壊を起こす
 *    可能性があった部分を修正 (dllmain.c) ---
 *
 * Revision 1.17  2003/03/29 12:56:15  tfuruka1
 * ● 公開用Versionの取得 ─── Version 2.2 ───
 *
 * Revision 1.16  2003/03/15 14:45:58  tfuruka1
 * ● プレビュー時の印刷設定に対応するための修正に伴う定数の追加とプロト
 *    タイプの修正を行った。
 *
 * Revision 1.15  2003/03/14 16:03:08  tfuruka1
 * ● PostScript関連の情報をMAILBOXからPRT_INFOへ移動した。将来、ak2prの
 *    コマンドオプションとして、指定可能にする為の修正である。
 *
 * Revision 1.14  2003/03/01 09:12:44  tfuruka1
 * ●リリース用として変更[Version]は[2.1]
 *
 * Revision 1.13  2003/02/25 15:34:40  tfuruka1
 * ●行番号出力制御処理を追加
 * ●Version を 2.1 へ変更。以前は1.7だったが、公開を2.0で行ってしまった
 *   ため、1.7から2.1になってしまった。
 *
 * Revision 1.12  2001/12/23 10:17:23  tfuruka1
 * ─────【Version 2.0にしました】─────
 * 一応、公開予定Versionにした。
 * ●Copyrightを印字しないモードを追加
 * ●作業ディレクトリ取得用の関数を追加（作業ファイル作成用の関数から切り
 *   出し）
 * ●このファイルには、現れないが・・・以下の修正を行った
 *     ●ドラッグ＆ドロップ機能の追加
 *     ●プレビューと印刷結果が異なる問題の修正
 *     ●ツールバーを追加
 *     ●細かいバグの修正
 *
 * Revision 1.11  2001/12/18 13:00:57  tfuruka1
 * デバッグ印刷のオプションを追加。
 *
 * Revision 1.10  2001/12/18 04:03:17  tfuruka1
 * ─────【Version 1.7にしました】─────
 * 取りあえず、プレビューできるようにしました。元々、プレビューをサポート
 * するつもりがなかったので、かなりソースが汚くなってしまいました。
 *
 * Revision 1.9  2001/12/08 15:23:48  tfuruka1
 * ─────【Version 1.6にしました】─────
 * メイン画面のLISTBOXをListViewへ変更しました。
 *
 * Revision 1.8  2001/12/07 18:27:28  tfuruka1
 * 用紙サイズの指定できるようにした。
 *
 * Revision 1.7  2001/10/01 13:24:06  tfuruka1
 * 【Version 1.5にしました】
 * 用紙の向きを指定出来るようにしました。
 *
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

#include "ver.h"
#define VERSION   "Version " MAKE_VERSION "($Name:  $)"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <mbstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <direct.h>

#include "resource.h"

#include "ak2pr.h"

#define IsShellTray() FindWindow("Shell_TrayWnd", NULL)

#define SV_DBGCLASS "ak2psのようなもの - ak2pr(Server-Detail)"
#define SV_CLASS    "ak2psのようなもの - ak2pr(Server)"
#define SV_CAPTION  "ak2pr Server"
#define PV_CAPTION  "ぷれびゅうううぅぅぅ" TIMESTAMP

#define SV_EXE_NAME "ak2prSV.EXE"

#define COPYRIGHT   "ak2pr " VERSION \
" Copyright 1997-2004 By T.Furukawa\n(tfuruka1@nifty.com)" TIMESTAMP

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
    BOOL valid;                                 // T:この構造体が有効
    CHAR szFileName[MAX_PATH];                  // ファイル名
    CHAR szTitle[256];                          // タイトル文字列
    int nNumOfUp;                               // 段組数
    int nTab;                                   // TAB幅
    int nType;                                  // 印刷タイプ
    int nOrientation;                           // 用紙の向き(0:デフォルト)
    short dmPaperSize;                          // 用紙サイズ(0:デフォルト)
    CHAR szTimeStamp[64];
    CHAR szTimeStamp1[64];
    int nBaseLine;                              // 0: Non, 1:E, 2:J, 9:A
    BOOL bNum;                                  // T: 行番号印刷
    BOOL bKeisen;                               // T: 罫線連結
    BOOL bNoRcvHeader;                          // Receivedヘッダを印字しない
    BOOL bColor;                                // T: Color印刷
    BOOL bPreView;                              // T: プレビュー
    BOOL bDebug;                                // T: デバッグ印刷
    BOOL bNoCopyright;                          // T: Copyrightを印刷しない
    BOOL bShortBinding;                         // T: 短編綴じ
    int nSingleLine;                            // T: 折り返さない
    double fFontSize;                           // フォントサイズ --- Point
    LOGFONT lfTHF;                              // 等幅フォント情報
    LOGFONT lfPPF;                         // プロポーショナルフォント
    LOGFONT lfOF;                              // その他のフォント情報
    LOGFONT lfOPPF;                             // その他のプロポーショナル
    // ----- 以下はPostScript印刷時の情報
    TCHAR szAcrobat[MAX_PATH];                  // Distiller in フォルダ
    TCHAR szGsPath[MAX_PATH];                   // GhostScript パス
    TCHAR szGsOpt[512];                         // GhostScript option

} PRT_INFO, *PPRT_INFO;

// nType に設定する値は以下の通り(クライアント側でも参照したいので、
// ak2pr.hに移動), デフォルトはPT_TEXT

// プレビュー用のデバイスコンテキスト情報等
typedef struct _PREVIEW_INFO {
    HDC hDC;                                    // デバイスコンテキスト
    HBITMAP hBitmap;                            // ビットマップ
    int wd, ht;                                 // サイズ
    int xoff, yoff;                             // 印刷不可エリア
    int dpiW, dpiH;                             // 解像度
    int status;                                 // 状態
    RECT rc;                                    // ウインドウの位置
} PREVIEW_INFO, *PPREVIEW_INFO;
enum {PVI_PRINT = 0,                            // 印刷
      PVI_SETUP,                                // 印刷設定後再プレビュー
      PVI_CANCEL                                // 破棄
};

// スレッド間メールボックス
typedef struct {
    HWND hWnd;                                  // ハンドル
    HDC hDC;                                    // デバイスハンドル(兼用)
    HDC hDCPrinter;                             // プリンタのDC
    HGLOBAL hDevMode;                           // プリンタデバイスモード
    HGLOBAL hDevNames;                          // プリンタデバイスネーム
    HANDLE hThread;                             // スレッドハンドル
    DWORD idThread;                             // スレッドID
    BOOL bRun;                                  // T:スレッド実行状態
    BOOL bStop;                                 // T:印刷停止状態
    PRT_INFO PrtInfo;                           // 印刷情報
    PREVIEW_INFO PrevInfo;                      // プレビュー情報
    // ----- 以下はメール印刷の情報
    TCHAR szFrom[128];                          // 差出人
    TCHAR szDate[128];                          // 受信日時
    TCHAR szReference[128];                     // リファレンス
    TCHAR szMessageID[128];                     // メッセージID
} MAILBOX, *PMAILBOX;

// フォント列挙情報
enum {FFI_FN_SETCOMBO = 0,                      // コンボボックスにセット
      FFI_FN_GETLOGFONT,                        // LOGFONTの取得
      NUMBER_OF_FFI_FN
};
typedef struct {
    int funcNumber;                             // 機能番号
    HWND hWnd;                                  // ウインドウハンドル
                                                // (FFI_FN_SETCOMBOの
                                                // 時に使用)
    LOGFONT lf;                                 // FFI_FN_GETLOGFONTの
                                                // 時に使用
} FONT_FUNK_INFO, *PFONT_FUNK_INFO;

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
VOID SetupPrtStyle(HWND hWnd, PPRT_INFO lppi);

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
    int nHeight,                                // フォントの高さ
    int nWeight,                                // フォントの太さ
    BOOL bItalic,                               // T: イタリック
    BOOL bUnderline,                            // T: 下線
    BOOL bStrikeOut,                            // T: 打ち消し線
    LPLOGFONT lplf                              // LOGFONTテンプレート
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
    UINT lfHeight,                              // フォントの高さ
    UINT lfWeight,                              // フォントのウエイト
    BOOL lfItalic,                              // T:イタリック
    BOOL lfUnderline,                           // T:下線
    BOOL lfStrikeout,                           // T:打ち消し線
    LPLOGFONT lplf                              // LOGFONTテンプレート
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
BOOL PrintPreview(HWND hWnd, PPREVIEW_INFO);
void SetPreViewPos(LPRECT lprc);
void GetPreViewPos(LPRECT lprc);

VOID WINAPI LsFontToCOMBO(HWND hWnd);
BOOL WINAPI GetLogFont(LPLOGFONT lplf);
/*--------------------------------------------------------------------
 * hDCPrinterで指定されてたデバイスコンテキストから、プレビュー用のデ
 * バイスコンテキストを作成する。プレビュー用のデバイスコンテキストは
 * 使用し終わった後に破棄する必要があります。また、デバイスコンテキス
 * トが所有しているビットマップも破棄する必要があります。
 * *-------------------------------------------------------------------*/
HDC WINAPI
MakePreviewInfo(HWND hWnd,                      // ウインドウハンドル
              HDC hDCPrinter,                   // プリンタDC
              PPREVIEW_INFO                     // プレビュー情報
    );
/* -------------------------------------------------------------------
 * 矩形を描画する
 * *-----------------------------------------------------------------*/
void WINAPI
DrawRect(
    HDC hDC,                                    // デバイスコンテキスト
    LPRECT lprc,                                // 矩形座標
    COLORREF rgb,                               // 色
    int pnStyle                                 // 線のスタイル
    );
/*-------------------------------------------------------------------- 
 * 作業用ディレクトリ名を得る。作業ディレクトリ名は書き換えてはいけま
 * せん。
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetTempDirectoryName();
#endif
