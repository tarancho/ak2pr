#ifndef _MPR_H_
#define _MPR_H_

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <mbstring.h>
#include <string.h>
#include <time.h>

#define PI (3.141592654)

LPTSTR TrimRight(LPTSTR lpszStr);
LPTSTR TrimLeft(LPTSTR lpszStr);
LPTSTR TrimString(LPSTR lpszStr);

#endif // _MPR_H_
