/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2001-12-15 01:44:10 tfuruka1>
 *
 * 「ak2psのようなもの」のQueue処理
 *
 * $Id: queue.c,v 1.6 2001/12/14 17:03:27 tfuruka1 Exp $
 * $Log: queue.c,v $
 * Revision 1.6  2001/12/14 17:03:27  tfuruka1
 * プレビューの状態状態をEnQueue/DeQueueで（デバッグ）表示するようにした。
 *
 * Revision 1.5  2001/12/08 15:20:08  tfuruka1
 * メイン画面のLISTBOX→ListViewへ変更。
 *
 * Revision 1.4  2001/10/01 13:20:27  tfuruka1
 * 用紙の向きを指定出来るように修正。
 *
 * Revision 1.3  2001/08/19 04:39:07  tfuruka1
 * PostScriptファイルの暫定対応（ただ単にDistillerの監視フォルダに放り込
 * むだけ）。
 *
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

static LPCTSTR
GetOrientationStr(int nOrientation)
{
    static LPCTSTR lpStr[] = {
        "DEFAULT",
        "PORTRAIT",
        "LANDSCAPE"
    };

    return lpStr[nOrientation % 3];
}

static LPCTSTR
GetNTypeName(int nType)
{
    static struct {
        int nType;
        LPCTSTR lpszName;
    } obj[] = {
        {PT_TEXT, "Plain"},
        {PT_MAIL, "Mail"},
        {PT_PS_ACROBAT, "PostScript(ACROBAT)"},
        {PT_PS_GHOST, "PostScript(GhostScript)"},
        {NUM_OF_PT, NULL}
    };
    int i;

    for (i = 0; obj[i].lpszName; i++) {
        if (nType == obj[i].nType) {
            return obj[i].lpszName;
        }
    }
    return (LPCTSTR)"不明";
}

BOOL
EnQueue(
    HWND hWnd,                                  // ハンドル(LIST BOX)
    PPRT_INFO pPrtInfo                          // 印刷情報
    )
{
    TCHAR szBuf[1024];
    SYSTEMTIME st;
    LV_ITEM item;
    
    // DLLのヴァージョンチェック
    if (0 != strcmp(pPrtInfo->szTimeStamp, pPrtInfo->szTimeStamp1) ||
        0 != strcmp(pPrtInfo->szTimeStamp, TIMESTAMP)) {
        strcpy(szBuf,  "DLLのヴァージョンが一致しません");
        MessageBox(hWnd, szBuf, __FILE__, MB_ICONSTOP);
        DbgPrint(NULL, 'E', "%s", szBuf);
        return FALSE;
    }

    GetLocalTime(&st);                          // 現在時刻の取得

    sprintf(szBuf, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.pszText = szBuf;
    item.iItem = ListView_GetItemCount(hWnd);
    item.iSubItem = 0;
    item.lParam = (LPARAM)pPrtInfo;
    ListView_InsertItem(hWnd, &item);

    sprintf(szBuf, "%d", pPrtInfo->nNumOfUp);
    item.mask = LVIF_TEXT;
    item.pszText = szBuf;
    item.iSubItem = 1;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%f", pPrtInfo->fFontSize);
    item.pszText = szBuf;
    item.iSubItem = 2;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%d", pPrtInfo->nTab);
    item.pszText = szBuf;
    item.iSubItem = 3;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetOrientationStr(pPrtInfo->nOrientation));
    item.pszText = szBuf;
    item.iSubItem = 4;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetPaperSizeComment(pPrtInfo->dmPaperSize));
    item.pszText = szBuf;
    item.iSubItem = 5;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", GetNTypeName(pPrtInfo->nType));
    item.pszText = szBuf;
    item.iSubItem = 6;
    ListView_SetItem(hWnd, &item);

    sprintf(szBuf, "%s", pPrtInfo->szTitle);
    item.pszText = szBuf;
    item.iSubItem = 7;
    ListView_SetItem(hWnd, &item);

    DbgPrint(hWnd, 'I', "以下の通りQueueingしました\n"
             "印刷ファイル名: %s\n"
             "印刷タイトル  : %s\n"
             "段組数        : %d\n"
             "タブ幅        : %d\n"
             "印刷タイプ    : %s\n"
             "フォントサイズ: %f\n"
             "プレビュー    : %d\n"
             "アドレス      : %x",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize,
             pPrtInfo->bPreView,
             pPrtInfo);

    return TRUE;
}

BOOL
DeQueue(
    HWND hWnd,                                  // ハンドル
    PPRT_INFO pPrtInfo                          // 印刷情報
    )
{
    PPRT_INFO pPrtInfoTmp;
    LVITEM item;
    TCHAR szBuf[1024];

    // Queueが空の場合は何もしない
    if (1 > ListView_GetItemCount(hWnd)) {
        return FALSE;
    }

    // 印刷情報を取り出す
    item.mask = LVIF_PARAM | LVIF_TEXT;
    item.iItem = 0;
    item.iSubItem = 0;    
    item.pszText = szBuf;
    item.cchTextMax = 1020;
    if (!ListView_GetItem(hWnd, &item)) {
        DbgPrint(hWnd, 'E', "DeQueueに失敗しました");
        return FALSE;
    }
    pPrtInfoTmp = (PPRT_INFO)item.lParam;
    DbgPrint(hWnd, 'D', "DeQueued: %x", pPrtInfoTmp);

    // 印刷情報を複写する
    memcpy(pPrtInfo, pPrtInfoTmp, sizeof(PRT_INFO));

    if (!ListView_DeleteItem(hWnd, 0)) {
        DbgPrint(hWnd, 'E', "DeQueueに失敗しました");
        free(pPrtInfoTmp);
        return FALSE;
    }   
    free(pPrtInfoTmp);                          // メモリ開放

    DbgPrint(hWnd, 'I', "以下の通りDeQueueしました\n"
             "印刷ファイル名: %s\n"
             "印刷タイトル  : %s\n"
             "段組数        : %d\n"
             "タブ幅        : %d\n"
             "印刷タイプ    : %s\n"
             "フォントサイズ: %f\n"
             "プレビュー    : %d",
             pPrtInfo->szFileName,
             pPrtInfo->szTitle,
             pPrtInfo->nNumOfUp,
             pPrtInfo->nTab,
             GetNTypeName(pPrtInfo->nType),
             pPrtInfo->fFontSize,
             pPrtInfo->bPreView);

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
    LVITEM item;

    cnt = ListView_GetItemCount(hWnd);		

    if (0 > cnt) {
        return 0;
    }
    for (i = 0, cnt--; cnt >= 0; cnt--) {
        if (bForce || LVIS_SELECTED == ListView_GetItemState(
            hWnd, cnt, LVIS_SELECTED)) {
            item.mask = LVIF_PARAM;
            item.iItem = cnt;
            item.iSubItem = 0;
            if (!ListView_GetItem(hWnd, &item)) {
                MessageBox(hWnd, "印刷データの削除に失敗しました",
                           "GetItem",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                continue;
            }

            pPrtInfoTmp = (PPRT_INFO)item.lParam;
            if (!ListView_DeleteItem(hWnd, cnt)) {
                MessageBox(hWnd, "印刷データの削除に失敗しました",
                           "DeleteItem",
                           MB_SETFOREGROUND | MB_ICONSTOP);
                unlink(pPrtInfoTmp->szFileName);
                free(pPrtInfoTmp);
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
    return i;                                   // 削除数を返却する
}
