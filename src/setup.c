/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-08-21 08:30:49 tfuruka1>
 *
 * 「ak2psのようなもの」の印字設定
 *
 * 覚書
 *
 * プロパティシートの各々のダイアログは、*タブ選択*したときに
 * INIT_DIALOG のイベントが発生する。当然WM_CLOSEは、INIT_DIALOGが走っ
 * ていない場合は、実行されない。この事を知らなかったので、不可解な現
 * 象が発生していました（INIT_DIALOGが常に発生していると思っていた）。
 *
 * $Id: setup.c,v 1.15 2004/08/21 01:01:01 tfuruka1 Exp $
 * $Log: setup.c,v $
 * Revision 1.15  2004/08/21 01:01:01  tfuruka1
 * テキスト印刷に於いて「行間」と「罫線連結」が有効になるようにしました。
 *
 * Revision 1.14  2004/06/18 00:57:56  tfuruka1
 * 改行コードの修正のみです。
 *
 * Revision 1.13  2004/01/19 05:38:35  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.12  2004/01/12 10:00:05  tfuruka1
 * 長辺綴じと短辺綴じに対応した事により、チェックボックスが増えた事による
 * 対応。
 *
 * Revision 1.11  2003/03/29 12:49:31  tfuruka1
 * ● セットアップダイアログに「個別設定」のタブを追加しました。言葉で書
 *    くとこれだけですけど、結構修正してます。
 *
 * Revision 1.10  2003/03/15 14:43:43  tfuruka1
 * ● 印刷設定を情報を格納するエリアを指定できるように変更した。プレビュー
 *    時の印刷設定に対応する為の修正である。
 *
 * Revision 1.9  2003/03/14 15:29:49  tfuruka1
 * ● PostScript関連のパラメータもPRT_INFOに持つように変更した。これによっ
 *    て、ak2prのコマンドラインからGhostScriptのパス等も指定が可能になる
 *    （将来対応予定）。
 *
 * Revision 1.8  2003/03/01 09:08:25  tfuruka1
 * ●ページ設定のe-mailタブの内容がクリア（特定の手順で操作を行った場合）
 *   される場合があったので、修正した。→プロパティシートのダイアログは、
 *   タブを選択状態にしないと、INITDIALOGが発生しないことを知らなかった事
 *   が起因。
 * ●ページ設定のPostScriptタブの内容がキャンセルを選択しても反映されてし
 *   まう問題を修正した。
 *
 * Revision 1.7  2001/12/23 10:13:56  tfuruka1
 * ●設定ダイアログが、二つ以上表示される（できる）問題を修正した。
 * ●Copyright表示を印字しないモードをサポートした。
 *
 * Revision 1.6  2001/12/18 12:59:04  tfuruka1
 * デバッグ印刷のオプションを追加。
 *
 * Revision 1.5  2001/12/14 17:09:17  tfuruka1
 * プレビュー対応
 *
 * Revision 1.4  2001/08/19 08:54:55  tfuruka1
 * PostScriptファイルを印刷するときにGhostScriptを呼び出せるようにした。
 *
 * Revision 1.3  2001/08/19 04:33:53  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
 * Revision 1.2  2001/02/05 17:03:33  tfuruka1
 * プリンタの設定を、メニューからプロパティシートの「共通」へ移動した。
 *
 * Revision 1.1  2001/02/05 16:58:52  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

#define SETUP_CAPTION "印刷スタイルの設定"
#define GS_DEF_OPT "-dNOPAUSE -dBATCH -sDEVICE=mswinpr2"

PRT_INFO g_PrtInfo;                             // デフォルト印刷情報
static PRT_INFO PrtInfoTmp;                     // 印刷情報作業用

static LPTSTR WINAPI
GetOpenFileNameWrap(
    HWND hWnd,
    LPCTSTR lpszFilter,
    LPCTSTR lpszTitle,
    LPTSTR lpszFileName
    )
{
    OPENFILENAME of;

    of.lStructSize       = sizeof(OPENFILENAME);
    of.hwndOwner         = hWnd;
    of.hInstance         = NULL;
    of.lpstrFilter = lpszFilter;
    of.lpstrCustomFilter = NULL;
    of.nMaxCustFilter    = 0;
    of.nFilterIndex      = 0;
    of.lpstrFile         = lpszFileName;
    of.nMaxFile          = MAX_PATH;
    of.lpstrFileTitle    = NULL;
    of.nMaxFileTitle     = 0;
    of.lpstrInitialDir   = NULL;
    of.lpstrTitle        = lpszTitle;
    of.Flags             = OFN_HIDEREADONLY;
    of.nFileOffset       = 0;
    of.nFileExtension    = 0;
    of.lpstrDefExt       = NULL;
    of.lCustData         = 0;
    of.lpfnHook          = NULL;
    of.lpTemplateName    = NULL;

    if (!GetOpenFileName(&of)) return NULL;
    return lpszFileName;
}

