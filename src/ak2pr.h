/* -*- mode: c++; coding: sjis-dos;-*-
 * Time-stamp: <2004-06-18 09:27:42 tfuruka1>
 *
 * �uak2ps�̂悤�Ȃ��́v�̋��ʃw�b�_�t�@�C��(�N���C�A���g�p)
 *
 * $Id: ak2pr.h,v 1.11 2004/06/18 00:57:43 tfuruka1 Exp $
 * $Log: ak2pr.h,v $
 * Revision 1.11  2004/06/18 00:57:43  tfuruka1
 * ���s�R�[�h�̏C���݂̂ł��B
 *
 * Revision 1.10  2004/01/12 09:59:19  tfuruka1
 * ���ӒԂ��ƒZ�ӒԂ��ɑΉ��������ɂ��A�֘A�֐��Ɉ���������������߁A
 * �֘A����v���^�C�v��ύX���܂����B
 *
 * Revision 1.9  2003/03/29 12:54:15  tfuruka1
 * �� SendPrintFromStdin�֐��̎d�l�ύX�ɂ��v���g�^�C�v�̕ύX�B
 * �� ReadClipBoardToFP�֐��̐V�K�ǉ��ɂ��A�v���g�^�C�v�̒ǉ��B
 *
 * Revision 1.8  2003/03/14 16:01:06  tfuruka1
 * �� �֐��̎d�l�ύX�y�сA�V�K�ǉ��ɂ��v���g�^�C�v�̕ύX(Syslog,
 *    ExecutePrtServer, GetPSTitle)���s�����B
 *
 * Revision 1.7  2003/02/25 15:32:01  tfuruka1
 * �s�ԍ��o�͐���ɂ��C��
 *
 * Revision 1.6  2001/12/08 15:26:32  tfuruka1
 * �u�p���T�C�Y�w��v�Ɓu���C����ʂ�LISTBOX��ListView�֕ύX�v�ׁ̈A�V��
 * �ɗp���T�C�Y�̐������擾����֐����K�v�ɂȂ����B
 *
 * Revision 1.5  2001/12/07 18:23:31  tfuruka1
 * �p���T�C�Y�̎w����o����悤�ɂ����B
 *
 * Revision 1.4  2001/10/01 13:22:28  tfuruka1
 * �p���̌������w��o����悤�ɏC���i�֐��̈����̒ǉ��j�B
 *
 * Revision 1.3  2001/08/19 04:41:03  tfuruka1
 * PostScript�t�@�C���̎b��Ή��i�����P��Distiller�̊Ď��t�H���_�ɕ��荞
 * �ނ����j�B
 *
 * Revision 1.2  2001/08/18 16:48:20  tfuruka1
 * ��SendPrintFromFile�֐��̔p�~�ɔ����v���g�^�C�v�錾���폜�B
 *
 * Revision 1.1  2001/02/05 17:44:24  tfuruka1
 * Initial revision
 *
 */
#ifndef _AK2PR_H_
#define _AK2PR_H_

// �G���Ȓ�`
#define MB_ERROR (MB_ICONSTOP | MB_SETFOREGROUND)

// ����f�[�^�̎��
enum {PT_TEXT,                                  // ���ʂ̃e�L�X�g
      PT_MAIL,                                  // ���[��(�f�R�[�h�ς�)
      PT_PS_ACROBAT, // PostScript(Acrobat�ŏ�������)
      PT_PS_GHOST, // PostScript(GhostScript�ŏ�������)
      NUM_OF_PT};

//
// �ȉ��̓v���g�^�C�v
//
/*--------------------------------------------------------------------
 * dwErr �Ŏw�肳�ꂽ�G���[�R�[�h�ɑΉ�����G���[���b�Z�[�W���֐��ɃV�X
 * �e�����b�Z�[�W�e�[�u�����\�[�X���猟�����Ĉꎞ�I�ȕ�����ւ̃|�C��
 * �^��ԋp����BdwErr �� GetLastError ���瓾���l���w�肷�鎖�Blpsz �̓G���[
 * ���b�Z�[�W�֒ǉ����镶������w�肷��BAPI �������w�肷��B
 * *-------------------------------------------------------------------*/
