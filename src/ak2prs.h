/* -*- mode: C++; coding: sjis-dos; -*-
 *
 * �uak2ps�̂悤�Ȃ��́v�̃T�[�o���̃w�b�_�t�@�C��
 *
 * $Id: ak2prs.h,v 1.25 2004/12/23 08:11:56 tfuruka1 Exp $
 * $Log: ak2prs.h,v $
 * Revision 1.25  2004/12/23 08:11:56  tfuruka1
 * �V���O�����C�����(�H�~�o����������������Ȃ�)�ɑΉ����܂����B�Ƃ肠��
 * ���A�T�[�o���̐ݒ�݂̂ł��B
 *
 * Revision 1.24  2004/08/21 11:12:49  tfuruka1
 * Version��Version�t�@�C������擾����悤�ɂ��܂����B
 *
 * Revision 1.23  2004/08/21 01:01:01  tfuruka1
 * �e�L�X�g����ɉ����āu�s�ԁv�Ɓu�r���A���v���L���ɂȂ�悤�ɂ��܂����B
 *
 * Revision 1.22  2004/06/18 00:57:43  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.21  2004/01/19 05:40:05  tfuruka1
 * �t�H���g�����w��o����悤�ɂȂ������Ɋւ���C�����s���܂����B
 *
 * Revision 1.20  2004/01/12 09:57:53  tfuruka1
 * �Z�ӒԂ��ƒ��ӒԂ��ɑΉ����܂����B����ɍ��킹��Verison�ԍ����ύX����
 * �����B
 *
 * Revision 1.19  2004/01/11 11:07:35  tfuruka1
 * Version�ԍ��̕ύX�̂݁B�o�b�t�@���Ƀ��_�C���N�g�L�����܂܂�Ă����ꍇ
 * �ɍ�ƃt�@�C���̍쐬�Ɏ��s��������C��(dllmain.c)�B
 *
 * Revision 1.18  2003/04/11 21:38:02  tfuruka1
 * �� Version�ԍ��̕ύX�̂� --- �I�����Ă��Ȃ��������A�������j����N����
 *    �\�����������������C�� (dllmain.c) ---
 *
 * Revision 1.17  2003/03/29 12:56:15  tfuruka1
 * �� ���J�pVersion�̎擾 ������ Version 2.2 ������
 *
 * Revision 1.16  2003/03/15 14:45:58  tfuruka1
 * �� �v���r���[���̈���ݒ�ɑΉ����邽�߂̏C���ɔ����萔�̒ǉ��ƃv���g
 *    �^�C�v�̏C�����s�����B
 *
 * Revision 1.15  2003/03/14 16:03:08  tfuruka1
 * �� PostScript�֘A�̏���MAILBOX����PRT_INFO�ֈړ������B�����Aak2pr��
 *    �R�}���h�I�v�V�����Ƃ��āA�w��\�ɂ���ׂ̏C���ł���B
 *
 * Revision 1.14  2003/03/01 09:12:44  tfuruka1
 * �������[�X�p�Ƃ��ĕύX[Version]��[2.1]
 *
 * Revision 1.13  2003/02/25 15:34:40  tfuruka1
 * ���s�ԍ��o�͐��䏈����ǉ�
 * ��Version �� 2.1 �֕ύX�B�ȑO��1.7���������A���J��2.0�ōs���Ă��܂���
 *   ���߁A1.7����2.1�ɂȂ��Ă��܂����B
 *
 * Revision 1.12  2001/12/23 10:17:23  tfuruka1
 * �����������yVersion 2.0�ɂ��܂����z����������
 * �ꉞ�A���J�\��Version�ɂ����B
 * ��Copyright���󎚂��Ȃ����[�h��ǉ�
 * ����ƃf�B���N�g���擾�p�̊֐���ǉ��i��ƃt�@�C���쐬�p�̊֐�����؂�
 *   �o���j
 * �����̃t�@�C���ɂ́A����Ȃ����E�E�E�ȉ��̏C�����s����
 *     ���h���b�O���h���b�v�@�\�̒ǉ�
 *     ���v���r���[�ƈ�����ʂ��قȂ���̏C��
 *     ���c�[���o�[��ǉ�
 *     ���ׂ����o�O�̏C��
 *
 * Revision 1.11  2001/12/18 13:00:57  tfuruka1
 * �f�o�b�O����̃I�v�V������ǉ��B
 *
 * Revision 1.10  2001/12/18 04:03:17  tfuruka1
 * �����������yVersion 1.7�ɂ��܂����z����������
 * ��肠�����A�v���r���[�ł���悤�ɂ��܂����B���X�A�v���r���[���T�|�[�g
 * ������肪�Ȃ������̂ŁA���Ȃ�\�[�X�������Ȃ��Ă��܂��܂����B
 *
 * Revision 1.9  2001/12/08 15:23:48  tfuruka1
 * �����������yVersion 1.6�ɂ��܂����z����������
 * ���C����ʂ�LISTBOX��ListView�֕ύX���܂����B
 *
 * Revision 1.8  2001/12/07 18:27:28  tfuruka1
 * �p���T�C�Y�̎w��ł���悤�ɂ����B
 *
 * Revision 1.7  2001/10/01 13:24:06  tfuruka1
 * �yVersion 1.5�ɂ��܂����z
 * �p���̌������w��o����悤�ɂ��܂����B
 *
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

#include "ver.h"
#define VERSION   "Version " MAKE_VERSION "($Name:  $)"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <time.h>
#include <fcntl.h>
#include <mbstring.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <direct.h>

#include "resource.h"

#include "ak2pr.h"

#define IsShellTray() FindWindow("Shell_TrayWnd", NULL)

#define SV_DBGCLASS "ak2ps�̂悤�Ȃ��� - ak2pr(Server-Detail)"
#define SV_CLASS    "ak2ps�̂悤�Ȃ��� - ak2pr(Server)"
#define SV_CAPTION  "ak2pr Server"
#define PV_CAPTION  "�Ղ�тイ����������" TIMESTAMP

#define SV_EXE_NAME "ak2prSV.EXE"

#define COPYRIGHT   "ak2pr " VERSION \
" Copyright 1997-2004 By T.Furukawa\n(tfuruka1@nifty.com)" TIMESTAMP

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
    BOOL valid;                                 // T:���̍\���̂��L��
    CHAR szFileName[MAX_PATH];                  // �t�@�C����
    CHAR szTitle[256];                          // �^�C�g��������
    int nNumOfUp;                               // �i�g��
    int nTab;                                   // TAB��
    int nType;                                  // ����^�C�v
    int nOrientation;                           // �p���̌���(0:�f�t�H���g)
    short dmPaperSize;                          // �p���T�C�Y(0:�f�t�H���g)
    CHAR szTimeStamp[64];
    CHAR szTimeStamp1[64];
    int nBaseLine;                              // 0: Non, 1:E, 2:J, 9:A
    BOOL bNum;                                  // T: �s�ԍ����
    BOOL bKeisen;                               // T: �r���A��
    BOOL bNoRcvHeader;                          // Received�w�b�_���󎚂��Ȃ�
    BOOL bColor;                                // T: Color���
    BOOL bPreView;                              // T: �v���r���[
    BOOL bDebug;                                // T: �f�o�b�O���
    BOOL bNoCopyright;                          // T: Copyright��������Ȃ�
    BOOL bShortBinding;                         // T: �Z�ҒԂ�
    int nSingleLine;                            // T: �܂�Ԃ��Ȃ�
    double fFontSize;                           // �t�H���g�T�C�Y --- Point
    LOGFONT lfTHF;                              // �����t�H���g���
    LOGFONT lfPPF;                         // �v���|�[�V���i���t�H���g
    LOGFONT lfOF;                              // ���̑��̃t�H���g���
    LOGFONT lfOPPF;                             // ���̑��̃v���|�[�V���i��
    // ----- �ȉ���PostScript������̏��
    TCHAR szAcrobat[MAX_PATH];                  // Distiller in �t�H���_
    TCHAR szGsPath[MAX_PATH];                   // GhostScript �p�X
    TCHAR szGsOpt[512];                         // GhostScript option

} PRT_INFO, *PPRT_INFO;

// nType �ɐݒ肷��l�͈ȉ��̒ʂ�(�N���C�A���g���ł��Q�Ƃ������̂ŁA
// ak2pr.h�Ɉړ�), �f�t�H���g��PT_TEXT

// �v���r���[�p�̃f�o�C�X�R���e�L�X�g���
typedef struct _PREVIEW_INFO {
    HDC hDC;                                    // �f�o�C�X�R���e�L�X�g
    HBITMAP hBitmap;                            // �r�b�g�}�b�v
    int wd, ht;                                 // �T�C�Y
    int xoff, yoff;                             // ����s�G���A
    int dpiW, dpiH;                             // �𑜓x
    int status;                                 // ���
    RECT rc;                                    // �E�C���h�E�̈ʒu
} PREVIEW_INFO, *PPREVIEW_INFO;
enum {PVI_PRINT = 0,                            // ���
      PVI_SETUP,                                // ����ݒ��ăv���r���[
      PVI_CANCEL                                // �j��
};

// �X���b�h�ԃ��[���{�b�N�X
typedef struct {
    HWND hWnd;                                  // �n���h��
    HDC hDC;                                    // �f�o�C�X�n���h��(���p)
    HDC hDCPrinter;                             // �v�����^��DC
    HGLOBAL hDevMode;                           // �v�����^�f�o�C�X���[�h
    HGLOBAL hDevNames;                          // �v�����^�f�o�C�X�l�[��
    HANDLE hThread;                             // �X���b�h�n���h��
    DWORD idThread;                             // �X���b�hID
    BOOL bRun;                                  // T:�X���b�h���s���
    BOOL bStop;                                 // T:�����~���
    PRT_INFO PrtInfo;                           // ������
    PREVIEW_INFO PrevInfo;                      // �v���r���[���
    // ----- �ȉ��̓��[������̏��
    TCHAR szFrom[128];                          // ���o�l
    TCHAR szDate[128];                          // ��M����
    TCHAR szReference[128];                     // ���t�@�����X
    TCHAR szMessageID[128];                     // ���b�Z�[�WID
} MAILBOX, *PMAILBOX;

// �t�H���g�񋓏��
enum {FFI_FN_SETCOMBO = 0,                      // �R���{�{�b�N�X�ɃZ�b�g
      FFI_FN_GETLOGFONT,                        // LOGFONT�̎擾
      NUMBER_OF_FFI_FN
};
typedef struct {
    int funcNumber;                             // �@�\�ԍ�
    HWND hWnd;                                  // �E�C���h�E�n���h��
                                                // (FFI_FN_SETCOMBO��
                                                // ���Ɏg�p)
    LOGFONT lf;                                 // FFI_FN_GETLOGFONT��
                                                // ���Ɏg�p
} FONT_FUNK_INFO, *PFONT_FUNK_INFO;

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
VOID SetupPrtStyle(HWND hWnd, PPRT_INFO lppi);

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
    int nHeight,                                // �t�H���g�̍���
    int nWeight,                                // �t�H���g�̑���
    BOOL bItalic,                               // T: �C�^���b�N
    BOOL bUnderline,                            // T: ����
    BOOL bStrikeOut,                            // T: �ł�������
    LPLOGFONT lplf                              // LOGFONT�e���v���[�g
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
    UINT lfHeight,                              // �t�H���g�̍���
    UINT lfWeight,                              // �t�H���g�̃E�G�C�g
    BOOL lfItalic,                              // T:�C�^���b�N
    BOOL lfUnderline,                           // T:����
    BOOL lfStrikeout,                           // T:�ł�������
    LPLOGFONT lplf                              // LOGFONT�e���v���[�g
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
BOOL PrintPreview(HWND hWnd, PPREVIEW_INFO);
void SetPreViewPos(LPRECT lprc);
void GetPreViewPos(LPRECT lprc);

VOID WINAPI LsFontToCOMBO(HWND hWnd);
BOOL WINAPI GetLogFont(LPLOGFONT lplf);
/*--------------------------------------------------------------------
 * hDCPrinter�Ŏw�肳��Ă��f�o�C�X�R���e�L�X�g����A�v���r���[�p�̃f
 * �o�C�X�R���e�L�X�g���쐬����B�v���r���[�p�̃f�o�C�X�R���e�L�X�g��
 * �g�p���I�������ɔj������K�v������܂��B�܂��A�f�o�C�X�R���e�L�X
 * �g�����L���Ă���r�b�g�}�b�v���j������K�v������܂��B
 * *-------------------------------------------------------------------*/
HDC WINAPI
MakePreviewInfo(HWND hWnd,                      // �E�C���h�E�n���h��
              HDC hDCPrinter,                   // �v�����^DC
              PPREVIEW_INFO                     // �v���r���[���
    );
/* -------------------------------------------------------------------
 * ��`��`�悷��
 * *-----------------------------------------------------------------*/
void WINAPI
DrawRect(
    HDC hDC,                                    // �f�o�C�X�R���e�L�X�g
    LPRECT lprc,                                // ��`���W
    COLORREF rgb,                               // �F
    int pnStyle                                 // ���̃X�^�C��
    );
/*-------------------------------------------------------------------- 
 * ��Ɨp�f�B���N�g�����𓾂�B��ƃf�B���N�g�����͏��������Ă͂�����
 * ����B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetTempDirectoryName();
#endif
