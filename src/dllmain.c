/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2003-03-14 22:09:25 tfuruka1>
 *
 * ak2ps �̂悤�Ȃ��̂̋��� DLL
 *
 * $Id: dllmain.c,v 1.13 2003/03/14 15:06:19 tfuruka1 Exp $
 * $Log: dllmain.c,v $
 * Revision 1.13  2003/03/14 15:06:19  tfuruka1
 * �� Syslog�֐��̎d�l�ύX�ɔ����ASyslog�֐����g�p���Ă��镔���̏C�����s��
 *    ���B
 * �� ��ƃt�@�C���̍쐬���[�����ꕔ�ύX����(.��_�ɕύX)�BDistiller���쐬
 *    ����PDF�t�@�C�����A�g���q�ȍ~�𖳎�����̂ŁA�o�̓t�@�C�������d����
 *    ��̂������̂��ړI�ł���B
 * �� �T�[�o�̋N���֐��ɁA�T�[�o�̋N���I�v�V������ǉ������B
 * �� Syslog�֐��̎d�l��ύX�����B���p�����[�^�ɕW���o�̓I�v�V�������
 *    �������B
 * �� PostScript�t�@�C������^�C�g�����擾����֐���ǉ������B
 *
 * Revision 1.12  2003/03/01 09:01:25  tfuruka1
 * �� ��ƃt�@�C���̍쐬�����ɂ����āA�t�@�C�����Ɏg�p�o���Ȃ��������w�苎
 *    �ꂽ�ꍇ�ɓK�؂ȕ����ɕύX����悤�ɏC�����s�����B
 *
 * Revision 1.11  2003/02/25 15:28:35  tfuruka1
 * �s�ԍ�����̏����ǉ��ɂ��Ή����s�����B
 *
 * Revision 1.10  2001/12/23 10:23:33  tfuruka1
 * ����ƃf�B���N�g�����擾�̊֐���V�K�ǉ��i��ƃt�@�C���쐬�֐�����؂�o
 *   ���j
 *
 * Revision 1.9  2001/12/17 14:33:17  tfuruka1
 * syslog�����ŕW���o�͂֏o�͂��Ă����������폜�����i����ς肤���Ƃ�����
 * �̂Łj�B
 *
 * Revision 1.8  2001/12/14 17:04:43  tfuruka1
 * �v���r���[�Ή�
 *
 * Revision 1.7  2001/12/08 15:19:06  tfuruka1
 * �p���T�C�Y�̎w��Ή��̑Ή��R��̑Ή��B
 *
 * Revision 1.6  2001/12/07 18:25:11  tfuruka1
 * �p���T�C�Y�̎w����o����悤�ɂ����B
 *
 * Revision 1.5  2001/10/01 13:20:47  tfuruka1
 * �p���̌������w��o����悤�ɏC���B
 *
 * Revision 1.4  2001/08/19 04:35:57  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.3  2001/08/18 16:30:30  tfuruka1
 * ����ƃt�@�C���̍쐬��_mktemp�֐����g�p���Ă������A�悭�l�����炱�̊�
 *   ���́A�ő�� 27 ������ӂȃt�@�C�������쐬�ł��Ȃ����ɋC���t����
 *   �i�Ƃ������A�G���[���o���j�̂ŁA_mktemp��base���ɗ͈�ӂɂȂ�悤��
 *   ���āA27�ȏ�̈�ӂȃt�@�C�����쐬�ł���悤�ɂ����B
 * ��MessageBox���g�p���Ă��镔����Syslog���g�p����悤�ɕύX����(Meadow
 *   ����Ă΂ꂽ����MessageBox���g�p����ƁAWindow�������Ȃ��̂Ń_�C�A��
 *   �O����鎖���ł��Ȃ��Ȃ�̂�)�B
 * ��SendPrintFromFile�֐���p�~���ASendPrintFromFileCopy�֐��ɓ��������B
 * ��Syslog�֐��ŁA�W���o�͂ɂ��o�͂���悤�ɂ����isyslogd���Ȃ��Ă��f
 *   �o�b�O�ł���j�B
 *
 * Revision 1.2  2001/02/05 17:38:02  tfuruka1
 * RCS�����������Ȃ����̂ŁA���������B�ߋ��̏C�����e�͈ȉ��̒ʂ�B
 * ���W�����͂���ǂݍ��񂾃o�C�g�����[���̏ꍇ�͏������s��Ȃ��悤�ɏC��
 *   �����B
 * ��Syslog�֐���ǉ������B��ȖړI�̓f�o�b�O�p�ł���B
 * ��GetLongBaseName�֐���ǉ������B
 *
 * Revision 1.1  2001/02/05 17:35:32  tfuruka1
 * Initial revision
 *
 *
 */
// (replace-regexp "[ \t]+$" "")
#include "ak2prs.h"
#include "ak2pr.h"

/* �p���T�C�Y�̈ꗗ */
static struct {
    LPTSTR cmdOpt;                              // �R�}���h�p�����[�^��
    short dmPaperSize;                          // DEVMODE�̃p�����[�^
    LPTSTR sComment;                            // �p���̐���
} devModePs[] = {
    {"A3", DMPAPER_A3, "A3�V�[�g, 297�~420�~�����[�g��"}, 
    {"A4", DMPAPER_A4, "A4�V�[�g, 210�~297�~�����[�g��"}, 
    {"A4SMALL", DMPAPER_A4SMALL, "A4 small �V�[�g, 210�~297�~�����[�g��"}, 
    {"A5", DMPAPER_A5, "A5�V�[�g, 148�~210�~�����[�g��"}, 
    {"B4", DMPAPER_B4, "B4�V�[�g, 250�~354�~�����[�g��"}, 
    {"B5", DMPAPER_B5, "B5�V�[�g, 182�~257�~�����[�g��"}, 
    {"LETTER", DMPAPER_LETTER, "Letter, 8 1/2�~11�C���`"}, 
    {"LEGAL", DMPAPER_LEGAL, "Legal, 8 1/2�~14�C���`"}, 
    {"CSHEET", DMPAPER_CSHEET, "C �V�[�g, 17�~22�C���`"}, 
    {"DSHEET", DMPAPER_DSHEET, "D �V�[�g, 22�~34�C���`"}, 
    {"ESHEET", DMPAPER_ESHEET, "E �V�[�g, 34�~44�C���`"}, 
    {"LETTERSMALL", DMPAPER_LETTERSMALL, "Letter Small, 8 1/2�~11�C���`"}, 
    {"TABLOID", DMPAPER_TABLOID, "Tabloid, 11�~17�C���`"}, 
    {"LEDGER", DMPAPER_LEDGER, "Ledger, 17�~11�C���`"}, 
    {"STATEMENT", DMPAPER_STATEMENT, "Statement, 5 1/2�~8 1/2�C���`"}, 
    {"EXECUTIVE", DMPAPER_EXECUTIVE, "Executive, 7 1/4�~10 1/2�C���`"}, 
    {"FOLIO", DMPAPER_FOLIO, "Folio, 8 1/2�~13�C���`"}, 
    {"QUARTO", DMPAPER_QUARTO, "Quarto, 215�~275�~�����[�g��"}, 
    {"10X14", DMPAPER_10X14, "10�~14�C���`�V�[�g"}, 
    {"11X17", DMPAPER_11X17, "11�~17�C���`�V�[�g"}, 
    {"NOTE", DMPAPER_NOTE, "Note, 8 1/2�~11�C���`"}, 
    {"ENV_9", DMPAPER_ENV_9, "#9 Envelope, 3 7/8�~8 7/8�C���`"}, 
    {"ENV_10", DMPAPER_ENV_10, "#10 Envelope, 4 1/8�~9 1/2�C���`"}, 
    {"ENV_11", DMPAPER_ENV_11, "#11 Envelope, 4 1/2�~10 3/8�C���`"}, 
    {"ENV_12", DMPAPER_ENV_12, "#12 Envelope, 4 3/4�~11�C���`"}, 
    {"ENV_14", DMPAPER_ENV_14, "#14 Envelope, 5�~11 1/2�C���`"}, 
    {"ENV_DL", DMPAPER_ENV_DL, "DL Envelope, 110�~220�~�����[�g��"}, 
    {"ENV_C5", DMPAPER_ENV_C5, "C5 Envelope, 162�~229�~�����[�g��"}, 
    {"ENV_C3", DMPAPER_ENV_C3, "C3 Envelope, 324�~458�~�����[�g��"}, 
    {"ENV_C4", DMPAPER_ENV_C4, "C4 Envelope, 229�~324�~�����[�g��"}, 
    {"ENV_C6", DMPAPER_ENV_C6, "C6 Envelope, 114�~162�~�����[�g��"}, 
    {"ENV_C65", DMPAPER_ENV_C65, "C65 Envelope, 114�~229�~�����[�g��"}, 
    {"ENV_B4", DMPAPER_ENV_B4, "B4 Envelope, 250�~353�~�����[�g��"}, 
    {"ENV_B5", DMPAPER_ENV_B5, "B5 Envelope, 176�~250�~�����[�g��"}, 
    {"ENV_B6", DMPAPER_ENV_B6, "B6 Envelope, 176�~125�~�����[�g��"}, 
    {"ENV_ITALY", DMPAPER_ENV_ITALY, "Italy Envelope, 110�~230�~�����[�g��"}, 
    {"ENV_MONARCH", DMPAPER_ENV_MONARCH, 
     "Monarch Envelope, 3 7/8�~7 1/2�C���`"}, 
    {"ENV_PERSONAL", DMPAPER_ENV_PERSONAL, 
     "6 3/4 Envelope, 3 5/8�~6 1/2�C���`"}, 
    {"FANFOLD_US", DMPAPER_FANFOLD_US, "US Std Fanfold, 14 7/8�~11�C���`"}, 
    {"FANFOLD_STD_GERMAN", DMPAPER_FANFOLD_STD_GERMAN, 
     "German Std Fanfold, 8 1/2�~12�C���`"}, 
    {"FANFOLD_LGL_GERMAN", DMPAPER_FANFOLD_LGL_GERMAN, 
     "German Legal Fanfold, 8 1/2�~13�C���`"}, 
    {NULL, 0, NULL}
};

/*--------------------------------------------------------------------
 * dwErr �Ŏw�肳�ꂽ�G���[�R�[�h�ɑΉ�����G���[���b�Z�[�W���֐��ɃV
 * �X�e�����b�Z�[�W�e�[�u�����\�[�X���猟�����Ĉꎞ�I�ȕ�����ւ̃|�C
 * ���^��ԋp����BdwErr �� GetLastError ���瓾���l���w�肷�鎖�Blpsz
 * �̓G���[���b�Z�[�W�֒ǉ����镶������w�肷��BAPI �������w�肷��B
 * *-------------------------------------------------------------------*/
LPCSTR WINAPI
GetLastErrorMessage(LPCSTR lpsz, DWORD dwErr)
{
#ifdef _WIN32_WCE
    static LPTSTR sz = "WindowsCE �ł� GetLastErrorMessage �͂݃T�|�[�g�ł�";
    return sz;
#else
    static char sz[1024];
    char szTmp[256];
    int i;

    if (!(i = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                            FORMAT_MESSAGE_IGNORE_INSERTS,
                            NULL, dwErr, 0, szTmp, sizeof(szTmp), NULL))) {
        strcpy(szTmp, "---");
    }
    else {
        szTmp[i] = '\0';
        for (i--; 0 <= i; i--) {
            if ('\n' == szTmp[i] || '\r' == szTmp[i]) {
                szTmp[i] = '\0';
            }
        }
    }
    wsprintf(sz, "[WIN32] %s: Error Code = %d(%#02x): %s",
             lpsz, dwErr, dwErr, szTmp);
    return (LPCTSTR)sz;
#endif
}

/*-------------------------------------------------------------------- 
 * ��Ɨp�f�B���N�g�����𓾂�B��ƃf�B���N�g�����͏��������Ă͂�����
 * ����B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetTempDirectoryName()
{
    static TCHAR szTempDirName[MAX_PATH] = "\0";
    char *p1;
    DWORD dwFA;
    int i;

    if (NULL == (p1 = getenv("TEMP"))) {
        Syslog(TRUE, "%s#%d: ���ϐ�TEMP��������܂���",
               __FILE__, __LINE__);
        return NULL;
    }

    for (i = 0; i < 100; i++) {
        sprintf(szTempDirName, "%s\\ak2prTempDir%d", p1, i);
        if (0xFFFFFFFF == (dwFA = GetFileAttributes(szTempDirName))) {
            if (0 != _mkdir(szTempDirName)) {
                continue;
            }
            break;
        }
        if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
            break;
        }
    }
    if (100 <= i) {
        Syslog(TRUE, "%s#%d: ��ƃf�B���N�g�����쐬�ł��܂���",
               __FILE__, __LINE__);
        szTempDirName[0] ='\0';
        return NULL;
    }
    return (LPCTSTR)szTempDirName;
}

/*--------------------------------------------------------------------
 * ��ƃt�@�C�����쐬����B����ɍ쐬�o�����ꍇ�̓t�@�C���|�C���^���
 * �p����B�쐬�ł��Ȃ������ꍇ�� NULL ��ԋp����Bmode �� fopen ��
 * mode �ƑS�������ł���B�܂�, �t�@�C�����g�p���I������� fclose �Ńt�@
 * �C�����N���[�Y��, lpszFileName �ŕԋp�����t�@�C�����폜���Ȃ���΂�
 * ��Ȃ��B
 * *-------------------------------------------------------------------*/
FILE * WINAPI
MakeTempFile(
    IN const char *mode,                        // ���[�h
    IN OUT LPTSTR lpszFileName                  // �쐬������ƃt�@�C����
    )
{
    static int iCnt = 0;
    FILE *fp;
    time_t t;
    TCHAR szFileName[MAX_PATH];
    LPCTSTR lpszDir;
    int i;

    // �t�@�C������ * �� \ �����܂܂�Ă���ƁA�������t�@�C�����쐬��
    // ���Ȃ��̂ŁA�K�؂ȕ����ɒu��������
    for (i = 0; *(lpszFileName + i); i++) {
        // �����̈�o�C�g�ڂ̏ꍇ�͎��̕������X�L�b�v����
        if (IsKanjiSJIS(*((LPBYTE)lpszFileName + i))) {
            i++;
            continue;
        }
        switch (*(lpszFileName + i)) {
        case '*':
        case '/':
        case ':':
        case '\\':
            *(lpszFileName + i) = '#';
            break;
        case ' ':
            *(lpszFileName + i) = '-';
            break;
        case '.':                               // Distiller���g���q��
                                                // ���f����̂�
            *(lpszFileName + i) = '_';
            break;
        }
    }

    if (!(lpszDir = GetTempDirectoryName())) {
        return NULL;
    }

    iCnt++;
    iCnt %= 256;

    t = time(NULL);

    strncpy(szFileName, lpszFileName, MAX_PATH);
    sprintf(lpszFileName, "%s\\%s~%08x_%d_XXXXXX",
            lpszDir, szFileName, t, iCnt);
    if (!_mktemp(lpszFileName)) {
        Syslog(TRUE, "%s#%d: ��ƃt�@�C�����쐬�ł��܂���",
               __FILE__, __LINE__);
        return NULL;
    }
    if (NULL == (fp = fopen(lpszFileName, mode))) {
        Syslog(TRUE, "%s#%d: ��ƃt�@�C��[%s]���I�[�v���ł��܂���(%s)",
               __FILE__, __LINE__, lpszFileName, strerror(0));
        return NULL;
    }
    return fp;
}

/*--------------------------------------------------------------------
 * �������g���i�[����Ă���f�B���N�g�����ւ̈ꎞ�I�ȃ|�C���^�𓾂�B
 * �ԋp����|�C���^�͈ꎞ�I�Ȃ��̂Ȃ̂�, �K�v�ȏꍇ�͕��ʂ��Ďg�p����
 * ���B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetMyDir(VOID)
{
    static TCHAR szBuf[1024];
    char *p1, *p2;

    // �t�@�C���̃t���p�X�𓾂�
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        int nErr = GetLastError();
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("GetModuleHandle", nErr));
        return NULL;
    }

    p1 = strrchr(szBuf, '\\');
    p2 = strrchr(szBuf, '/');
    p1 = ((ULONG)p1 > (ULONG)p2) ? p1 : p2;
    if (!p1) {
        strcpy(szBuf, "./");
    }
    else {
        *(p1 + 1) = '\0';
    }
    return (LPCTSTR)szBuf;
}

/*--------------------------------------------------------------------
 * Unix��basename�Ɠ����B�ԋp����|�C���^�͈ꎞ�I�ȃ|�C���^�Ȃ̂ŁA�K
 * ���R�s�[���Ďg�p����K�v������B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
BaseName(LPCTSTR lpszPath)
{
    LPCTSTR a, b;

    a = _mbsrchr(lpszPath, '\\');
    b = _mbsrchr(lpszPath, '/');

    if (a == b) {                               // ���������� NULL�����Ȃ�
        return lpszPath;
    }

    return ((ULONG)a > (ULONG)b ? a : b) + 1;
}
LPCTSTR WINAPI
GetLongBaseName(LPCTSTR lpszPath)
{
    HANDLE hFile;
    WIN32_FIND_DATA wfd;

    hFile = FindFirstFile(lpszPath, &wfd);
    if (INVALID_HANDLE_VALUE == hFile) {
        return BaseName(lpszPath);
    }
    FindClose(hFile);
    return wfd.cFileName;
}

/*--------------------------------------------------------------------
 * �v�����g�T�[�o���N�����Ă��邩�`�F�b�N����B�N�����Ă����ꍇ�� HWND
 * ��ԋp����B���N���̏ꍇ�� NULL ��Ԃ��B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
IsPrtServerEnable(VOID)
{
    return (BOOL)FindWindow(SV_CLASS, SV_CAPTION);
}

/*--------------------------------------------------------------------
 * �v�����g�T�[�o���N������B����ɋN���o�����ꍇ�� TRUE ��ԋp����B�N
 * ���ł��Ȃ������ꍇ�� FALSE ��Ԃ��B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
ExecutePrtServer(LPCTSTR lpszOption)
{
    PROCESS_INFORMATION  pi;                    // �v���Z�X���
    STARTUPINFO sui;                            // �N�����
    TCHAR szImageName[1024];                    // ���W���[����
    LPCTSTR lpszMyDir;                          // �����̃f�B���N�g��
    int i;                                      // �ėp

    if (IsPrtServerEnable())                    // ���ɋN�����Ă����ꍇ��
        return TRUE;                            // ����ɋN���������̂Ƃ���

    if (!(lpszMyDir = GetMyDir())) {            // �������g�̃f�B���N�g���𓾂�
        return FALSE;
    }

    // ���s�t�@�C�����쐬����
    sprintf(szImageName, "%s/%s %s", lpszMyDir, SV_EXE_NAME, lpszOption);

    // �v���Z�X���ƋN�����̏�����
    memset(&sui, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    sui.cb = sizeof(STARTUPINFO);
    sui.dwFlags = STARTF_USESHOWWINDOW;
    sui.wShowWindow = SW_SHOWNORMAL;

    if (!CreateProcess(NULL, szImageName, NULL, NULL, FALSE,
                       0, NULL, NULL, &sui, &pi )) {
        int nErr = GetLastError();
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("CreateProcess", nErr));
        return FALSE;
    }

    // WaitForInputIdle(pi.hProcess, INFINITE);
    // �E�B���h�E�����X�����܂ő҂�
    for (i = 0; i < 10; i++) {
        if (IsPrtServerEnable()) {
            break;
        }
        Sleep(1000);
    }
    // �v���Z�X�n���h���͕K�v�����̂ŕ���
    CloseHandle(pi.hProcess);

    if (pi.hThread) {                           // �X���b�h�n���h��������
        CloseHandle(pi.hThread);
    }

    // ������x�T�[�o�[���`�F�b�N����
    if (!IsPrtServerEnable()) {
        Syslog(TRUE, "%s#%d: �T�[�o�̋N���Ɏ��s�����悤�ł�",
               __FILE__, __LINE__);
        return FALSE;
    }
    return TRUE;                                // ����N��
}

/*--------------------------------------------------------------------
 * �v�����g�T�[�o�ֈ�����𑗐M����B
 * *-------------------------------------------------------------------*/
static BOOL WINAPI
SendPrintData(
    HWND hWnd,                                  // ���M���̃n���h��
    PPRT_INFO pPrtInfo                          // ������
    )
{
    HWND hWndTo;                                // ���M��̃n���h��
    COPYDATASTRUCT cds;                         // ���M�f�[�^

    if (!ExecutePrtServer(NULL)) {             // �v�����^�T�[�o���N��
        return FALSE;                           // �T�[�o�����Ȃ�
    }

    // DLL��Varsion�s��v��h����, �^�C���X�^���v�𖄂߂�
    strcpy(pPrtInfo->szTimeStamp, TIMESTAMP);
    strcpy(pPrtInfo->szTimeStamp1, TIMESTAMP);

    // �T�[�o�̃n���h���𓾂�
    if (!(hWndTo = FindWindow(SV_CLASS, SV_CAPTION))) {
        int nErr = GetLastError();
        Syslog(TRUE, "%s#%d: %s", __FILE__, __LINE__,
               GetLastErrorMessage("FindWindow", nErr));
        return FALSE;
    }

    // ���M�f�[�^��ݒ肷��
    cds.dwData = 0;
    cds.cbData = sizeof(PRT_INFO);
    cds.lpData = (LPVOID)pPrtInfo;

    // ������𑗐M����
    return SendMessage(hWndTo, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cds);
}

/*--------------------------------------------------------------------
 * �W�����͂̓��e��ǂݍ��݃v�����g�T�[�o�ֈ�����𑗐M����B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromStdin(
    HWND hWnd,                                  // �n���h��
    LPCTSTR lpszTitle,                          // �^�C�g�����w�莞��"stdin"
    int nNumOfUp,                               // �i�g��
    int nTab,                                   // �^�u��
    double fFontSize,                           // �t�H���g�T�C�Y
    int nType,                                  // ����f�[�^�^�C�v
    int nOrientation,                           // �p���̌���
    short dmPaperSize,                          // �p���T�C�Y
    int bNum                                    // �s�ԍ��̈��
    )
{
    PRT_INFO PrtInfo;                           // �v�����g�t�@�C�����
    FILE *fp;                                   // �t�@�C���|�C���^
    TCHAR szBuf[1024];                          // �W�����͗p�o�b�t�@
    BOOL bNotZero = FALSE;                      // �t�@�C���T�C�Y�[��

    // �v�����g�t�@�C�����̏�����
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    if (lpszTitle) {
        strncpy(PrtInfo.szTitle, lpszTitle, 255);
    }
    else {
        strcpy(PrtInfo.szTitle, "stdin");
    }

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = nType;
    PrtInfo.fFontSize = fFontSize;
    PrtInfo.nOrientation = nOrientation;
    PrtInfo.dmPaperSize = dmPaperSize;
    PrtInfo.bPreView = FALSE;
    PrtInfo.bNum = bNum;                        // �s�ԍ��̈��(-1:�T�[�o)

    // ��ƃt�@�C�����쐬����
    strcpy(PrtInfo.szFileName, PrtInfo.szTitle); 
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }

    // �W�����͂���ǂݍ���
    while (fgets(szBuf, 1024, stdin)) {
        fprintf(fp, "%s", szBuf);
        bNotZero++;
    }
    fclose(fp);

    if (!bNotZero) {
        unlink(PrtInfo.szFileName);
        Syslog(TRUE, "%s", "�t�@�C���T�C�Y��0�Ȃ̂ŃL�����Z�����܂�");
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * �w�肳�ꂽ�t�@�C�����ꎞ�t�@�C���֕��ʂ��C�v�����g�T�[�o�ֈ�����
 * �𑗐M����B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromFileCopy(
    HWND hWnd,                                  // �n���h��
    LPCTSTR lpszTitle,                          // �^�C�g�����w�莞��"stdin"
    LPCTSTR lpszFileName,                       // �R�s�[���t�@�C����
    int nNumOfUp,                               // �i�g��
    int nTab,                                   // �^�u��
    double fFontSize,                           // �t�H���g�T�C�Y
    int nType,                                  // ����f�[�^�^�C�v
    int nOrientation,                           // �p���̌���
    short dmPaperSize,                          // �p���T�C�Y
    int bNum                                    // �s�ԍ����
    )
{
    PRT_INFO PrtInfo;                           // �v�����g�t�@�C�����
    FILE *fp;                                   // �t�@�C���|�C���^
    DWORD dwFA;                                 // �t�@�C�����

    // �t�@�C���̑��݃`�F�b�N���s��
    if (0xFFFFFFFF == (dwFA = GetFileAttributes(lpszFileName))) {
        LPCTSTR p;
        dwFA = GetLastError();
        p = GetLastErrorMessage("GetFileAttributes()", dwFA);
        Syslog(TRUE, "%s#%d: %s %s", __FILE__, __LINE__, p, lpszFileName);
        return FALSE;
    }

    // �f�B���N�g���̏ꍇ�̓G���[
    if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
        Syslog(TRUE, "%s#%d: %s�̓f�B���N�g���ł�",
               __FILE__, __LINE__, lpszFileName);
        return FALSE;
    }

    // �v�����g�t�@�C�����̏�����
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    if (lpszTitle) {
        strncpy(PrtInfo.szTitle, lpszTitle, 255);
    }
    else {
        strncpy(PrtInfo.szTitle, GetLongBaseName(lpszFileName), 255);
    }

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = nType;
    PrtInfo.fFontSize = fFontSize;
    PrtInfo.nOrientation = nOrientation;
    PrtInfo.dmPaperSize = dmPaperSize;
    PrtInfo.bPreView = FALSE;
    PrtInfo.bNum = bNum;                        // �s�ԍ����(Boolean�ł͂Ȃ�)

    // ��ƃt�@�C�����쐬����
    strcpy(PrtInfo.szFileName, PrtInfo.szTitle); 
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }
    fclose(fp);

    // �t�@�C���𕡎ʂ���
    if (!CopyFile(lpszFileName, PrtInfo.szFileName, FALSE)) {
        Syslog(TRUE, "%s#%d: %s %s", __FILE__, __LINE__,
               GetLastErrorMessage("GetFileAttributes()",
                                   GetLastError()),
               PrtInfo.szFileName);
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * UNIX��Syslog�̊ȈՔŁB���debug.local7�����o�͂��܂���B
 * *-------------------------------------------------------------------*/
VOID WINAPI
    Syslog(BOOL bStdOut,                        // T:stdout�ɂ��o��
           LPCSTR lpstr,                        // ����printf�Ɠ���
           ...                                  // ����
    )
{
    WSADATA wsaData;
    SOCKET finet;
    PSERVENT sp;
    SOCKADDR_IN sin;
    LPHOSTENT lpHost;
    va_list args;                               // �����W�J�p
    char szLine[1024 * 64], szBuf[1024], *p;

    // ���W���[�����̃t���p�X�𓾂�
    if (!GetModuleFileName(GetModuleHandle(NULL), szBuf, 1024)) {
        return;
    }
    // �t�@�C�������������؂�o��
    if (NULL == (p = strrchr(szBuf, '\\'))) {
        return;
    }
    // FACILITY = 23, LEVEL = 7
    sprintf(szLine, "<%d>[%s] ", 7 + 23 * 8, p + 1);

    // ������������ɍ����Đ��`����
    va_start(args, lpstr);
    vsprintf(szLine + strlen(szLine), lpstr, args);
    va_end(args);

    if (bStdOut) {
        printf("%s\n", szLine);
    }

    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return;
	}

    if (0 > (finet = socket(AF_INET, SOCK_DGRAM, 0))) {
        return;
    }

    lpHost = gethostbyname("localhost");

    if (NULL == (sp = getservbyname("syslog", "udp"))) {
        return;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(sp->s_port);
    sin.sin_addr = *((LPIN_ADDR)*lpHost->h_addr_list);

    sendto(finet, szLine, strlen(szLine), 0, (PSOCKADDR)&sin,
           sizeof(SOCKADDR));
    closesocket(finet);
    WSACleanup();
}

/*-------------------------------------------------------------------- 
 * �w��\�ȗp���T�C�Y�̈ꗗ��ԋp����B
 * 
 * *-------------------------------------------------------------------*/
LPTSTR WINAPI
GetPaperSizeUsageMessage()
{
    int i;
    static TCHAR buf[4096];

    strcpy(buf, "�p���T�C�Y�͈ȉ��̉��ꂩ���w�肵�ĉ������B"
           "�A��, �S�ẴI�v�V�������S�Ẵv�����^��"
           "�L���Ƃ͌���܂���B\n\n"
           "�I�v�V����          ����\n"
           "------------------- ------------------------------\n");
    for (i = 0; devModePs[i].cmdOpt; i++) {
        sprintf(buf + strlen(buf), "%-20s%s\n", 
                devModePs[i].cmdOpt, devModePs[i].sComment);
        if (strlen(buf) > (4096 - 80)) {
            break;
        }
    }
    return buf;
}

/*-------------------------------------------------------------------- 
 * �R�}���h�I�v�V�����Ŏw�肵���p���T�C�Y����ADEVMODE�Ŏg�p����p���T
 * �C�Y�̒萔�𓾂�B�p���T�C�Y���s���̏ꍇ��0��ԋp����B
 * *-------------------------------------------------------------------*/
short WINAPI
GetPaperSizeDevMode(LPTSTR cmdOpt)
{
    int i;
    for (i = 0; devModePs[i].cmdOpt; i++) {
        if (0 == stricmp(cmdOpt, devModePs[i].cmdOpt)) {
            return devModePs[i].dmPaperSize;
        }
    }
    return 0;
}
/*--------------------------------------------------------------------
 * DEVMODE�Ŏg�p����p���T�C�Y�̒萔����p���T�C�Y�̐����𓾂�B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetPaperSizeComment(short dmPaperSize)
{
    int i;

    if (0 == dmPaperSize) {
        return (LPCTSTR)"�f�t�H���g";
    }
    for (i = 0; devModePs[i].cmdOpt; i++) {
        if (dmPaperSize == devModePs[i].dmPaperSize) {
            return (LPCTSTR)devModePs[i].sComment;
        }
    }
    return (LPCTSTR)"�s��";
}
/*--------------------------------------------------------------------
 * PostScript �t�@�C������ATitle�𓾂�B
 * *-------------------------------------------------------------------*/
#define PS_TITLE_STR "%%Title:"

LPCTSTR WINAPI
    GetPSTitle(LPCTSTR lpszFile,                // PS�t�@�C����
               LPTSTR lpszTitle,                // �^�C�g���i�[�G���A
               int cbMax                        // �i�[�ő啶����
               )
{
    FILE *fp;
    TCHAR szBuf[1024], *p;

    memset(lpszTitle, 0, cbMax);

    if (NULL == (fp = fopen(lpszFile, "rt"))) {
        Syslog(TRUE, "%s#%d: PS�t�@�C��[%s]���I�[�v���ł��܂���(%s)",
               __FILE__, __LINE__, lpszFile, strerror(0));
        return NULL;        
    }

    while (fgets(szBuf, 1024, fp)) {
        if (p = strstr(szBuf, PS_TITLE_STR)) {
            break;
        }
    }
    fclose(fp);
    if (p) {
        strncpy(lpszTitle, p + strlen(PS_TITLE_STR), cbMax);
    }
    else {
        strncpy(lpszTitle, "�^�C�g���s��", cbMax);
    }
    TrimString(lpszTitle);
    Syslog(FALSE, "DBG:GetPSTitle[%s]", lpszTitle);
    return (LPCTSTR)lpszTitle;
}