//
// ━━━━━━以下は共通設定のコールバック集です━━━━━━━━━━
//
static BOOL CALLBACK
DoInitDialogCom(
    HWND hWnd,                                  // ハンドル
    HWND hWndFocus,                             // フォーカスハンドル
    LPARAM lParam                               // 初期化パラメータ
    )
{
    TCHAR szBuf[64];

    // タブが選択されないと、INIT_DIALOGが走らない→走らないらしい
    DbgPrint(NULL, 'D', "InitDialog(共通)");

    // タブ幅のセット
    sprintf(szBuf, "%d", PrtInfoTmp.nTab);
    SetDlgItemText(hWnd, IDC_TABSTOP, szBuf);
    // フォントサイズのセット
    sprintf(szBuf, "%f", PrtInfoTmp.fFontSize);
    SetDlgItemText(hWnd, IDC_FONTSIZE, szBuf);
    // 段組数のセット
    sprintf(szBuf, "%d", PrtInfoTmp.nNumOfUp);
    SetDlgItemText(hWnd, IDC_NUP, szBuf);

    // プレビューの実行有無
    CheckDlgButton(hWnd, IDC_CHK_PREVIEW, PrtInfoTmp.bPreView ? TRUE : FALSE);

    // デバッグ印刷の実行有無
    CheckDlgButton(hWnd, IDC_C_DEBUG, PrtInfoTmp.bDebug ? TRUE : FALSE);

    // Copyright情報の印刷有無
    CheckDlgButton(hWnd, IDC_CHK_NOCOPYRIGHT,
                   PrtInfoTmp.bNoCopyright ? TRUE : FALSE);

    // 短辺綴じの有無
    CheckDlgButton(hWnd, IDC_CHK_SHORT_BINDING,
                   PrtInfoTmp.bShortBinding ? TRUE : FALSE);

    // フォント選択コンボボックスの初期化
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_THF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_PPF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_OF));
    LsFontToCOMBO(GetDlgItem(hWnd, IDC_COMBO_OPPF));

    // デフォルトフォントの選択
    SendDlgItemMessage(hWnd, IDC_COMBO_THF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfTHF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_PPF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfPPF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_OF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfOF.lfFaceName);
    SendDlgItemMessage(hWnd, IDC_COMBO_OPPF, CB_SELECTSTRING, -1, 
                       (LPARAM)PrtInfoTmp.lfOPPF.lfFaceName);

    return TRUE;
}

