#include "fonts.h"
#include <commctrl.h>
#include "resource.h"

HFONT g_hFixed = NULL, g_hProportional = NULL;

HFONT GetFont(PSZ pszFontName, LONG lPtSize, BOOL bBold)
{
	LOGFONT lf;
	HWND hwndDesktop = GetDesktopWindow();
	HDC hdcDesktop = GetDC(hwndDesktop);
	lf.lfHeight = ~MulDiv(lPtSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);
	ReleaseDC(hwndDesktop, hdcDesktop);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	if(bBold) lf.lfWeight = FW_BOLD;
	else lf.lfWeight = FW_REGULAR;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lstrcpy(lf.lfFaceName, pszFontName);
	return CreateFontIndirect(&lf);
}

VOID SetFixedWidthFont(HWND hwndDlg, PSZ pszName, LONG lSize, BOOL bBold)
{
	if(g_hFixed)
		DeleteObject(g_hFixed);
	g_hFixed = GetFont(pszName, lSize, bBold);
	SendDlgItemMessage(hwndDlg, IDC_FIXED, WM_SETFONT, (WPARAM)g_hFixed, TRUE);
	SendDlgItemMessage(hwndDlg, IDC_FIXED, WM_SETTEXT, 0, (LPARAM)"Fixed Width Font");
}

VOID SetProportionalFont(HWND hwndDlg, PSZ pszName, LONG lSize, BOOL bBold)
{
	if(g_hProportional)
		DeleteObject(g_hProportional);
	g_hProportional = GetFont(pszName, lSize, bBold);
	SendDlgItemMessage(hwndDlg, IDC_PROPORTIONAL, WM_SETTEXT, 0 ,(LPARAM)"Proportional Font");
	SendDlgItemMessage(hwndDlg, IDC_PROPORTIONAL, WM_SETFONT, (WPARAM)g_hProportional, TRUE);
}

BOOL SelectFont(HWND hwndDlg, PSZ pszName, PLONG plSize, PBOOL pbBold)
{
	LOGFONT lf;

	lstrcpy(lf.lfFaceName, pszName);
	lf.lfWeight = *pbBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = FALSE;

	HWND hwndDesktop = GetDesktopWindow();
	HDC hdcDesktop = GetDC(hwndDesktop);
	lf.lfHeight = MulDiv(*plSize, GetDeviceCaps(hdcDesktop, LOGPIXELSY), 72);
	lf.lfWidth = 0;
	ReleaseDC(hwndDesktop, hdcDesktop);

	CHOOSEFONT cs = {sizeof(CHOOSEFONT), hwndDlg, NULL, &lf, *plSize * 10, CF_SCREENFONTS | CF_NOSCRIPTSEL | CF_INITTOLOGFONTSTRUCT, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	if(ChooseFont(&cs))
	{
		 lstrcpy(pszName, lf.lfFaceName);
		 *plSize = cs.iPointSize / 10;
		 if(cs.nFontType & BOLD_FONTTYPE)
			*pbBold = TRUE;
		 else
			*pbBold = FALSE;
		 return TRUE;
	}
	return FALSE;
}

VOID FreeFonts()
{
	if(g_hFixed)
		DeleteObject(g_hFixed);
	if(g_hProportional)
		DeleteObject(g_hProportional);
}