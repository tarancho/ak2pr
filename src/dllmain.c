/* -*- mode: C++; coding: sjis-dos; -*-
 * Time-stamp: <2000-11-05 15:05:35 tfuruka1>
 *
 * ak2ps �̂悤�Ȃ��̂̋��� DLL
 *
 * $Id: dllmain.c,v 1.1 2001/02/05 17:35:32 tfuruka1 Exp $
 * $Log: dllmain.c,v $
 * Revision 1.1  2001/02/05 17:35:32  tfuruka1
 * Initial revision
 *
 * Revision 1.3  2000/11/05 06:07:03  tfuruka1
 * �W�����͂���ǂݍ��񂾃o�C�g�����[���̏ꍇ�͏������s��Ȃ��悤�ɏC����
 * ���B
 *
 * Revision 1.2  2000/11/04 13:52:09  tfuruka1
 * Syslog�֐���ǉ������B��ȖړI�̓f�o�b�O�p�ł���B
 *
 * Revision 1.1  2000/11/04 13:50:26  tfuruka1
 * Initial revision
 *
 * Revision 1.3  2000/10/22 11:21:55  tfuruka1
 * RCS�����������Ȃ����̂ŕ��������B
 *
 * Revision 1.2  2000/10/22 09:37:57  tfuruka1
 * GetLongBaseName�֐���ǉ������B
 *
 * Revision 1.1  1999/12/16 04:25:57  tfuruka1
 * Initial revision
 *
 */