static VOID
DoCommandCom(
    HWND hWnd,                                  // ハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールのハンドル
    UINT codeNotify                             // 通知コード
    )
{
    TCHAR szBuf[64];
    int i;
    double d;

    switch (id) {                               // コントロール番号
    case IDC_TABSTOP:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            i = (int)strtoul(szBuf, NULL, 10);
            if (0 >= i || 32 < i) {
                i = 8;
            }
            sprintf(szBuf, "%d", i);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.nTab = i;
        }
        break;
    case IDC_FONTSIZE:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            d = strtod(szBuf, NULL);
            if (2 >= d || 48 < d) {
                d = 10.0;
            }
            sprintf(szBuf, "%f", d);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.fFontSize = d;
        }
        break;
    case IDC_NUP:
        if (EN_KILLFOCUS == codeNotify) {
            GetDlgItemText(hWnd, id, szBuf, 10);
            i = (int)strtoul(szBuf, NULL, 10);
            if (0 >= i || 10 < i) {
                i = 1;
            }
            sprintf(szBuf, "%d", i);
            SetDlgItemText(hWnd, id, szBuf);
            PrtInfoTmp.nNumOfUp = i;
        }
        break;
    case IDC_CHK_PREVIEW:
        PrtInfoTmp.bPreView = IsDlgButtonChecked(hWnd, IDC_CHK_PREVIEW);
        break;
    case IDC_CHK_SHORT_BINDING:
        PrtInfoTmp.bShortBinding = IsDlgButtonChecked(hWnd,
                                                      IDC_CHK_SHORT_BINDING);
        break;
    case IDC_CHK_NOCOPYRIGHT:
        PrtInfoTmp.bNoCopyright
            = IsDlgButtonChecked(hWnd, IDC_CHK_NOCOPYRIGHT);
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.nTab = 8;
        PrtInfoTmp.nNumOfUp = 2;
        PrtInfoTmp.fFontSize = 10.0;

        sprintf(szBuf, "%d", PrtInfoTmp.nTab);
        SetDlgItemText(hWnd, IDC_TABSTOP, szBuf);

        sprintf(szBuf, "%d", PrtInfoTmp.nNumOfUp);
        SetDlgItemText(hWnd, IDC_NUP, szBuf);

        sprintf(szBuf, "%f", PrtInfoTmp.fFontSize);
        SetDlgItemText(hWnd, IDC_FONTSIZE, szBuf);

        PrtInfoTmp.bPreView = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_PREVIEW, PrtInfoTmp.bPreView);

        PrtInfoTmp.bShortBinding = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_SHORT_BINDING, PrtInfoTmp.bShortBinding);

        PrtInfoTmp.bNoCopyright = FALSE;
        CheckDlgButton(hWnd, IDC_CHK_NOCOPYRIGHT, PrtInfoTmp.bNoCopyright);

        // フォントの選択
        strcpy(PrtInfoTmp.lfTHF.lfFaceName, FN_MSM);
        strcpy(PrtInfoTmp.lfPPF.lfFaceName, FN_MSPG);
        strcpy(PrtInfoTmp.lfOF.lfFaceName, FN_COU);
        strcpy(PrtInfoTmp.lfOPPF.lfFaceName, FN_ARIAL);
        SendDlgItemMessage(hWnd, IDC_COMBO_THF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfTHF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_PPF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfPPF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_OF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfOF.lfFaceName);
        SendDlgItemMessage(hWnd, IDC_COMBO_OPPF, CB_SELECTSTRING, -1, 
                           (LPARAM)PrtInfoTmp.lfOPPF.lfFaceName);
        break;
    case IDC_PRINTER:
        SetupPrinter(hWnd, &g_MailBox.hDevNames, &g_MailBox.hDevMode);
        break;
    }
}

static VOID
DoCloseCom(HWND hWnd)
{
    TCHAR szBuf[64];
    int i;
    double d;

    DbgPrint(NULL, 'D', "Close(共通)");

    GetDlgItemText(hWnd, IDC_TABSTOP, szBuf, 10);
    i = (int)strtoul(szBuf, NULL, 10);
    if (0 >= i || 32 < i) {
        i = 8;
    }
    PrtInfoTmp.nTab = i;

    GetDlgItemText(hWnd, IDC_FONTSIZE, szBuf, 10);
    d = strtod(szBuf, NULL);
    if (2 >= d || 48 < d) {
        d = 10.0;
    }
    PrtInfoTmp.fFontSize = d;

    GetDlgItemText(hWnd, IDC_NUP, szBuf, 10);
    i = (int)strtoul(szBuf, NULL, 10);
    if (0 >= i || 10 < i) {
        i = 1;
    }
    PrtInfoTmp.nNumOfUp = i;

    PrtInfoTmp.bPreView = IsDlgButtonChecked(hWnd, IDC_CHK_PREVIEW);
    PrtInfoTmp.bDebug = IsDlgButtonChecked(hWnd, IDC_C_DEBUG);
    PrtInfoTmp.bNoCopyright = IsDlgButtonChecked(hWnd, IDC_CHK_NOCOPYRIGHT);
    PrtInfoTmp.bShortBinding = IsDlgButtonChecked(hWnd, IDC_CHK_SHORT_BINDING);

    // フォント
    GetDlgItemText(hWnd, IDC_COMBO_THF, PrtInfoTmp.lfTHF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_PPF, PrtInfoTmp.lfPPF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_OF, PrtInfoTmp.lfOF.lfFaceName,
                   LF_FACESIZE);
    GetDlgItemText(hWnd, IDC_COMBO_OPPF, PrtInfoTmp.lfOPPF.lfFaceName,
                   LF_FACESIZE);

    DbgPrint(NULL, 'I', "共通設定終了\nTABSTOP:%d\nFONTSIZE:%f\n"
             "NUP:%d\nPREVIE:%d\nCOPYRIGHT:%d\nSHORTBIND:%d\n"
             "TH=%s,PP=%s,OTHER=%s, OTHER(PP)=",
             PrtInfoTmp.nTab, PrtInfoTmp.fFontSize, PrtInfoTmp.nNumOfUp,
             PrtInfoTmp.bPreView, PrtInfoTmp.bNoCopyright,
             PrtInfoTmp.bShortBinding,
             PrtInfoTmp.lfTHF.lfFaceName,
             PrtInfoTmp.lfPPF.lfFaceName,
             PrtInfoTmp.lfOF.lfFaceName,
             PrtInfoTmp.lfOPPF.lfFaceName);
}