LPCSTR WINAPI
GetLastErrorMessage(LPCSTR lpsz, DWORD dwErr);
/*--------------------------------------------------------------------
 * �v�����g�T�[�o���N�����Ă��邩�`�F�b�N����B�N�����Ă����ꍇ��TRUE
 * ��ԋp����B���N���̏ꍇ�� FALSE��Ԃ��B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
IsPrtServerEnable(VOID);
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
    );
/*--------------------------------------------------------------------
 * �������g���i�[����Ă���f�B���N�g�����ւ̈ꎞ�I�ȃ|�C���^�𓾂�B
 * �ԋp����|�C���^�͈ꎞ�I�Ȃ��̂Ȃ̂�, �K�v�ȏꍇ�͕��ʂ��Ďg�p����
 * ���B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetMyDir(VOID);
/*--------------------------------------------------------------------
 * UNIX��basename�Ɠ����B�t���p�X����t�@�C�����̕����ւ̈ꎞ�I�ȃ|�C
 * ���^�𓾂�B�ԋp����|�C���^�͈ꎞ�I�Ȃ��̂Ȃ̂�, �K�v�ȏꍇ�͕���
 * ���Ďg�p���鎖�B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
BaseName(LPCTSTR lpszFullPath);
LPCTSTR WINAPI
GetLongBaseName(LPCTSTR lpszFullPath);
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
    int bNum,                                   // �s�ԍ�
    int nBinding                                // �Ԃ�����
    );
/*-------------------------------------------------------------------- 
 * �W�����͂̓��e��ǂݍ��݃v�����g�T�[�o�ֈ�����𑗐M����B�W����
 * �͂̑���ɃN���b�v�{�[�h�̓��e��ǂݍ��ގ����ł���B
 *  *-------------------------------------------------------------------*/
BOOL WINAPI
SendPrintFromStdin(
    BOOL bClipBoard,                            // T:�N���b�v�{�[�h���g�p
    HWND hWnd,                                  // �n���h��
    LPCTSTR lpszTitle,                          // �^�C�g�����w�莞��"stdin"
    int nNumOfUp,                               // �i�g��
    int nTab,                                   // �^�u��
    double fFontSize,                           // �t�H���g�T�C�Y
    int nType,                                  // ����f�[�^�^�C�v
    int nOrientation,                           // �p���̌���
    short dmPaperSize,                          // �p���T�C�Y
    int bNum,                                   // �s�ԍ�
    int nBinding                                // �Ԃ�����
    );
/*--------------------------------------------------------------------
 * lpstr�Ŏw�肳�ꂽ����������������ɏ]���ĕҏW���\���E�B���h�E�֕\��
 * ����B������printf�Ɠ��l�ł���B�\���E�B���h�E�����݂��Ȃ��ꍇ�́C
 * hWnd�֕\���E�B���h�E��Create������ɕ\������BnFlag�̓��b�Z�[�W����
 * �ʂ���ׂ̃C���W�P�[�^�Ƃ��ĕ\�������BnFlag�� 'N' ������0x20�`
 * 0x7e���̉��ꂩ�̃R�[�h���w��o����B�߂�l�͏�ɕ\���E�C���h�E�̃n
 * ���h���ł���B�A���C����������ɏ]���č쐬���ꂽ�������16384������
 * ���łȂ���΂Ȃ�Ȃ��BnFlag�� 'I', 'C', 'F', 'R', 'W', 'E', 'S', '
 * s', 'r' ��F�����܂��B����ȊO��'I'�Ƃ��ď������܂��B
 *
 * ����: �\���E�C���h�E�̃R���g���[��ID��200���g�p���܂��B�]���ăI�[�i
 * �E�C���h�E�ł�ID=200�̃R���g���[���͎g�p�ł��܂���(�ʏ��ID��100��
 * ��g�p����܂��̂ŁC�Ԃ���\���͏��Ȃ����ł�)�BID=200��
 * WM_CTLCOLORLISTBOX, WM_DRAWITEM, WM_MEASUREITEM�y��, WM_SIZE���t�b
 * �N���܂��̂ŁC�����̃C�x���g�̓I�[�i�[�E�C���h�E�֒ʒm����܂���B
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgPrint(
    HWND hWnd,                                  // �E�C���h�E�n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpstr,                               // ����printf�Ɠ���
    ...                                         // ����
    );
/*--------------------------------------------------------------------
 * lpStart����uLen�o�C�g��16�i�_���v��\������B�\���E�B���h�E�����݂�
 * �Ȃ��ꍇ�́ChWnd�֕\���E�B���h�E��Create������ɕ\������BlpszHead
 * �̓_���v�̑O�ɕ\�����镶������w�肷��BnFlag�̈Ӗ���DbgPrint��
 * �Q�Ƃ̎��B
 * *-------------------------------------------------------------------*/