#include "ak2prs.h"
#include "ak2pr.h"

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
 * ��ƃt�@�C�����쐬����B����ɍ쐬�o�����ꍇ�̓t�@�C���|�C���^���
 * �p����B�쐬�ł��Ȃ������ꍇ�� NULL ��ԋp����Bmode �� fopen �� 
 * mode �ƑS�������ł���B�܂�, �t�@�C�����g�p���I������� fclose �Ńt�@
 * �C�����N���[�Y��, lpszFileName �ŕԋp�����t�@�C�����폜���Ȃ���΂�
 * ��Ȃ��B
 * *-------------------------------------------------------------------*/
FILE * WINAPI
MakeTempFile(
    IN const char *mode,                        // ���[�h
    OUT LPTSTR lpszFileName                     // �쐬������ƃt�@�C����
    )
{
    FILE *fp;
    char *p1;

    if (NULL == (p1 = getenv("TEMP"))) {
        MessageBox(NULL, "���ϐ�TEMP��������܂���",
                   "MakeTempFile()", MB_ERROR);
        return NULL;
    }
    sprintf(lpszFileName, "%s/ak2prXXXXXX", p1);
    if (!_mktemp(lpszFileName)) {
        MessageBox(NULL, "��ƃt�@�C�����쐬�ł��܂�",
                   "MakeTempFile()", MB_ERROR);
        return NULL;
    }
    if (NULL == (fp = fopen(lpszFileName, mode))) {
        MessageBox(NULL,
                   "��ƃt�@�C�����I�[�v���쐬�ł��܂��� MakeTempFile()",
                   strerror(0), MB_ERROR);
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
        MessageBox(NULL, GetLastErrorMessage("GetModuleHandle", nErr), 
                   "GetMyDir()", MB_ERROR);
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
static BOOL WINAPI
ExecutePrtServer(VOID)
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
    sprintf(szImageName, "%s/%s", lpszMyDir, SV_EXE_NAME);

    // �v���Z�X���ƋN�����̏�����
    memset(&sui, 0, sizeof(STARTUPINFO));
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    sui.cb = sizeof(STARTUPINFO);
    sui.dwFlags = STARTF_USESHOWWINDOW;
    sui.wShowWindow = SW_SHOWNORMAL;

    if (!CreateProcess(NULL, szImageName, NULL, NULL, FALSE,
                       0, NULL, NULL, &sui, &pi )) {
        int nErr = GetLastError();
        MessageBox(NULL, GetLastErrorMessage("CreateProcess", nErr),
                   "ExecutePrtServer()", MB_ERROR);
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
        MessageBox(NULL, "�T�[�o�̋N���Ɏ��s�����悤�ł�",
                   "ExecutePrtServer()", MB_ERROR);
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

    if (!ExecutePrtServer()) {                  // �v�����^�T�[�o���N��
        return FALSE;                           // �T�[�o�����Ȃ�
    }

    // DLL��Varsion�s��v��h����, �^�C���X�^���v�𖄂߂�
    strcpy(pPrtInfo->szTimeStamp, TIMESTAMP);
    strcpy(pPrtInfo->szTimeStamp1, TIMESTAMP);

    // �T�[�o�̃n���h���𓾂�
    if (!(hWndTo = FindWindow(SV_CLASS, SV_CAPTION))) {
        int nErr = GetLastError();
        MessageBox(hWnd, GetLastErrorMessage("FindWindow", nErr),
                   "SendPrintData()", MB_ERROR);
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
    BOOL bMail                                  // T: ����f�[�^�̓��[��[mail]
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
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = TRUE;
    PrtInfo.fFontSize = fFontSize;

    // ��ƃt�@�C�����쐬����
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
        Syslog("%s", "�t�@�C���T�C�Y��0�Ȃ̂ŃL�����Z�����܂�");
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
    BOOL bMail                                  // T: ����f�[�^�̓��[��[mail]
    )
{
    PRT_INFO PrtInfo;                           // �v�����g�t�@�C�����
    FILE *fp;                                   // �t�@�C���|�C���^

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
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = TRUE;
    PrtInfo.fFontSize = fFontSize;

    // ��ƃt�@�C�����쐬����
    if (NULL == (fp = MakeTempFile("wt", PrtInfo.szFileName))) {
        return FALSE;
    }
    fclose(fp);

    // �t�@�C���𕡎ʂ���
    if (!CopyFile(lpszFileName, PrtInfo.szFileName, FALSE)) {
        MessageBox(hWnd, GetLastErrorMessage("GetFileAttributes()",
                                             GetLastError()),
                   PrtInfo.szFileName, MB_ERROR);
        return FALSE;
    }

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * �t�@�C���̑��݃`�F�b�N���s���v�����g�T�[�o�ֈ�����𑗐M����B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromFile(
    HWND hWnd,                                  // �n���h��
    LPCTSTR lpszFileName,                       // �t�@�C����
    int nNumOfUp,                               // �i�g��
    int nTab,                                   // �^�u��
    double fFontSize,                           // �t�H���g�T�C�Y
    BOOL bMail                                  // T: ����f�[�^�̓��[��[mail]
    )
{
    PRT_INFO PrtInfo;                           // �v�����g�t�@�C�����
    TCHAR szBuf[1024];                          // �W�����͗p�o�b�t�@
    DWORD dwFA;                                 // �t�@�C�����

    // �t�@�C���̑��݃`�F�b�N���s��
    if (0xFFFFFFFF == (dwFA = GetFileAttributes(lpszFileName))) {
        LPCTSTR p;
        dwFA = GetLastError();
        p = GetLastErrorMessage("GetFileAttributes()", dwFA);
        if (hWnd) {
            MessageBox(hWnd, p, __FILE__, MB_ICONSTOP);
        }
        else {
            fprintf(stderr, "%s\n", p);
        }
        return FALSE;
    }

    // �f�B���N�g���̏ꍇ�̓G���[
    if (dwFA & FILE_ATTRIBUTE_DIRECTORY) {
        sprintf(szBuf, "%s�̓f�B���N�g���ł�", lpszFileName);
        if (hWnd) {
            MessageBox(hWnd, lpszFileName, __FILE__, MB_ICONSTOP);
        }
        else {
            fprintf(stderr, "%s\n", lpszFileName);
        }
        return FALSE;
    }

    // �v�����g�t�@�C�����̏�����
    memset((LPVOID)&PrtInfo, 0, sizeof(PRT_INFO));
    strncpy(PrtInfo.szTitle, GetLongBaseName(lpszFileName), 255);

    PrtInfo.nNumOfUp = nNumOfUp;
    PrtInfo.nTab = nTab;
    PrtInfo.nType = bMail ? PT_MAIL : PT_TEXT;
    PrtInfo.bDelete = FALSE;                    // ���t�@�C���͍폜���Ȃ�
    PrtInfo.fFontSize = fFontSize;
    strcpy(PrtInfo.szFileName, lpszFileName);

    return SendPrintData(hWnd, &PrtInfo);
}

/*--------------------------------------------------------------------
 * UNIX��Syslog�̊ȈՔŁB���debug.local7�����o�͂��܂���B
 * *-------------------------------------------------------------------*/
VOID WINAPI
Syslog(
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    )
{
    WSADATA wsaData;
    SOCKET finet;
    PSERVENT sp;
    SOCKADDR_IN sin;
    LPHOSTENT lpHost;
    va_list args;                               // �����W�J�p
    char szLine[8192], szBuf[1024], *p;

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