static BOOL
DialogProcCom(
    HWND hWnd,                                  // ダイアログボックスのハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // 第1メッセージ パラメータ
    LPARAM lParam                               // 第2メッセージ パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogCom);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandCom);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseCom);
    }
    return FALSE;    
}

//
// ━━━━━━以下はメール印刷のコールバック集です━━━━━━━━━━
//
static BOOL CALLBACK
DoInitDialogMail(
    HWND hWnd,                                  // ハンドル
    HWND hWndFocus,                             // フォーカスハンドル
    LPARAM lParam                               // 初期化パラメータ
    )
{
    DbgPrint(NULL, 'D', "InitDialog(e-mail)");

    // カラー印刷のチェック
    CheckDlgButton(hWnd, IDC_C_COLOR, PrtInfoTmp.bColor ? TRUE : FALSE);
    // Receivedヘッダの印字チェック
    CheckDlgButton(hWnd, IDC_C_NORHEAD,
                   PrtInfoTmp.bNoRcvHeader ? TRUE : FALSE);

    return TRUE;
}

static VOID
DoCommandMail(
    HWND hWnd,                                  // ハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールのハンドル
    UINT codeNotify                             // 通知コード
    )
{
    switch (id) {                               // コントロール番号
    case IDC_C_COLOR:
        // Color印刷の指定
        PrtInfoTmp.bColor = IsDlgButtonChecked(hWnd, IDC_C_COLOR);
        break;
    case IDC_C_NORHEAD:
        // Receivedヘッダの印字指定
        PrtInfoTmp.bNoRcvHeader = IsDlgButtonChecked(hWnd, IDC_C_NORHEAD);
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.bColor = TRUE;
        PrtInfoTmp.bNoRcvHeader = TRUE;
        CheckDlgButton(hWnd, IDC_C_COLOR, PrtInfoTmp.bColor);
        CheckDlgButton(hWnd, IDC_C_NORHEAD, PrtInfoTmp.bNoRcvHeader);
        break;
    }
}

static void
DoCloseMail(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(e-mail)");

    // Color印刷の指定
    PrtInfoTmp.bColor = IsDlgButtonChecked(hWnd, IDC_C_COLOR);
    // Receivedヘッダの印字指定
    PrtInfoTmp.bNoRcvHeader = IsDlgButtonChecked(hWnd, IDC_C_NORHEAD);
}

static BOOL
DialogProcMail(
    HWND hWnd,                                  // ダイアログボックスのハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // 第1メッセージ パラメータ
    LPARAM lParam                               // 第2メッセージ パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogMail);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandMail);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseMail);
    }
    return FALSE;    
}
//
// ━━━━━━以下はテキスト印刷のコールバック集です━━━━━━━━━━
//
static BOOL CALLBACK
DoInitDialogText(
    HWND hWnd,                                  // ハンドル
    HWND hWndFocus,                             // フォーカスハンドル
    LPARAM lParam                               // 初期化パラメータ
    )
{
    DbgPrint(NULL, 'D', "InitDialog(Text)");

    // 罫線連結のチェック
    CheckDlgButton(hWnd, IDC_C_KEISEN, PrtInfoTmp.bKeisen ? TRUE : FALSE);
    // 行番号のチェック
    CheckDlgButton(hWnd, IDC_C_NUM, PrtInfoTmp.bNum ? TRUE : FALSE);
    // ベースライン
    CheckRadioButton(hWnd, IDC_R_JAPAN, IDC_R_AUTO, PrtInfoTmp.nBaseLine);

    return TRUE;
}

