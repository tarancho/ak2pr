/* -*- mode: c++; coding: sjis-dos; -*-
 * Time-stamp: <2004-06-18 09:35:13 tfuruka1>
 * Copyright (C) 2004 Tadamegu Furukawa
 *
 * �t�H���g�Ɋ֘A�����`
 *
 * $Id: fonts.c,v 1.2 2004/06/18 00:57:56 tfuruka1 Exp $
 * $Log: fonts.c,v $
 * Revision 1.2  2004/06/18 00:57:56  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.1  2004/01/19 05:32:32  tfuruka1
 * Initial revision
 *
 */

#include "ak2prs.h"

/*
 * �t�H���g�񋓃R�[���o�b�N�֐�
 */
int CALLBACK
EnumFontFamProc(
    ENUMLOGFONT FAR *lpelf,             // pointer to logical-font data
    NEWTEXTMETRIC FAR *lpntm,           // pointer to physical-font data
    int FontType,                       // type of font
    LPARAM lParam                       // address of application-defined data
    )
{
    PFONT_FUNK_INFO pFFI = (PFONT_FUNK_INFO)lParam;

    // �f�[�^�����݂��Ȃ��ꍇ�͏������Ȃ�
    if (!lpelf) {
        return 0;
    }

    switch (pFFI->funcNumber) {
    case FFI_FN_SETCOMBO:
        if (FontType & TRUETYPE_FONTTYPE) {
            SendMessage(pFFI->hWnd, CB_ADDSTRING, 0, 
                        (LPARAM)lpelf->elfLogFont.lfFaceName);
        }
        break;
    case FFI_FN_GETLOGFONT:
        memcpy(&pFFI->lf, &lpelf->elfLogFont, sizeof(LOGFONT));
        return 0;
    }

    return 1;
}

/*
 * �t�H���g�̈ꗗ���R���{�{�b�N�X�֐ݒ肷��
 */
VOID WINAPI
LsFontToCOMBO(HWND hWnd)
{
    FONT_FUNK_INFO ffi;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

    ffi.funcNumber = FFI_FN_SETCOMBO;
    ffi.hWnd = hWnd;

    // �R���{�{�b�N�X�̓��e��S�ď���
    SendMessage(hWnd, CB_RESETCONTENT, 0, 0);

    EnumFontFamilies(hDC, NULL, (FONTENUMPROC)EnumFontFamProc, (LPARAM)&ffi);
    DeleteDC(hDC);
}

/*
 * �t�H���g������, LOGFONT ���쐬����t�H���g���� LOGFONT �ɗ\�ߐݒ肳
 * ��Ă���K�v������܂��B
 */
BOOL WINAPI
GetLogFont(LPLOGFONT lplf)
{
    // �f�t�H���g�� LOGFONT
    static LOGFONT lft = {-13, 0, 0, 0, 400, 0, 0, 0, SHIFTJIS_CHARSET, 
                          3, 2, 1, 49, TEXT("MS �S�V�b�N")};
    FONT_FUNK_INFO ffi;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

    ffi.funcNumber = FFI_FN_GETLOGFONT;
    memset(&ffi.lf, 0, sizeof(LOGFONT));
    EnumFontFamilies(hDC, lplf->lfFaceName, 
                     (FONTENUMPROC)EnumFontFamProc, (LPARAM)&ffi);
    DeleteDC(hDC);

    if (ffi.lf.lfFaceName[0]) {
        memcpy(lplf, &ffi.lf, sizeof(LOGFONT));
        return TRUE;
    } else {
        memcpy(lplf, &lft, sizeof(LOGFONT));
        return FALSE;
    }
}
#if 0
int main(int argc, char *argv[])
{
    LsFontToCOMBO(NULL);
    printf("�ȏ�\n");
}
#endif