HWND WINAPI
DbgDump(
    HWND hWnd,                                  // �n���h��
    int nFlag,                                  // �t���O
    LPCSTR lpszHead,                            // �^�C�g��
    LPBYTE lpStart,                             // �J�n�A�h���X
    ULONG uLen                                  // �_���v��
    );
/*--------------------------------------------------------------------
 * UNIX��Syslog�̊ȈՔŁB���debug.local7�����o�͂��܂���B
 * *-------------------------------------------------------------------*/
VOID WINAPI
Syslog(
       BOOL bStdOut,                            // T:�W���o�͂ւ��o��
       LPCSTR lpstr,                            // ����printf�Ɠ���
       ...                                      // ����
    );
/*--------------------------------------------------------------------
 * �v�����g�T�[�o���N������B����ɋN���o�����ꍇ�� TRUE ��ԋp����B�N
 * ���ł��Ȃ������ꍇ�� FALSE ��Ԃ��B
 * *-------------------------------------------------------------------*/
BOOL WINAPI
ExecutePrtServer(LPCTSTR lpszOption);
/*-------------------------------------------------------------------- 
 * �w��\�ȗp���T�C�Y�̈ꗗ��ԋp����B
 * 
 * *-------------------------------------------------------------------*/
LPTSTR WINAPI
GetPaperSizeUsageMessage();
/*-------------------------------------------------------------------- 
 * �R�}���h�I�v�V�����Ŏw�肵���p���T�C�Y����ADEVMODE�Ŏg�p����p���T
 * �C�Y�̒萔�𓾂�B�p���T�C�Y���s���̏ꍇ��0��ԋp����B
 * *-------------------------------------------------------------------*/
short WINAPI
GetPaperSizeDevMode(LPTSTR cmdOpt);
/*--------------------------------------------------------------------
 * DEVMODE�Ŏg�p����p���T�C�Y�̒萔����p���T�C�Y�̐����𓾂�B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
GetPaperSizeComment(short dmPaperSize);

/*--------------------------------------------------------------------
 * PostScript �t�@�C������ATitle�𓾂�B
 * *-------------------------------------------------------------------*/
LPCTSTR WINAPI
    GetPSTitle(LPCTSTR lpszFile,                // PS�t�@�C����
               LPTSTR lpszTitle,                // �^�C�g���i�[�G���A
               int cbMax                        // �i�[�ő啶����
               );
/* -------------------------------------------------------------------
 * �N���b�v�{�[�h�̓��e���t�@�C���ɏo�͂���
 * *-----------------------------------------------------------------*/
BOOL WINAPI
    ReadClipBoardToFP(HWND hWnd,                /* �N���b�v�{�[�h���I�[
                                                 * �v������E�C���h�E�F
                                                 * NULL�͌��݂̃^�X�N
                                                 *  */
                      FILE *fp                  /* �t�@�C���|�C���^ */
                      );
#endif //_AK2PR_H_