static VOID
DoCommandText(
    HWND hWnd,                                  // ハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールのハンドル
    UINT codeNotify                             // 通知コード
    )
{
    switch (id) {                               // コントロール番号
    case IDC_R_NOINTERLINE:
    case IDC_R_ENGLISH:
    case IDC_R_JAPAN:
    case IDC_R_AUTO:
        PrtInfoTmp.nBaseLine = id;
        break;
    case IDC_DEFAULT:
        PrtInfoTmp.bNum = TRUE;
        PrtInfoTmp.nBaseLine = IDC_R_ENGLISH;
        PrtInfoTmp.bKeisen = FALSE;

        CheckDlgButton(hWnd, IDC_C_KEISEN, FALSE);
        CheckDlgButton(hWnd, IDC_C_NUM, TRUE);
        CheckDlgButton(hWnd, IDC_R_ENGLISH, TRUE);
        // ベースライン
        CheckRadioButton(hWnd, IDC_R_JAPAN, IDC_R_AUTO, PrtInfoTmp.nBaseLine);
        break;
    }
}

static void
DoCloseText(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(text)");

    // 罫線連結
    PrtInfoTmp.bKeisen = IsDlgButtonChecked(hWnd, IDC_C_KEISEN);
    // 行番号
    PrtInfoTmp.bNum = IsDlgButtonChecked(hWnd, IDC_C_NUM);

    // ベースラインスキップ
    if (IsDlgButtonChecked(hWnd, IDC_R_NOINTERLINE)) {
        PrtInfoTmp.nBaseLine = IDC_R_NOINTERLINE;
    } else if (IsDlgButtonChecked(hWnd, IDC_R_ENGLISH)) {
        PrtInfoTmp.nBaseLine = IDC_R_ENGLISH;
    } else if (IsDlgButtonChecked(hWnd, IDC_R_JAPAN)) {
        PrtInfoTmp.nBaseLine = IDC_R_JAPAN;
    } else {
        PrtInfoTmp.nBaseLine = IDC_R_AUTO;
    }
}

static BOOL
DialogProcText(
    HWND hWnd,                                  // ダイアログボックスのハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // 第1メッセージ パラメータ
    LPARAM lParam                               // 第2メッセージ パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogText);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandText);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseText);
    }
    return FALSE;    
}

//
// ━━━━━━以下はPostScriptのコールバック集です━━━━━━━━━━
//
static BOOL CALLBACK
DoInitDialogPs(
    HWND hWnd,                                  // ハンドル
    HWND hWndFocus,                             // フォーカスハンドル
    LPARAM lParam                               // 初期化パラメータ
    )
{
    DbgPrint(NULL, 'D', "InitDialog(PS)");

    SetDlgItemText(hWnd, IDC_ED_ACRIN, PrtInfoTmp.szAcrobat);
    SetDlgItemText(hWnd, IDC_ED_GHOST, PrtInfoTmp.szGsPath);
    SetDlgItemText(hWnd, IDC_ED_GSOP, PrtInfoTmp.szGsOpt);
    return TRUE;
}

static VOID
DoCommandPs(
    HWND hWnd,                                  // ハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールのハンドル
    UINT codeNotify                             // 通知コード
    )
{
    switch (id) {                               // コントロール番号
    case IDC_BT_GSOP:
        SetDlgItemText(hWnd, IDC_ED_GSOP, GS_DEF_OPT);
        break;
    }
}

static void
DoClosePs(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(PS)");

    GetDlgItemText(hWnd, IDC_ED_ACRIN, PrtInfoTmp.szAcrobat, MAX_PATH);
    GetDlgItemText(hWnd, IDC_ED_GHOST, PrtInfoTmp.szGsPath, MAX_PATH);
    GetDlgItemText(hWnd, IDC_ED_GSOP, PrtInfoTmp.szGsOpt, 512);
}

static BOOL
DialogProcPs(
    HWND hWnd,                                  // ダイアログボックスのハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // 第1メッセージ パラメータ
    LPARAM lParam                               // 第2メッセージ パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogPs);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandPs);
        HANDLE_MSG(hWnd, WM_DESTROY, DoClosePs);
    }
    return FALSE;    
}

//
// ━━━━━━以下は個別設定のコールバック集です━━━━━━━━━━
//
static BOOL CALLBACK
DoInitDialogKbt(
    HWND hWnd,                                  // ハンドル
    HWND hWndFocus,                             // フォーカスハンドル
    LPARAM lParam                               // 初期化パラメータ
    )
{
    DbgPrint(NULL, 'D', "InitDialog(個別)");

    // タイトルの設定
    SetDlgItemText(hWnd, IDC_ED_TITLE, PrtInfoTmp.szTitle);
    return TRUE;
}

