#include <windows.h>
#include <string.h>

LPTSTR
TrimRight(LPTSTR lpszStr)
{
    size_t len = strlen(lpszStr);

    for (--len; len; len--) {
        if ('\r' == *(lpszStr + len) ||
            '\n' == *(lpszStr + len) ||
            '\t' == *(lpszStr + len) ||
            '\f' == *(lpszStr + len) ||
            ' ' == *(lpszStr + len)) {
            *(lpszStr + len) = '\0';
        }
        else {
            break;
        }
    }
    return lpszStr;
}

LPTSTR
TrimLeft(LPTSTR lpszStr)
{
    LPTSTR pFrom = lpszStr, pTo = lpszStr;
    
    for (; *pFrom; pFrom++) {
        if ('\r' != *pFrom &&
            '\n' != *pFrom &&
            '\t' != *pFrom &&
            '\f' != *pFrom &&
            ' ' != *pFrom) {
            break;
        }
    }

    if (pFrom == pTo) {                         // —]•ª‚È•¶Žš‚Í–³‚¢
        return lpszStr;
    }
    for (; *pFrom; pFrom++, pTo++) {
        *pTo = *pFrom;
    }
    *pTo = '\0';
    return lpszStr;
}

LPTSTR
TrimString(LPSTR lpszStr)
{
    return TrimLeft(TrimRight(lpszStr));
}
