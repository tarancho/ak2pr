/* -*- mode: C++; coding: sjis-dos; -*-
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o���̃w�b�_�t�@�C��
 *
 * $Id: ak2prs.h,v 1.6 2001/09/28 15:49:05 tfuruka1 Exp $
 * $Log: ak2prs.h,v $
 * Revision 1.6  2001/09/28 15:49:05  tfuruka1
 * �yVersion 1.4 �ɂ��܂����zak2pr.exe, ak2prW.exe�ň������f�o�b�O�p�Ƀ_
 * ���v���Ă��鏈���Ń������j����������Ă������΂��ȃo�O���C�����܂���
 * �i�������R�w��F�Ⴆ�΁A�t�@�C����999���炢�w�肷��ƁA��������j
 * �󂵂Ă����j�B
 *
 * Revision 1.5  2001/08/19 09:03:25  tfuruka1
 * ���������� Version 1.3�ɂ��܂���  ����������
 * RCS��Revision�ƈ�v������͖̂������i�p�ɂɏオ��߂���̂Łj����̂ŁA
 * ����ς�A�蓮�ŏグ�鎖�ɂ����B
 *
 * 1.2����̖ʂȍ�����
 * ���e���|�����t�@�C���̕s��(27��)���������Ȃ��悤�ɂ����B
 * ���N���C�A���g�ƃT�[�o�Ń��[�L���O�f�B���N�g�����قȂ��Ă���ꍇ�A���
 *   �t�@�C���𑊑΃p�X�Ŏw�肵���ꍇ�ɁA�T�[�o���Ńt�@�C�������o�ɂȂ��
 *   ����C���B
 * ��PostScript�t�@�C���ɑΉ�����(�Ƃ����Ă��A���ۂ́AAcrobat Distiller���A
 *   GhostScript�Ƀt�H���[�h���邾��)�B
 *
 * Revision 1.4  2001/08/19 04:37:45  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.3  2001/08/18 17:05:03  tfuruka1
 * VERSION���グ��̂�Y��Ă��E�E�E�ʓ|�Ȃ̂ŁARCS��Revision���g�p�����
 * ���ɂ����B
 *
 * Revision 1.2  2001/08/18 16:34:06  tfuruka1
 * ��PRT_INFO�\���̂���bDelete�����o���폜�����B
 * ��DeleteQueue�֐����V�K�ǉ����ꂽ�̂ŁA�v���g�^�C�v�錾��ǉ������B
 *
 * Revision 1.1  2001/02/05 17:16:50  tfuruka1
 * Initial revision
 *
 */
// (replace-regexp "/\\*\\(.+\\)\\*/" "//\\1")
// (replace-regexp "[ \t]+$" "")

#ifndef _AK2PRS_H_
#define _AK2PRS_H_

#define TIMESTAMP "Time-stamp: <2001-09-29 00:47:02 tfuruka1>"
#define VERSION   "Version 1.4"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <mbstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

#include "resource.h"

#include "ak2pr.h"

#define IsShellTray() FindWindow("Shell_TrayWnd", NULL)

#define SV_DBGCLASS "ak2ps�̂悤�Ȃ��� - ak2pr(Server-Detail)"
#define SV_CLASS    "ak2ps�̂悤�Ȃ��� - ak2pr(Server)"
#define SV_CAPTION  "ak2pr Server"

#define SV_EXE_NAME "ak2prSV.EXE"

#define COPYRIGHT   "ak2pr " VERSION\
" Copyright 1997-2001 By T.Furukawa\n(tfuruka1@nifty.com)" TIMESTAMP

// 1�o�C�g�ڂ������R�[�h���ۂ��`�F�b�N����}�N��
#define IsKanjiSJIS(c)  (((c) >= 0x81 && (c) <= 0x9f) ||\
                         ((c) >= 0xe0 && (c) <= 0xfc))

