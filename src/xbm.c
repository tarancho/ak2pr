/* -*- mode: c++ -*-
 * $Id: xbm.c,v 1.2 2005/05/07 12:14:33 tfuruka1 Exp $
 * $Name:  $
 *
 * xbm, uncompface �t�@�C���̓W�J�����s�Ȃ��܂��B�{����, ���X
 * uncompface ��W�J����̂��ړI��, uncompface �� -X �I�v�V�����Ő���
 * ���� xbm �t�@�C����W�J�ł���̂��m�F�����Ƃ���Łc uncompface ��
 * -X �I�v�V��������, version �ɂ����, �T�|�[�g����Ȃ������������Ă�
 * �܂�, �}�, umcompface �ɑΉ������̂ł������B
 *
 * �����N���́Agdi32.lib, user32.lib ���K�v�ł��B�P�̃f�o�b�O���� 
 * XBM_DEBUG ���R���p�C���I�v�V�����Ɏw�肷��ƁA���̃t�@�C�������œ�
 * �삵�܂��B
 *
 *    cl /W3 /Zi /DXBM_DEBUG xbm.c /link gdi32.lib user32.lib
 *
 * $Log: xbm.c,v $
 * Revision 1.2  2005/05/07 12:14:33  tfuruka1
 * printf�ōs���Ă����f�o�b�O��ak2pr�p�̂��̂ɕύX���܂����B
 *
 * Revision 1.1  2005/04/30 17:15:08  tfuruka1
 * �V�K�ǉ�
 *
 */

#include "xbm.h"

#if defined(XBM_DEBUG)
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    )
{
    va_list args;                               // �����W�J�p
    // ������������ɍ����Đ��`����
    va_start(args, lpstr);
    vprintf(lpstr, args);
    va_end(args);
    printf("\n");
    return NULL;
}
#else
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    );
#endif

/*
 * XBM�t�@�C������A���A���͍����𓾂�
 */
static int
GetXbmValue(FILE *fp, char *lpszItem)
{
    char szBuf[1024];
    char *p;
    char *p1;

    // �t�@�C���̐擪�փV�[�N
    if (0 != fseek(fp, 0, SEEK_SET)) {
        perror("SEEK");
        return 0;
    }

    while (fgets(szBuf, 1024, fp)) {
        if ('#' != szBuf[0]) {
            continue;
        }
        if (NULL == (p = strstr(szBuf, lpszItem))) {
            continue;
        }
        if (NULL == (p1 = strrchr(p, ' '))) {
            continue;
        }
        return atoi(p1);
    }
    return 0;
}

/*
 * XBM�t�@�C������A���𓾂�
 */
static int
GetXbmWidth(FILE *fp)
{
    return GetXbmValue(fp, "_width ");
}

/*
 * XBM�t�@�C������A�����𓾂�
 */
static int
GetXbmHeight(FILE *fp)
{
    return GetXbmValue(fp, "_height ");
}

/*
 * XBM�t�@�C���̃f�[�^�����̐擪�ɃV�[�N����
 */
