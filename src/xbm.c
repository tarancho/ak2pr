/* -*- mode: c++ -*-
 * $Id: xbm.c,v 1.3 2005/05/08 13:03:27 tfuruka1 Exp $
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
 * Revision 1.3  2005/05/08 13:03:27  tfuruka1
 * X-Face�֘A�̒ǉ�
 *
 * Revision 1.2  2005/05/07 12:14:33  tfuruka1
 * printf�ōs���Ă����f�o�b�O��ak2pr�p�̂��̂ɕύX���܂����B
 *
 * Revision 1.1  2005/04/30 17:15:08  tfuruka1
 * �V�K�ǉ�
 *
 */

#include "xbm.h"

#define XFACE_STR "X-Face:"
#define PROCESS_TIMEOUT 100                     // �v���Z�X�^�C���A�E�g

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

BOOL WINAPI
MakeTempFileAndClose(
    IN const char *mode,                        // ���[�h
    IN OUT LPTSTR lpszFileName                  // �쐬������ƃt�@�C����
    )
{
    FILE *fp;

    sprintf(lpszFileName, "XBM_XXXXXX");
    if (!_mktemp(lpszFileName)) {
        return FALSE;
    }
    fp = fopen(lpszFileName, mode);
    fclose(fp);
    return TRUE;
}

#else
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    );

BOOL WINAPI
MakeTempFileAndClose(
    IN const char *mode,                        // ���[�h
    IN OUT LPTSTR lpszFileName                  // �쐬������ƃt�@�C����
    );

#endif

/*
 * �q�v���Z�X�̎��s
 */
static int WINAPI
ExecuteProcess(LPTSTR lpCmd)
{
    DWORD exitCode;
    HANDLE hProcess;
    long ret;
    STARTUPINFO stInfo;
    PROCESS_INFORMATION procInfo;
    int i;

    stInfo.cb = sizeof(STARTUPINFO);
    stInfo.lpReserved = NULL;
    stInfo.lpDesktop = NULL;
    stInfo.lpTitle = NULL;
    stInfo.dwFlags = STARTF_USESHOWWINDOW;
    stInfo.cbReserved2 = 0;
    stInfo.lpReserved2 = NULL;
    stInfo.wShowWindow = SW_HIDE;               // ��ʕ\������
    ret = CreateProcess(NULL, (LPTSTR)lpCmd, NULL, NULL, FALSE, 0, NULL,
                        NULL, &stInfo, &procInfo);

    if (!ret) {
        int nErr = GetLastError();
        DbgPrint(0, 'E', "%s(%d) �R�}���h���s���s�F %s", __FILE__, __LINE__,
                 lpCmd);
        return 255;
    }

    // �n���h���͎g�p���Ȃ��̂ŕ���
    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);

    // �v���Z�X�̏I����҂����킹��
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 1, procInfo.dwProcessId);
    for (i = 0; i < PROCESS_TIMEOUT; i++) {
        GetExitCodeProcess(hProcess, &exitCode);
        if (STILL_ACTIVE != exitCode) {
            DbgPrint(0, 'I', "�R�}���h���s(%s)�I��: ExitCode=%d",
                     lpCmd, exitCode);
            CloseHandle(hProcess);
            return exitCode;
        }
        Sleep(100);
    }
    // �^�C���A�E�g
    DbgPrint(0, 'W', "�v���Z�X�������I�����܂� %s", lpCmd);
    TerminateProcess(hProcess, 255);
    CloseHandle(hProcess);
    return 255;
}

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
    int max = (SETXBM_XBM == kind) ? 4 : 6;
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
 * XBM �܂���, uncompface �t�@�C���̏���W�J����BLPXBM_INFO��lpData
 * �ɓW�J�̈�ƁAcbData ��lpData�̃o�C�g�����ݒ肳��Ă���K�v�������
 * ���B���̒l���z����T�C�Y��W�J���鎖�͂ł��܂���B
 */
