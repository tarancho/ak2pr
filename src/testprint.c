/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-01-18 19:33:37 tfuruka1>
 *
 * ak2psのようなもののテスト印字関連
 *
 * $Id: testprint.c,v 1.4 2004/01/19 05:39:09 tfuruka1 Exp $
 * $Log: testprint.c,v $
 * Revision 1.4  2004/01/19 05:39:09  tfuruka1
 * フォント情報を指定出来るようになった事に関する修正を行いました。
 *
 * Revision 1.3  2001/12/23 10:21:43  tfuruka1
 * ●プレビューモードの判断方法を修正
 *
 * Revision 1.2  2001/12/18 04:01:00  tfuruka1
 * プレビューを表示できるように修正
 *
 * Revision 1.1  2001/02/05 17:42:12  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#include "ak2prs.h"
#include "ak2pr.h"

/* -------------------------------------------------------------------
 * クロスを描画する。(x, y)を中心として半径rの円の中にクロスを描画する。
 * デバイスコンテキストは呼び出し側で用意し, ペンがセレクトされている
 * 状態である事。
 * *-----------------------------------------------------------------*/
static void DrawCross(
    HDC hDC,                                    // デバイスコンテキスト
    int x,
    int y,
    int xr,                                     // 水平方向の半径
    int yr                                      // 垂直方向の半径
    )
{ 
    POINT pt[2];

    Arc(g_MailBox.hDC, x - xr, y - yr, x + xr, y + yr, x + xr, y, x + xr, y);

    pt[0].x = pt[1].x = x;
    pt[0].y = y + yr;
    pt[1].y = y - yr;
    Polyline(g_MailBox.hDC, &pt[0], 2);

    pt[0].y = pt[1].y = y;
    pt[0].x = x + xr;
    pt[1].x = x - xr;
    Polyline(g_MailBox.hDC, &pt[0], 2);
}

/* -------------------------------------------------------------------
 * トンボを描画する。用紙の1inchi内側にトンボを切る
 * *-----------------------------------------------------------------*/