static BOOL
SkipToFirstByte(FILE *fp)
{
    int c;

    while (EOF != (c = getc(fp))) {
        if ('{' == c) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * XBM�t�@�C������1�o�C�g���ǂݍ��ނ��Auncompress���o�͂����f�[�^�t�@
 * �C������2�o�C�g���ǂݍ��ށB
 */
static long
GetByte(FILE *fp, int kind)
{
    int c;
    int idx;
    int max = (AXBM_XBM == kind) ? 4 : 6;
    char szBuf[12];

    // �ŏ��̋󔒂�����
    while (EOF != (c = getc(fp))) {
        if (!isspace(c)) {
            break;
        }
    }
    idx = 0;
    if (!isdigit(c)) {
        DbgPrint(0, 'D', "%s#(%d)�z��O�̕���: %c[%02x]",
                 __FILE__, __LINE__, c, c);
        return -1;
    }
    szBuf[idx] = c;
    idx++;
    szBuf[idx] = '\0';
    while (EOF != (c = getc(fp))) {
        if (isspace(c)) {
            continue;
        }
        if (('X' != c) && ('x' != c) && (!isxdigit(c))) {
            return (int)strtol(szBuf, NULL, 16);
        }
        szBuf[idx] = c;
        idx++;
        szBuf[idx] = '\0';
        if (max < idx) {
            DbgPrint(0, 'D', "%s(%d)���������������܂�: %s\n",
                     __FILE__, __LINE__, szBuf);
            return -1;
        }
    }
    DbgPrint(0, 'D', "%s(%d)EOF���o", __FILE__, __LINE__);
    return -1;
}

/*
 * XBM �܂���, uncompface �t�@�C���̏���W�J����B�����œn���ꂽ�t�@
 * �C���|�C���^�͏���������N���[�Y���܂��B�G���[�����������ꍇ��, �N
 * ���[�Y���܂���B
 */
LPXBM_INFO
AllocXBM(FILE *fp, int kind)
{
    int i;
    long c;

    LPXBM_INFO lpXbm = (LPXBM_INFO)malloc(sizeof(XBM_INFO));

    if (!lpXbm) {
        DbgPrint(0, 'D', "%s(%d)�������s��(XBM-INFO)", __FILE__, __LINE__);
        return NULL;
    }
    lpXbm->lpData = NULL;
    lpXbm->kind = kind;

    if (AXBM_XBM == kind) {                     // xbm
        lpXbm->nWidth = GetXbmWidth(fp);
        lpXbm->nHeight = GetXbmHeight(fp);
    } else {                                    // uncompface
        lpXbm->nWidth = 48;
        lpXbm->nHeight = 48;
    }
    lpXbm->cbWidth = ((lpXbm->nWidth / 8)
                      + (0 == (lpXbm->nWidth % 8) ? 0 : 1));
    lpXbm->cbData = lpXbm->cbWidth * lpXbm->nHeight;

    lpXbm->lpData = (unsigned char *)malloc(lpXbm->cbData);
    if (!lpXbm->lpData) {
        free(lpXbm);
        DbgPrint(0, 'D', "%s(%d)�������s��(XBM-DATA)", __FILE__, __LINE__);
        return NULL;
    }

    if (AXBM_XBM == kind) {                     // xbm
        SkipToFirstByte(fp);

        for (i = 0; i < lpXbm->cbData; i++) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = c & 0xff;
        }
    } else {                                    // uncompface
        for (i = 0; i < lpXbm->cbData; i += 2) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = (c >> 8)& 0xff;
            *(lpXbm->lpData + i + 1) = c & 0xff;
        }
    }
    fclose(fp);

    return lpXbm;
}

/*
 * AllocXBM�Ŏ擾�������G���A���J�����܂��B
 */
void
FreeXBM(LPXBM_INFO lpXbm)
{
    if (lpXbm && lpXbm->lpData) {
        free(lpXbm->lpData);
    }
    if (lpXbm) {
        free(lpXbm);
    }
}

/*
 * XBM���̍��W�̓��e��ԋp���܂��B
 */
int
PeekXBM(LPXBM_INFO lpXbm, int x, int y)
{
    int idx = y * (lpXbm->cbWidth) + (x / 8);
    int shift = (x % 8);
    int c = *(lpXbm->lpData + idx);

    if (AXBM_UFACE == lpXbm->kind) {
        shift = 7 - shift;
    }
    return (c >> shift) & 1;
}

/*
 * XBM���̓��e��`�悵�܂��B
 */
BOOL WINAPI
DrawXBM(
    LPXBM_INFO lpXbm,                           // xbm���
    HDC hDC,                                    // �`�悷��DC
    int x,                                      // �`����W
    int y,                                      // �`����W
    int nWidth,                                 // �`�敝
    int nHeight,                                // �`�捂
    COLORREF crFg,                              // �S�i�F
    COLORREF crBg,                              // �w�i�F
    DWORD dwRop                              // ���X�^�[�I�y���[�V����
    )
{
    HDC hMemDC;                        // �f�o�C�X�R���e�L�X�g�i���z�j
    HBITMAP hBitMap;                            // �r�b�g�}�b�v
    HBITMAP hOldBitmap;                         // �Â��r�b�g�}�b�v
    int xx, yy;

    // ���z�f�o�C�X�R���e�L�X�g���쐬����
    if (!(hMemDC = CreateCompatibleDC(hDC))) {
        DbgPrint(0, 'D', "%s(%d)���z�f�o�C�X�R���e�L�X�g�쐬���s",
                 __FILE__, __LINE__);
        return FALSE;
    }

    // �r�b�g�}�b�v�̍쐬
    if (!(hBitMap = CreateBitmap(lpXbm->nWidth,
                                 lpXbm->nHeight, 1, 1, NULL))) {
        DbgPrint(0, 'D', "%s(%d)�r�b�g�}�b�v�̍쐬�Ɏ��s���܂���",
                 __FILE__, __LINE__);
        DeleteDC(hMemDC);                  // �f�o�C�X�R���e�L�X�g�폜
        return FALSE;
    }

    // �r�b�g�}�b�v�̑I��
    if (!(hOldBitmap = SelectObject(hMemDC, hBitMap))) {
        DbgPrint(0, 'D', "%s(%d)�r�b�g�}�b�v�̑I���Ɏ��s���܂����B",
                 __FILE__, __LINE__);
        DeleteDC(hMemDC);                  // �f�o�C�X�R���e�L�X�g�폜
        DeleteObject(hBitMap);                  // �r�b�g�}�b�v�폜
        return FALSE;
    }

    // XBM�̕`��
    for (yy = 0; yy < lpXbm->nWidth; yy++) {
        for (xx = 0; xx < lpXbm->nHeight; xx++) {
            SetPixel(hMemDC, xx, yy,
                     PeekXBM(lpXbm, xx, yy) ? crFg : crBg);
        }
    }

    // XBM�̓]��
    StretchBlt(hDC, x, y, nWidth, nHeight,
               hMemDC, 0, 0, lpXbm->nWidth, lpXbm->nHeight, dwRop);

    // ��n��
    SelectObject(hMemDC, hOldBitmap);           // �r�b�g�}�b�v��߂�
    DeleteObject(hBitMap);                      // �r�b�g�}�b�v�폜
    DeleteDC(hMemDC);                           // ���zDC�폜

    return TRUE;
}

#if defined(XBM_DEBUG) // ��������������������������������������������
/*
 * SDK32:�R���\�[���E�B���h�E�̃n���h���擾
 *
 * �{�֐��͈ȉ��̋L�����Q�l�ɂ��č쐬���܂����B
 *
 * �ŏI�X�V��: 1999/02/09
 * �����ԍ�: J046738
 *
 * ���̎����͈ȉ��ɂ��ċL�q�������̂ł��B
 *
 * Microsoft(R) Win32(R) Software Development Kit (SDK)
 *
 * ���̎����́A�č� Microsoft Corporation ����񋟂���Ă��� Knowledge
 * Base �� Article ID Q124103 (�ŏI�X�V�� 1988-12-23) �����Ƃɍ쐬����
 * ���̂ł��B
 */
#define MY_BUFSIZE 1024                      // �R���\�[���̃^�C�g���p
HWND GetConsoleHwnd(VOID)
{
    HWND hwndFound;
    char pszNewWindowTitle[MY_BUFSIZE];
    char pszOldWindowTitle[MY_BUFSIZE];
    int i;

    // �R���\�[���^�C�g���̎擾
    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    // �Ǝ��ɁA�E�B���h�E�̐V�K�^�C�g�����t�H�[�}�b�g���܂�
    wsprintf(pszNewWindowTitle,"%d/%d-%s",
             GetTickCount(),
             GetCurrentProcessId(),
             pszOldWindowTitle);

    // ���݂̃E�B���h�E�^�C�g����ύX���܂�
    SetConsoleTitle(pszNewWindowTitle);

    for (i = 0; i < 100; i++) {
        // �E�B���h�E�̐V�K�^�C�g����T���ɂ����܂�
        hwndFound = FindWindow(NULL, pszNewWindowTitle);
        if (hwndFound) {
            DbgPrint(0, 'D', "%s(%d)GetConsoleHwnd(): ��=%d, T=%s\n",
                     __FILE__, __LINE__, i, pszNewWindowTitle);
            break;                              // ��������
        }
        Sleep(10);                              // 10m Wait
    }

    // ���̃E�B���h�E�^�C�g���֖߂��܂�
    SetConsoleTitle(pszOldWindowTitle);

    return hwndFound;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int x, y;
    int kind;
    LPXBM_INFO lpXbm;
    HWND hWnd = GetConsoleHwnd();
    HDC hDC = GetDC(hWnd);

    if (3 != argc) {
        printf("Usage: xbm [u | x] FileName\n");
        return 1;
    }

    kind = (*argv[1] == 'u') ? AXBM_UFACE : AXBM_XBM;

    if (!(fp = fopen(argv[2], "rt"))) {
        perror(argv[1]);
        return 1;
    }

    if (!(lpXbm = AllocXBM(fp, kind))) {
        return 1;
    }

    // �e�L�X�g�ŕ`��
    for (y = 0; y < lpXbm->nHeight; y++) {
        for (x = 0; x < lpXbm->nWidth; x++) {
            printf("%c", PeekXBM(lpXbm, x, y) ? 'X': ' ');
        }
        printf("\n");
    }

    // �O���t�B�b�N�ŕ`��
    DrawXBM(lpXbm, hDC, 100, 100, lpXbm->nWidth * 2, lpXbm->nHeight * 2,
            RGB(0, 0, 0), RGB(255, 255, 255), SRCCOPY);
    FreeXBM(lpXbm);

    ReleaseDC(hWnd, hDC);
    return 0;
}
#endif