BOOL WINAPI
SetXBM(LPXBM_INFO lpXbm, LPCTSTR lpszFileName, int kind)
{
    int i;
    long c;
    size_t size;
    FILE *fp;

    if (!(fp = fopen(lpszFileName, "rt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszFileName));
        return FALSE;
    }

    lpXbm->kind = kind;

    if (SETXBM_XBM == kind) {                   // xbm
        lpXbm->nWidth = GetXbmWidth(fp);
        lpXbm->nHeight = GetXbmHeight(fp);
    } else {                                    // uncompface
        lpXbm->nWidth = 48;
        lpXbm->nHeight = 48;
    }
    lpXbm->cbWidth = ((lpXbm->nWidth / 8)
                      + (0 == (lpXbm->nWidth % 8) ? 0 : 1));
    size = lpXbm->cbWidth * lpXbm->nHeight;
    if (size > lpXbm->cbData) {
        DbgPrint(NULL, 'E', "%s(%d) XBM���������ɓW�J�ł��܂���B"
                 "%d�o�C�g�̗̈悪�K�v�ł����A%d�o�C�g��������܂���B",
                 __FILE__, __LINE__, size, lpXbm->cbData);
        return FALSE;
    }
    lpXbm->cbData = size;

    if (SETXBM_XBM == kind) {                   // xbm
        SkipToFirstByte(fp);

        for (i = 0; i < (int)lpXbm->cbData; i++) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = c & 0xff;
        }
    } else {                                    // uncompface
        for (i = 0; i < (int)lpXbm->cbData; i += 2) {
            c = GetByte(fp, kind);
            *(lpXbm->lpData + i) = (c >> 8)& 0xff;
            *(lpXbm->lpData + i + 1) = c & 0xff;
        }
    }
    fclose(fp);

    return TRUE;
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

    if (SETXBM_UFACE == lpXbm->kind) {
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

/*
 * ���[���t�@�C������X-FACE������؂�o���A�t�@�C���ɏo�͂���
 */
static BOOL WINAPI
CutXFACEToFile(
    LPTSTR lpszInFile,                          // ���[���t�@�C��
    LPTSTR lpszOutFile                          // X-FACE�o�͐�
    )
{
    FILE *fpIn;
    FILE *fpOut;
    int cbXFace = strlen(XFACE_STR);
    BOOL bFind = FALSE;
    TCHAR szBuf[1024];

    if (!(fpIn = fopen(lpszInFile, "rt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszInFile));
        return FALSE;
    }
    if (!(fpOut = fopen(lpszOutFile, "wt"))) {
        DbgPrint(0, 'E', "%s(%d) %s", __FILE__, __LINE__,
                 _strerror(lpszOutFile));
        fclose(fpIn);
        return FALSE;
    }

    // X-Face:�����t���閘�A�ǂݔ�΂�
    while (fgets(szBuf, 1024, fpIn)) {
        if (0 == memicmp(szBuf, XFACE_STR, cbXFace)) {
            bFind = TRUE;
            break;
        }
    }
    if (!bFind) {
        DbgPrint(0, 'I', "%s �͑��݂��܂���", XFACE_STR);
        fclose(fpIn);
        fclose(fpOut);
        return FALSE;
    }
    fprintf(fpOut, "%s", szBuf + cbXFace);

    while (fgets(szBuf, 1024, fpIn)) {
        // �p���s�łȂ���ΏI��
        if (' ' != szBuf[0]) {
            break;
        }
        fprintf(fpOut, "%s", szBuf);
    }
    fclose(fpIn);
    fclose(fpOut);
    return TRUE;
}

/*
 * uncompface�����s����
 */
BOOL WINAPI
ExecuteUncompface(
    LPTSTR lpszCmdPath,                         // uncompface�p�X
    LPTSTR lpszInFile,                          // ���[���t�@�C��
    LPTSTR lpszOutFile                          // �o�̓t�@�C��
    )
{
    TCHAR szCmd[1024];
    TCHAR szTempFile[MAX_PATH];
    int exitCode;

    strcpy(szTempFile, "XFACE");
    if (!MakeTempFileAndClose("wt", szTempFile)) {
        return FALSE;
    }

    if (!CutXFACEToFile(lpszInFile, szTempFile)) {
        unlink(szTempFile);
        return FALSE;
    }

    sprintf(szCmd, "%s %s %s", lpszCmdPath, szTempFile, lpszOutFile);
    exitCode = ExecuteProcess(szCmd);
    unlink(szTempFile);
    return 0 == exitCode ? TRUE : FALSE;
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
    int x, y;
    int kind;
    LPCTSTR lpszFileName;
    TCHAR szFileName[MAX_PATH];
    HWND hWnd = GetConsoleHwnd();
    HDC hDC = GetDC(hWnd);
    unsigned char xbmBuf[6 * 48];
    XBM_INFO xbmInfo;

    xbmInfo.lpData = xbmBuf;
    xbmInfo.cbData = sizeof(xbmBuf);

    if (3 != argc) {
        printf("Usage: xbm [u | x | m<uncompfacePath>] FileName\n");
        printf(" u - uncompface�ŏ������ꂽ�t�@�C��\n"
               " x - xbm�t�@�C��\n"
               " m - X-FACE���܂܂ꂽ���[���t�@�C��\n");
        return 1;
    }

    kind = (*argv[1] != 'x') ? SETXBM_UFACE : SETXBM_XBM;
    lpszFileName = argv[2];
    if (*argv[1] == 'm') {
        strcpy(szFileName, "X-FACE");
        if (!MakeTempFileAndClose("wt", szFileName)) {
            return 1;
        }
        if (!ExecuteUncompface(argv[1] + 1, argv[2], szFileName)) {
            unlink(szFileName);
            return 1;
        }
        lpszFileName = szFileName;
    }
    if (!SetXBM(&xbmInfo, lpszFileName, kind)) {
        if (*argv[1] == 'm') {
            unlink(lpszFileName);
        }
        return 1;
    }
    if (*argv[1] == 'm') {
        unlink(lpszFileName);
    }

    // �e�L�X�g�ŕ`��
    for (y = 0; y < xbmInfo.nHeight; y++) {
        for (x = 0; x < xbmInfo.nWidth; x++) {
            printf("%c", PeekXBM(&xbmInfo, x, y) ? 'X': ' ');
        }
        printf("\n");
    }

    // �O���t�B�b�N�ŕ`��
    DrawXBM(&xbmInfo, hDC, 100, 100, xbmInfo.nWidth * 2, xbmInfo.nHeight * 2,
            RGB(0, 0, 0), RGB(255, 255, 255), SRCCOPY);

    ReleaseDC(hWnd, hDC);
    return 0;
}
#endif