void DrawTombow(void)
{
    HPEN hPen, hOldPen;                         // ペン
    POINT pt[4];
    int x, y, i, nWd;                           // 汎用
    int nPaperWidth = GetPrtWd();
    int nPaperHeight = GetPrtHt();
    int nDPIW = GetPrtDpiW();
    int nDPIH = GetPrtDpiH();
    int nPaperMarginW = GetPrtMgW();
    int nPaperMarginH = GetPrtMgH();

    hPen = CreatePrtPen(PS_SOLID, ConvX2Dt(0.005, nDPIW, CX_CM), RGB(0, 0, 0));
    hOldPen = SelectObject(g_MailBox.hDC, hPen);
    // ------------------------------
    // 上と下の中央のトンボを描画する
    // ------------------------------
    pt[0].x = pt[1].x = GetPrtCenter(GPC_W);
    pt[0].y = GetPrtDpiH() - GetPrtMgH();
    pt[1].y = pt[0].y - ConvX2Dt(2, GetPrtDpiH(),CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 上の縦線

    pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[1].y = pt[0].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 上の縦線

    pt[0].x = nPaperWidth / 2 - nDPIW / 2 - nPaperMarginW;
    pt[1].x = pt[0].x + nDPIW;
    pt[0].y = pt[1].y = nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 上の横線
    
    pt[0].y = pt[1].y = nPaperHeight - nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 下の横線

    // オフセット無しのトンボの位置
    DrawCross(g_MailBox.hDC, nPaperWidth / 2, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    DrawCross(g_MailBox.hDC, nPaperWidth / 2, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // ----------------------
    // 左右のトンボを描画する
    // ----------------------
    pt[0].y = pt[1].y = nPaperHeight / 2 - nPaperMarginH;
    pt[0].x = nDPIW - nPaperMarginW;
    pt[1].x = pt[0].x - ConvX2Dt(2, nDPIW, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 左の横線

    pt[0].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].x = pt[0].x + ConvX2Dt(2, nDPIW, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 右の横線

    pt[0].x = pt[1].x = nDPIW - nPaperMarginW;
    pt[0].y = nPaperHeight / 2 - nDPIH / 2 - nPaperMarginH;
    pt[1].y = pt[0].y + nDPIH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 左の縦線

    pt[0].x = pt[1].x = nPaperWidth - nDPIW - nPaperMarginW;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 右の縦線

    // オフセット無しのトンボの位置
    DrawCross(g_MailBox.hDC, nDPIW, nPaperHeight / 2,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nPaperHeight / 2,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // 左上隅のトンボ
    // --------------
    pt[1].x = pt[2].x = nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x - ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y - ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // オフセット無しの場合
    DrawCross(g_MailBox.hDC, nDPIW, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM),
              ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // 右上隅のトンボ
    // --------------
    pt[1].x = pt[2].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x + ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y - ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // オフセット無しの場合
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // 左下隅のトンボ
    pt[1].x = pt[2].x = nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x - ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // オフセット無しの場合
    DrawCross(g_MailBox.hDC, nDPIW, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));

    // --------------
    // 右下隅のトンボ
    // --------------
    pt[1].x = pt[2].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[1].y = pt[0].y = nPaperHeight - nDPIH - nPaperMarginH;
    pt[0].x = pt[1].x + ConvX2Dt(2, nDPIW, CX_CM);
    pt[2].y = pt[1].y + ConvX2Dt(2, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 3);

    // オフセット無しの場合
    DrawCross(g_MailBox.hDC, nPaperWidth - nDPIW, nPaperHeight - nDPIH,
              ConvX2Dt(.5, nDPIW, CX_CM), ConvX2Dt(.5, nDPIH, CX_CM));
    // ----------------
    // 物差しの描画(横)
    // ----------------
    pt[0].x = x = nDPIW - nPaperMarginW;
    pt[1].x = nPaperWidth - nDPIW - nPaperMarginW;
    pt[0].y = pt[1].y = y =
        nPaperHeight - nDPIH - nPaperMarginH + ConvX2Dt(0.5, nDPIH, CX_CM);
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 横の線

    nWd = (int)((nPaperWidth - nDPIW * 2) * (25.4 * 2) / nDPIW);
    pt[0].y = y;
    for (i = 0; i <= nWd; i++) {
        pt[0].x = pt[1].x = x + ConvX2Dt(i / 20.0, nDPIW, CX_CM);
        switch(i % 20) {
        case 0:
            pt[1].y = pt[0].y + ConvX2Dt(0.7, nDPIH, CX_CM);
            break;
        case 10:
            pt[1].y = pt[0].y + ConvX2Dt(0.5, nDPIH, CX_CM);
            break;
        default:
            if (i % 2) {
                pt[1].y = pt[0].y + ConvX2Dt(0.15, nDPIH, CX_CM);
            }
            else {
                pt[1].y = pt[0].y + ConvX2Dt(0.3, nDPIH, CX_CM);
            }
            break;
        }
        Polyline(g_MailBox.hDC, &pt[0], 2);     // 縦の線
    }
    // ----------------
    // 物差しの描画(縦)
    // ----------------
    pt[0].x = pt[1].x = x = nPaperWidth - nDPIW
        + ConvX2Dt(.5, nDPIW, CX_CM) - nPaperMarginW;
    pt[0].y = y = nDPIH - nPaperMarginH;
    pt[1].y = nPaperHeight - nDPIH - nPaperMarginH;
    Polyline(g_MailBox.hDC, &pt[0], 2);         // 縦の線

    nWd = (int)((nPaperHeight - nDPIH * 2) * (25.4 * 2) / nDPIH);
    pt[0].x = x;
    for (i = 0; i <= nWd; i++) {
        pt[0].y = pt[1].y = y + ConvX2Dt(i / 20.0, nDPIW, CX_CM);
        switch(i % 20) {
        case 0:
            pt[1].x = pt[0].x + ConvX2Dt(0.7, nDPIW, CX_CM);
            break;
        case 10:
            pt[1].x = pt[0].x + ConvX2Dt(0.5, nDPIW, CX_CM);
            break;
        default:
            if (i % 2) {
                pt[1].x = pt[0].x + ConvX2Dt(0.15, nDPIW, CX_CM);
            }
            else {
                pt[1].x = pt[0].x + ConvX2Dt(0.3, nDPIW, CX_CM);
            }
            break;
        }
        Polyline(g_MailBox.hDC, &pt[0], 2);     // 横の線
    }

    // 後処理
    SelectObject(g_MailBox.hDC, hOldPen);
    DeleteObject(hPen);
}

/* -------------------------------------------------------------------
 * テスト印刷を行う
 * *-----------------------------------------------------------------*/
void
DoTestPrint(void)
{
    HFONT hFont, hFontOld;                      // フォント
    RECT rc;                                    // 矩形
    TCHAR szBuf[4096], szUser[64];
    SYSTEMTIME st;                              // システム時刻
    int i;
    int nPaperWidth, nPaperHeight, nDPIW, nDPIH, nPaperMarginW, nPaperMarginH;

    DbgPrint(NULL, 'I', "テスト印刷開始");

    BeginDocument();                            // プリンタ初期化

    nPaperWidth = GetPrtWd();
    nPaperHeight = GetPrtHt();
    nDPIW = GetPrtDpiW();
    nDPIH = GetPrtDpiH();
    nPaperMarginW = GetPrtMgW();
    nPaperMarginH = GetPrtMgH();

    StartPage(g_MailBox.hDC);                   // ページ開始

    rc.top = rc.right = 0;
    rc.left = nPaperWidth;
    rc.bottom = nPaperHeight;
    FillRect(g_MailBox.hDC, &rc, GetStockObject(WHITE_BRUSH));

    DrawTombow();                               // トンボの印刷

    SetTextColor(g_MailBox.hDC, RGB(0, 0, 0));

    // タイトル用フォントの作成
    hFont = CreatePrtFont(ConvX2Dt(20, nDPIH, CX_PT), 400,
                          TRUE, FALSE, FALSE, &g_MailBox.PrtInfo.lfPPF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    // タイトルの表示領域の設定
    rc.top = nDPIH - nPaperMarginH;
    rc.left = nDPIW - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(20, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW - nPaperMarginW;

    wsprintf(szBuf, "ak2pr %s テスト印字", VERSION);
    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);

    // 後処理
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);

    // フッダ用フォントの作成
    hFont = CreatePrtFont(ConvX2Dt(8, nDPIH, CX_PT), 400,
                          TRUE, FALSE, FALSE, &g_MailBox.PrtInfo.lfPPF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    rc.top = nPaperHeight - nDPIH - ConvX2Dt(30, nDPIH, CX_PT) - nPaperMarginH;
    rc.left = nDPIW - nPaperMarginW;
    rc.bottom = rc.top + ConvX2Dt(30, nDPIH, CX_PT);
    rc.right = nPaperWidth - nDPIW - nPaperMarginW;

    GetLocalTime(&st);
    i = 63;
    if (!GetUserName(szUser, &i)) {
        strcpy(szUser, "hogehoge");
    }

    wsprintf(szBuf, "この用紙は%d年%d月%d日%d時%d分%d秒%dミリ秒に"
             "%sによって印刷されました。\n" COPYRIGHT,
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
             st.wSecond, st. wMilliseconds, szUser);

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_NOPREFIX | DT_CENTER | DT_WORDBREAK);

    // 後処理
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);

    // 本文用フォントの作成
    hFont = CreatePrtFont(ConvX2Dt(10, nDPIH, CX_PT), 400,
                          FALSE, FALSE, FALSE, &g_MailBox.PrtInfo.lfTHF);
    hFontOld = SelectObject(g_MailBox.hDC, hFont);

    rc.top = nDPIH + ConvX2Dt(30, nDPIH, CX_PT) - nPaperMarginH;
    rc.left = nDPIW + ConvX2Dt(10, nDPIW, CX_PT) - nPaperMarginW;
    rc.bottom = nPaperHeight - nDPIH + ConvX2Dt(30, nDPIH, CX_PT)
        - nPaperMarginH;
    rc.right = nPaperWidth - nDPIW - ConvX2Dt(10, nDPIW, CX_PT)
        - nPaperMarginW;

    sprintf(szBuf, "　この用紙のサイズがプリンタドライバで設定した用紙サイズと"
            "一致しているならば, 1インチ(2.54cm)内側にトンボが切られている筈"
            "です。"
            "もしも切られていない場合は, 用紙サイズが異なっているか, "
            "プリンタドライバ"
            "が嘘を言っているか, 極めつけは私の計算ミスです。"
            "因みに, プリンタドライバが認識している現在の用紙サイズは横%d"
            "ドット, 縦%dドットです。このプリンタの解像度は横方向が%ddpi, "
            "縦方向が%ddpiなので, "
            "用紙サイズは横%fインチ即ち%fcm, 縦%fインチ即ち%fcmになります。\n"
            "\n　解像度の単位dpiは1インチ当りのドット数を現わしますので"
            "1インチ内側にトンボを切るのは複雑な計算を必要としません。"
            "単純に解像度を加減算するだけで座標を求める事が出来ます。"
            "このプリンタとこの用紙の組み合わせの場合は"
            "(%d, %d)-(%d, %d)の範囲にトンボが切られている事になります。"
            "但し, このプリンタには印刷不可能な領域として, "
            "横方向に%dドット(%fインチ即ち%fmm), "
            "縦方向に%dドット(%fインチ即ち%fmm)"
            "それぞれ存在していますので, 正確な座標は"
            "(%d, %d)-(%d, %d)になります。\n\n"
            "　また, トンボの内側のドット数は, 用紙サイズからそれぞれの方向"
            "の解像度に2を掛けた値を減算した値になります。この用紙の場合は"
            "横が%dドット, 縦が%dドットになります。これらの値をcmに変換"
            "すると, 横%fcm, 縦%fcmになります。トンボの内側の長さを示す"
            "物差しが印刷されている筈ですので, 確認してみて下さい。"
            "\n\n　○の中に＋のマークは印刷不可領域を無視した場合のトンボ"
            "の位置です。従って, 実際のトンボの位置からこのマークの中心"
            "迄の距離と印刷不可領域のサイズが一致している筈です。"
            "\n\n\n\n"
            "\t項目      \tドット\tインチ   \tcm\n"
            "\t------------------------------------------\n"
            "\t用紙の幅  \t%d\t%09.6f\t%09.6f\n"
            "\t用紙の高さ\t%d\t%09.6f\t%09.6f\n"
            "\t印字不可横\t%d\t%09.6f\t%09.6f\n"
            "\t印字不可縦\t%d\t%09.6f\t%09.6f\n"
            "\t------------------------------------------\n"
            "\t解像度: %d×%d\n",
            nPaperWidth, nPaperHeight, nDPIW, nDPIH,
            (double)nPaperWidth / nDPIW,
            (double)nPaperWidth / nDPIW * 2.54,
            (double)nPaperHeight / nDPIH,
            (double)nPaperHeight / nDPIH * 2.54,
            nDPIW, nDPIH, nPaperWidth - nDPIW, nPaperHeight - nDPIH,
            nPaperMarginW,
            (double)nPaperMarginW / nDPIW,
            (double)nPaperMarginW / nDPIW * 25.4,
            nPaperMarginH,
            (double)nPaperMarginH /nDPIH,
            (double)nPaperMarginH /nDPIH * 25.4,
            nDPIW - nPaperMarginW, nDPIH - nPaperMarginH,
            nPaperWidth - nDPIW - nPaperMarginW,
            nPaperHeight - nDPIH - nPaperMarginH,
            nPaperWidth - nDPIW * 2, nPaperHeight - nDPIH * 2,
            (double)(nPaperWidth - nDPIW * 2) / nDPIW * 2.54,
            (double)(nPaperHeight - nDPIH * 2) / nDPIH * 2.54,
            nPaperWidth,
            (double)nPaperWidth / nDPIW,
            (double)nPaperWidth / nDPIW * 2.54,
            nPaperHeight,
            (double)nPaperHeight / nDPIH,
            (double)nPaperHeight / nDPIH * 2.54,
            nPaperMarginW,
            (double)nPaperMarginW / nDPIW,
            (double)nPaperMarginW / nDPIW * 2.54,
            nPaperMarginH,
            (double)nPaperMarginH /nDPIH,
            (double)nPaperMarginH /nDPIH * 2.54,
            nDPIW, nDPIH);

    DrawText(g_MailBox.hDC, szBuf, -1, &rc,
             DT_EXPANDTABS | DT_NOPREFIX | DT_LEFT | DT_WORDBREAK);

    // 後処理
    SelectObject(g_MailBox.hDC, hFontOld);
    DeleteObject(hFont);
    DbgPrint(NULL, 'I', "テスト印刷終了");

    // プレビューの場合は、プレビュー画面を表示する
    if (g_MailBox.PrtInfo.bPreView) {
        PrintPreview(g_MailBox.hWnd, &g_MailBox.PrevInfo);
    }
    else {
        EndPage(g_MailBox.hDC);                 // ページ出力
        EndDoc(g_MailBox.hDC);                  // ドキュメント完了
    }
}