// �t�H���g��
#define FN_MSPM     TEXT("�l�r �o����")
#define FN_MSM      TEXT("�l�r ����")
#define FN_MSPG     TEXT("�l�r �o�S�V�b�N")
#define FN_MSG      TEXT("�l�r �S�V�b�N")
#define FN_COU      TEXT("Courier New")
#define FN_ARIAL    TEXT("Arial")

// ConvX2Dt, ConvDt2X �̃t���O
#define CX_CM       1                           // cm�Ŏ擾
#define CX_MM       2                           // mm�Ŏ擾
#define CX_PT       3                           // point�Ŏ擾

// GetPrtCenter �̃t���O(�_���a�w��)
#define GPC_W       1                           // ��
#define GPC_H       2                           // �c
#define GPC_IM      4                           // �}�[�W������

#define HANDLE_WM_REALCLOSE(hwnd, wParam, lParam, fn) \
    ((BOOL)(fn)((hwnd)))
#define HANDLE_WM_COPYDATA(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam), \
          (PCOPYDATASTRUCT)lParam), 0L)
#define HANDLE_WM_TASKMENU(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))
#define HANDLE_WM_SUSPEND(hwnd, wParam, lParam, fn) \
    ((LRESULT)(fn)((hwnd), (wParam), lParam))

// ���[�U���b�Z�[�W
enum {WM_TASKMENU = WM_USER + 1, WM_REALCLOSE, WM_SUSPEND};

// ������ł��B���̍\���̂ɒl��ݒ肵��WM_COPYDATA��SV_CLASS,
// SV_CAPTION�̃E�C���h�E�����L���Ă���v���Z�X��SendMessage����ƈ��
// ���s���܂��B
typedef struct _PrtInfo{
    CHAR szFileName[MAX_PATH];                  // �t�@�C����
    CHAR szTitle[256];                          // �^�C�g��������
    int nNumOfUp;                               // �i�g��
    int nTab;                                   // TAB��
    int nType;                                  // ����^�C�v
    CHAR szTimeStamp[64];
    CHAR szTimeStamp1[64];
    int nBaseLine;                              // 0: Non, 1:E, 2:J, 9:A
    BOOL bNum;                                  // T: �s�ԍ����
    BOOL bKeisen;                               // T: �r���A��
    BOOL bNoRcvHeader;                          // Received�w�b�_���󎚂��Ȃ�
    BOOL bColor;                                // T: Color���
    double fFontSize;                           // �t�H���g�T�C�Y --- Point
} PRT_INFO, *PPRT_INFO;

// nType �ɐݒ肷��l�͈ȉ��̒ʂ�(�N���C�A���g���ł��Q�Ƃ������̂ŁA
// ak2pr.h�Ɉړ�), �f�t�H���g��PT_TEXT


// �X���b�h�ԃ��[���{�b�N�X
typedef struct {
    HWND hWnd;                                  // �n���h��
    HDC hDC;                                    // �f�o�C�X�n���h��
    HGLOBAL hDevMode;                           // �v�����^�f�o�C�X���[�h
    HGLOBAL hDevNames;                          // �v�����^�f�o�C�X�l�[��
    HANDLE hThread;                             // �X���b�h�n���h��
    DWORD idThread;                             // �X���b�hID
    BOOL bRun;                                  // T:�X���b�h���s���
    BOOL bStop;                                 // T:�����~���
    PRT_INFO PrtInfo;                           // ������
    // ----- �ȉ��̓��[������̏��
    TCHAR szFrom[128];                          // ���o�l
    TCHAR szDate[128];                          // ��M����
    TCHAR szReference[128];                     // ���t�@�����X
    TCHAR szMessageID[128];                     // ���b�Z�[�WID
    // ----- �ȉ���PostScript������̏��
    TCHAR szAcrobat[MAX_PATH];                  // Distiller in �t�H���_
    TCHAR szGsPath[MAX_PATH];                   // GhostScript �p�X
    TCHAR szGsOpt[512];                         // GhostScript option
} MAILBOX, *PMAILBOX;

