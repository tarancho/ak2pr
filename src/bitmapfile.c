/* -*- mode: c++; coding: sjis; -*-
 * Copyright (C) 2005 Tadamegu Furukawa
 * $Id: bitmapfile.c,v 1.2 2005/08/03 10:09:20 tfuruka1 Exp $
 * $Name:  $
 *
 * �r�b�g�}�b�v�t�@�C���֘A
 *
 * +------------------------------------------+
 * | BITMAPFILEHEADER                         |
 * +------------------------------------------+
 * | BITMAPINFO or BITMAPCOREINFO             |
 * | +----------------------------------------+
 * | | BITMAPCOREHEADER or BITMAPINFOHEADER or|
 * | | BITMAPV4HEADER or BITMAPV5HEADER       |
 * | +----------------------------------------+
 * | | Color table(Optional)                  |
 * +-+----------------------------------------+
 * | Bitmap data                              |
 * +------------------------------------------+
 * | Profile data(Optional)                   |
 * +------------------------------------------+
 *
 * �P�̕]���̕��@�F
 *
 *   �ȉ��̕��@�ŃR���p�C�����܂��B
 *
 *      cl /W3 /Zi /DDEBUG bitmapfile.c /link gdi32.lib user32.lib
 *
 *   ���Ƃ́A�R�}���h���C������
 *
 *     bitmapfile.exe bmpFile outputFile
 *
 *   �ő������Ŏw�肵���r�b�g�}�b�v����ʂɕ\�����A���̏������
 *   ���Ŏw�肵���t�@�C���ɏo�͂��܂��B
 *
 * $Log: bitmapfile.c,v $
 * Revision 1.2  2005/08/03 10:09:20  tfuruka1
 * ���[�������Face������ł���悤�ɂ��܂����B
 *
 * Revision 1.1  2005/08/03 03:43:42  tfuruka1
 * �V�K�ǉ�(Face�Ή��̑O����)
 *
 */

#include "bitmapfile.h"

#define DBG_FMT(format) "%s(%d) : " format, __FILE__, __LINE__

/* 
 * �r�b�g�}�b�v�t�@�C����ǂݍ���, �r�b�g�}�b�v�n���h�����쐬���܂��B
 * ���������ꍇ��, BITMAP �n���h����ԋp���܂��B���s�����ꍇ�� NULL ��
 * �ԋp���܂��B�ԋp�����r�b�g�}�b�v���s�v�ɂȂ�����, DeleteObject �֐�
 * �ō폜���Ă��������B
 *
 * �J���[�p���b�g�ɂ͑Ή����Ă��܂���B
 *
 */
HBITMAP WINAPI
LoadBitMapFile(
    HDC hDC,                                    // ���̃f�o�C�X�ƌ݊�
                                                // ���̂���r�b�g�}�b
                                                // �v���쐬����܂�
    LPCTSTR lpszBmFile                          // �t�@�C����
    )
{
    FILE *fp;
    HBITMAP hBm = NULL;
    BITMAPFILEHEADER bmih;
    LPBITMAPINFO lpBmInfo = NULL;
    size_t cbBmInfo;
    LPVOID pColor = NULL;

    if (!(fp = fopen(lpszBmFile, "rb"))) {
        printf(DBG_FMT("%s\n"), _strerror(lpszBmFile));
        return 0;
    }

    if (sizeof(bmih) != fread(&bmih, 1, sizeof(bmih), fp)) {
        printf(DBG_FMT("BITMAPFILEHEADER ��ǂݍ��߂܂���: %s\n"), lpszBmFile);
        fclose(fp);
        return 0;
    }
    printf("BITMAPFILEHEADER %s : Type=%x(%c%c), Size=%d, Offset=%d\n",
           lpszBmFile,
           bmih.bfType,
           bmih.bfType % 256,
           bmih.bfType / 256,
           bmih.bfSize, 
           bmih.bfOffBits);
    if (0x4d42 != bmih.bfType) {
        printf("%s �͗L���ȃr�b�g�}�b�v�t�@�C���ł͂���܂���B", lpszBmFile);
        fclose(fp);
        return 0;
    }

    cbBmInfo = bmih.bfSize - sizeof(BITMAPFILEHEADER);
    printf("BITMAP���T�C�Y: %d\n", cbBmInfo);

    if (!(lpBmInfo = (LPBITMAPINFO)malloc(cbBmInfo))) {
        printf(DBG_FMT("BITMAP����W�J���郁�������m�ۂł��܂���B\n"));
        fclose(fp);
        return 0;
    }

    if (cbBmInfo != fread(lpBmInfo, 1, cbBmInfo, fp)) {
        printf(DBG_FMT("BITMAP����ǂݍ��߂܂���: %s\n"), lpszBmFile);
        fclose(fp);
        return 0;
    }
    fclose(fp);

    pColor = ((LPSTR)lpBmInfo + (WORD)(lpBmInfo->bmiHeader.biSize)); 

    switch (lpBmInfo->bmiHeader.biSize) {
    case sizeof(BITMAPCOREHEADER):
        printf("BITMAPCOREHEADER: PCOLOR=%x\n", pColor);
        break;
    case sizeof(BITMAPINFOHEADER):
        printf("BITMAPINFOHEADER: PCOLOR=%x\n", pColor);
        break;
    case sizeof(BITMAPV4HEADER):
        printf("BITMAPV4HEADER: PCOLOR=%x\n", pColor);
        break;
    default:
        printf(DBG_FMT("BITMAPINFOHEADER�̎�ʂ����ł��܂���\n"));
        free(lpBmInfo);
        return 0;
    }
    // �J���[�p���b�g�ɑΉ����悤�Ǝv�������ǁA�Ƃ肠�������~�ł��B

    if (!(hBm = CreateDIBitmap(
              hDC,
              (LPBITMAPINFOHEADER)lpBmInfo,
              CBM_INIT,
              (LPBYTE)lpBmInfo + bmih.bfOffBits - sizeof(BITMAPFILEHEADER),
              lpBmInfo,
              DIB_RGB_COLORS))) {
        printf(DBG_FMT("BITMAP�̍쐬�Ɏ��s���܂���\n"));
        free(lpBmInfo);
        return NULL;
    }
    free(lpBmInfo);
    return hBm;
}