static VOID
DoCommandKbt(
    HWND hWnd,                                  // ハンドル
    int id,                                     // コントロールID
    HWND hWndCtl,                               // コントロールのハンドル
    UINT codeNotify                             // 通知コード
    )
{
    // 現在は特に処理なし
    switch (id) {                               // コントロール番号
    default:
        break;
    }
}

static void
DoCloseKbt(HWND hWnd)
{
    DbgPrint(NULL, 'D', "Close(個別)");

    GetDlgItemText(hWnd, IDC_ED_TITLE, PrtInfoTmp.szTitle, 255);
}

static BOOL
DialogProcKbt(
    HWND hWnd,                                  // ダイアログボックスのハンドル
    UINT uMsg,                                  // メッセージ
    WPARAM wParam,                              // 第1メッセージ パラメータ
    LPARAM lParam                               // 第2メッセージ パラメータ
    )
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DoInitDialogKbt);
        HANDLE_MSG(hWnd, WM_COMMAND, DoCommandKbt);
        HANDLE_MSG(hWnd, WM_DESTROY, DoCloseKbt);
    }
    return FALSE;    
}

// ━━━━━━━━━━━━━━━━━━━━━━━━
// ━━━━━━ セットアップダイアログ ━━━━━━
// ━━━━━━━━━━━━━━━━━━━━━━━━
VOID
SetupPrtStyle(HWND hWnd, PPRT_INFO lppiArea)
{
    PROPSHEETPAGE psp[6];
    PROPSHEETHEADER psh;
    int i;
    int iNumOfDlg;                              // ダイアログの数
    static BOOL running = FALSE;

    // 現在セットアップウインドウを表示中の場合は何もしない。厳密には
    // クリティカルセクションが必要ですが、よっぽどの事が無い限りは大
    // 丈夫でしょう。仮にダイアログが二つ表示されたところで、大した害
    // はないので、フラグでごまかします。
    if (running) {
        return;
    }

    running = TRUE;

    // ダイアログの数を設定。デフォルト設定は4、個別の場合は5
    iNumOfDlg = (&g_PrtInfo == lppiArea) ? 4 : 5;

    for (i = 0; i < iNumOfDlg; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE | PSH_USEHICON;
        psp[i].hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
        psp[i].lParam = i;
    }

    i = 0;
    psp[i].hIcon = LoadIcon(NULL, IDI_APPLICATION);
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_SETUP);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcCom;
    psp[i].pszTitle = "共通設定";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_MAIL));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_MAIL);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcMail;
    psp[i].pszTitle = "e-mail印刷";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_TEXT));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_TEXT);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcText;
    psp[i].pszTitle = "テキスト印刷";

    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_PS));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_PS);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcPs;
    psp[i].pszTitle = "PostScript";

    // 以下の設定は、個別設定の時のみ有効になるが、設定は常に行う。デ
    // フォルト設定の場合は表示されない。
    i++;
    psp[i].hIcon = LoadIcon(psp[i].hInstance, MAKEINTRESOURCE(IDI_KOBETSU));
    psp[i].pszTemplate = MAKEINTRESOURCE(IDD_KOBETSU);
    psp[i].pfnDlgProc = (DLGPROC)DialogProcKbt;
    psp[i].pszTitle = "個別設定";

    memset(&psh, 0, sizeof(PROPSHEETHEADER));
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_USEHICON | PSH_PROPSHEETPAGE | PSH_PROPTITLE | 
        PSH_NOAPPLYNOW;
    psh.hwndParent = hWnd;
    psh.hInstance = psp[0].hInstance;
    psh.pszCaption = (LPSTR)SETUP_CAPTION;
    psh.hIcon = LoadIcon(NULL, IDI_ASTERISK);
    psh.ppsp = (LPCPROPSHEETPAGE)&psp;
    psh.nPages = iNumOfDlg;

    // 現在の内容を複写
    memcpy(&PrtInfoTmp, lppiArea, sizeof(PRT_INFO));

    i = PropertySheet(&psh);

    running = FALSE;

    if (1 != i) {
        return;
    }

    // 設定結果を反映
    memcpy(lppiArea, &PrtInfoTmp, sizeof(PRT_INFO));

    // 設定したエリアがデフォルトオプションエリアと同じ場合
    if (&g_PrtInfo == lppiArea) {
        // 初期化ファイルへも反映
        SetDefaultPrtInfo();
    }
}
