#include <windows.h>

HFONT GetFont(PSZ pszFontName, LONG lPtSize, BOOL bBold);
VOID SetFixedWidthFont(HWND hwndDlg, PSZ pszName, LONG lSize, BOOL bBold);
VOID SetProportionalFont(HWND hwndDlg, PSZ pszName, LONG lSize, BOOL bBold);
BOOL SelectFont(HWND hwndDlg, PSZ pszName, PLONG plSize, PBOOL pbBold);
VOID FreeFonts();