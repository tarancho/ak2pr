/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-08-19 01:01:12 tfuruka1>
 *
 * 「ak2psのようなもの」のQueue処理
 *
 * $Id: queue.c,v 1.2 2001/08/18 16:15:10 tfuruka1 Exp $
 * $Log: queue.c,v $
 * Revision 1.2  2001/08/18 16:15:10  tfuruka1
 * ●PRT_INFO構造体からbDeleteメンバを削除したので（常に一旦作業ファイル
 *   にコピーするようにしたので、このメンバは必要なくなった）、参照しない
 *   ように修正。
 * ●印刷Queueを削除した時に作業ファイルが削除されていなかったバグを修正
 *   する為に、DeleteQueue関数を新規追加した。
 *
 * Revision 1.1  2001/02/05 17:42:36  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"

static void
AdjustListWidth(HWND hWnd)
{
   HDC hDC;
   SIZE Size;
   int nMaxStrWd, i, j;
   TCHAR szBuf[1024];

    // 最も長い行のドット数を得る
    nMaxStrWd = 0;
    hDC = GetDC(hWnd);                          // ListBoxのDC
    j = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
    for (i = 0; i < j; i++) {
        SendMessage(hWnd, LB_GETTEXT, i, (LPARAM)szBuf);
        GetTextExtentPoint32(hDC, szBuf, strlen(szBuf), &Size);
        if (nMaxStrWd < Size.cx) {
            nMaxStrWd = Size.cx;
        }
    }
    ReleaseDC(hWnd, hDC);                       // DCの開放
    // リストボックスの幅を調整する
    SendMessage(hWnd, LB_SETHORIZONTALEXTENT, (WPARAM)nMaxStrWd + 16, 0);
}

BOOL
EnQueue(
    HWND hWnd,                                  // ハンドル(LIST BOX)
    PPRT_INFO pPrtInfo                          // 印刷情報
    )
{
    TCHAR szBuf[1024];
    SYSTEMTIME st;
    int nRet;

    // DLLのヴァージョンチェック
    if (0 != strcmp(pPrtInfo->szTimeStamp, pPrtInfo->szTimeStamp1) ||
        0 != strcmp(pPrtInfo->szTimeStamp, TIMESTAMP)) {
        strcpy(szBuf,  "DLLのヴァージョンが一致しません");
        MessageBox(hWnd, szBuf, __FILE__, MB_ICONSTOP);
        DbgPrint(NULL, 'E', "%s", szBuf);
        return FALSE;
    }

    GetLocalTime(&st);                          // 現在時刻の取得
    sprintf(szBuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d %s\t%s\t%d段組"
            "\t%fPoint\tTabStop=%d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
            pPrtInfo->szTitle, PT_TEXT == pPrtInfo->nType ? "Text" : "Mail",
            pPrtInfo->nNumOfUp, pPrtInfo->fFontSize, pPrtInfo->nTab);
    if (0 > (nRet = SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)szBuf))) {
        DbgPrint(hWnd, 'E', "LB_ADDSTRING ERROR (%d)", nRet);
        return FALSE;
    }

    if (LB_ERR == SendMessage(hWnd, LB_SETITEMDATA, nRet, (LPARAM)pPrtInfo)) {
        DbgPrint(hWnd, 'E', "LB_SETITEMDATA ERROR", nRet);
        SendMessage(hWnd, LB_DELETESTRING, nRet, 0);
        return FALSE;
    }

    AdjustListWidth(hWnd);

    DbgPrint(hWnd, 'I', "以下の通りQueueingしました\n"
             "印刷ファイル名: %s\n"
             "印刷タイトル  : %s\n"
             "段組数        : %d\n"
             "タブ幅        : %d\n"
             "印刷タイプ    : %s\n"
             "フォントサイズ: %f",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             PT_TEXT == pPrtInfo->nType ? "テキスト" :
             (PT_MAIL == pPrtInfo->nType ? "デコード済みメール" : 
              "未デコードメール"),
             pPrtInfo->fFontSize);

    return TRUE;
}

BOOL
DeQueue(
    HWND hWnd,                                  // ハンドル
    PPRT_INFO pPrtInfo                          // 印刷情報
    )
{
    PPRT_INFO pPrtInfoTmp;

    // Queueが空の場合は何もしない
    if (1 > SendMessage(hWnd, LB_GETCOUNT, 0, 0)) {
        return FALSE;
    }

    // 印刷情報を取り出す
    if (LB_ERR == (int)(pPrtInfoTmp = (PPRT_INFO)SendMessage(
        hWnd, LB_GETITEMDATA, 0, 0))) {
        DbgPrint(hWnd, 'E', "DeQueueに失敗しました");
        return FALSE;
    }

    if (LB_ERR == SendMessage(hWnd, LB_DELETESTRING, 0, 0)) {
        DbgPrint(hWnd, 'E', "DeQueueに失敗しました");
        free(pPrtInfoTmp);
        return FALSE;
    }

    AdjustListWidth(hWnd);

    // 印刷情報を複写する
    memcpy(pPrtInfo, pPrtInfoTmp, sizeof(PRT_INFO));
    free(pPrtInfoTmp);                          // メモリ開放

    DbgPrint(hWnd, 'I', "以下の通りDeQueueしました\n"
             "印刷ファイル名: %s\n"
             "印刷タイトル  : %s\n"
             "段組数        : %d\n"
             "タブ幅        : %d\n"
             "印刷タイプ    : %s\n"
             "フォントサイズ: %f",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             PT_TEXT == pPrtInfo->nType ? "テキスト" :
             (PT_MAIL == pPrtInfo->nType ? "デコード済みメール" : 
              "未デコードメール"),
             pPrtInfo->fFontSize);

    return TRUE;
}

int
DeleteQueue(
    HWND hWnd,                                  // ハンドル
    BOOL bForce                                 // T:全て削除
    )
{
    PPRT_INFO pPrtInfoTmp;
    int i;
    int cnt;

    cnt = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
    if (0 > cnt) {
        return 0;
    }
    for (i = 0, cnt--; cnt >= 0; cnt--) {
        if (bForce || SendMessage(hWnd, LB_GETSEL, cnt, 0)) {
            pPrtInfoTmp = (PPRT_INFO)SendMessage(hWnd, LB_GETITEMDATA, cnt, 0);
            SendMessage(hWnd, LB_DELETESTRING, cnt, 0);
            if (LB_ERR == (int)pPrtInfoTmp || !pPrtInfoTmp) {
                MessageBox(hWnd, "印刷データの削除に失敗しました",
                           "LB_GETITEMDATA",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                continue;
            }
            unlink(pPrtInfoTmp->szFileName);
            DbgPrint(NULL, 'I', "以下のデータを削除しました\n"
                     "印刷ファイル名: %s\n"
                     "印刷タイトル  : %s\n",
                     pPrtInfoTmp->szFileName,
                     pPrtInfoTmp->szTitle);
            free(pPrtInfoTmp);                  // メモリ開放
            i++;
        }
    }
    AdjustListWidth(hWnd);
    return i;                                   // 削除数を返却する
}