/*
 * �r�b�g�}�b�v���r�b�g�}�b�v�t�@�C���ɕۑ����܂��B
 */
BOOL WINAPI
SaveBitmapFile(
    HDC hDC,
    HBITMAP hBm,
    LPCTSTR lpszSaveFileName
    )
{
    FILE *fp;
    BITMAP bm;
    BITMAPFILEHEADER bmfh;
    BITMAPINFO bmi;
    LPBYTE lpBits;

    if (!GetObject(hBm, sizeof(BITMAP), &bm)) {
        return FALSE;
    }

    memset(&bmfh, 0, sizeof(BITMAPFILEHEADER));
    memset(&bmi, 0, sizeof(BITMAPINFO));

    bmfh.bfType = 'M' * 256 + 'B';
    bmfh.bfSize = sizeof(BITMAPFILEHEADER)
        + sizeof(BITMAPINFO) + bm.bmWidthBytes * bm.bmHeight;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = bm.bmHeight;
    bmi.bmiHeader.biPlanes = bm.bmPlanes;
    bmi.bmiHeader.biBitCount = bm.bmBitsPixel;

    if (!(lpBits = (LPBYTE)malloc(bm.bmWidthBytes * bm.bmHeight))) {
        printf(DBG_FMT("�r�b�g�}�b�v��W�J���郁�������m�ۂł��܂���\n"));
        return FALSE;
    }

    if (!GetDIBits(hDC, hBm, 0, bm.bmHeight, lpBits, &bmi, DIB_RGB_COLORS)) {
        free(lpBits);
        printf(DBG_FMT("�r�b�g���̎擾�Ɏ��s���܂���\n"));
        return FALSE;
    }

    if (!(fp = fopen(lpszSaveFileName, "wb"))) {
        free(lpBits);
        printf(DBG_FMT("�t�@�C���̃I�[�v���Ɏ��s���܂���: %s\n"),
               _strerror(0));
        return FALSE;
    }
    fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
    fwrite(&bmi, 1, sizeof(BITMAPINFO), fp);
    fwrite(lpBits, 1, bm.bmWidthBytes * bm.bmHeight, fp);
    free(lpBits);
    fclose(fp);
    
    return TRUE;
}
//�����������������������������������������������������ȉ��͕]���p�֐�
#if defined(DEBUG)
#include <time.h>
int main(int argc, char *argv[])
{
    TCHAR szTitle[1024];
    HBITMAP hBM, hBMOld;
    BITMAP bm;
    HWND hWnd;
    HDC hDC;
    HDC hDCMem;

    sprintf(szTitle, "Test-%d", time(NULL));
    SetConsoleTitle(szTitle);
    Sleep(40);

    hWnd = FindWindow(NULL, szTitle);
    printf("HWND: %x\n", hWnd);
    hDC = GetDC(hWnd);
    hDCMem = CreateCompatibleDC(hDC);
    printf("HDC: %x, %x\n", hDC, hDCMem);

    if (!(hBM = LoadBitMapFile(hDC, argv[1]))) {
        ReleaseDC(hWnd, hDC);
        DeleteDC(hDCMem);
        return 1;
    }

    printf("GetObject: %d\n", GetObject(hBM, sizeof(BITMAP), &bm));
    printf("Size: %d�~%d %x\n", bm.bmWidth, bm.bmHeight, bm.bmBits);

    hBMOld = SelectObject(hDCMem, hBM);
    printf("BitBlt: %d\n",
           BitBlt(hDC, 0, 32, bm.bmWidth, bm.bmHeight, hDCMem, 0, 0, SRCCOPY));
    SelectObject(hDCMem, hBMOld);

    SaveBitmapFile(hDCMem, hBM, argv[2]);

    DeleteObject(hBM);

    ReleaseDC(hWnd, hDC);
    DeleteDC(hDCMem);
    return 0;
}

#endif