// �O����`
extern PRT_INFO g_PrtInfo;                      // �f�t�H���g�̈�����
extern MAILBOX g_MailBox;                       // �X���b�h�ԃ��[���{�b�N�X
extern CRITICAL_SECTION g_csCriticalSection;    // �N���e�B�J���Z�N�V����

// �v���g�^�C�v
HWND CreateApplication(HANDLE hInstance);

LRESULT CALLBACK
MainWndProc(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    );

LRESULT CALLBACK
MainWndProcDetail(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    UINT uMsg,                                  // ���b�Z�[�W
    WPARAM wParam,                              // �p�����[�^
    LPARAM lParam                               // �p�����[�^
    );

VOID PrintThread(LPDWORD lpIDThread);
VOID SetupPrtStyle(HWND hWnd);

BOOL
EnQueue(
    HWND hWnd,                                  // �n���h��
    PPRT_INFO pPrtInfo                          // ������
    );

BOOL
DeQueue(
    HWND hWnd,                                  // �n���h��
    PPRT_INFO pPrtInfo                          // ������
    );
int
DeleteQueue(
    HWND hWnd,                                  // �n���h��
    BOOL bForce                                 // T:�S�č폜
    );
BOOL
SetupPrinter(
    HWND hWnd,                                  // �n���h��
    HGLOBAL *lphDevNames,
    HGLOBAL *lphDevMode
    );
HPEN
CreatePrtPen(
    UINT nStyle,                                // �y���̃X�^�C��
    LONG nWidth,                                // ����
    COLORREF crColor                            // �F
    );
HFONT
CreatePrtFont(
    LPTSTR lpszFontName,                        // �t�H���g��
    int nHeight,                                // �t�H���g�̍���
    int nWeight,                                // �t�H���g�̑���
    BOOL bItalic,                               // T: �C�^���b�N
    BOOL bUnderline,                            // T: ����
    BOOL bStrikeOut,                            // T: �ł�������
    BOOL bSjis                                  // T: ���{��
    );
LPTSTR
ufgets(
    LPTSTR szBuf,                               // �i�[�o�b�t�@
    int cbMax,                                  // �ő啶����
    FILE *fp,                                   // �t�@�C���|�C���^
    int nTab                                    // �^�u�X�g�b�v
    );
/* -------------------------------------------------------------------
 * lpszStr�v�����^�֏o�͂���B�t�H���g�͎w�肳�ꂽ�p�����[�^�����ɍ쐬
 * ����B
 * *-----------------------------------------------------------------*/
BOOL SetFontAndPrint(
    LPTSTR lpszStr,                             // �o�͕�����
    LPTSTR lfFaceName,                          // �t�H���g��
    UINT lfHeight,                              // �t�H���g�̍���
    UINT lfWeight,                              // �t�H���g�̃E�G�C�g
    BOOL lfItalic,                              // T:�C�^���b�N
    BOOL lfUnderline,                           // T:����
    BOOL lfStrikeout,                           // T:�ł�������
    BOOL bJapan                                 // T:���{��t�H���g
    );
LPTSTR TrimString(LPSTR lpszStr);
LPTSTR TrimLeft(LPTSTR lpszStr);
LPTSTR TrimRight(LPTSTR lpszStr);
void DrawTombow(void);
void DoTestPrint(void);
void GetDefaultPrtInfo(void);
void SetDefaultPrtInfo(void);
int GetPrtWd(void);
int GetPrtHt(void);
int GetPrtMgW(void);
int GetPrtMgH(void);
int GetPrtDpiW(void);
int GetPrtDpiH(void);
int GetPrtCenter(int flg);
int GetPrtBasePoint(void);
double ConvDt2X(int value, int nDpi, int flg);
int ConvX2Dt(double value, int nDpi, int flg);
BOOL BeginDocument(void);
BOOL BeginPage(void);
BOOL EndPageDocument(void);
BOOL EndDocument(void);
BOOL PutsPrinter(LPTSTR szBuf);
void PrintMail(void);
void PrintText(void);
#endif
