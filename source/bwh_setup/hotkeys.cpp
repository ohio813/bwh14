#include "hotkeys.h"
#include <commctrl.h>
#include "resource.h"

BOOL g_bCtrlDown = FALSE;
BOOL g_bAltDown = FALSE;

PSZ HotkeyToString(DWORD dwHotkey, PSZ pszBuffer)
{
	WORD wCharString;
	pszBuffer[0] = '\0';
	char szString[6];

	if(dwHotkey)
	{
		if(dwHotkey & CTRL)
			lstrcat(pszBuffer, "Ctrl - ");
		if(dwHotkey & ALT)
			lstrcat(pszBuffer, "Alt - ");
		
		dwHotkey &= ~(CTRL | ALT);

		if(dwHotkey == VK_BACK)
			lstrcat(pszBuffer, "Backspace");
		else if(dwHotkey == VK_TAB)
			lstrcat(pszBuffer, "Tab");
		else if(dwHotkey == VK_CLEAR)
			lstrcat(pszBuffer, "Clear");
		else if(dwHotkey == VK_RETURN)
			lstrcat(pszBuffer, "Return");
		else if(dwHotkey == VK_SHIFT)
			lstrcat(pszBuffer, "Shift");
		else if(dwHotkey == VK_PAUSE)
			lstrcat(pszBuffer, "Pause");
		else if(dwHotkey == VK_CAPITAL)
			lstrcat(pszBuffer, "Caps Lock");
		else if(dwHotkey == VK_ESCAPE)
			lstrcat(pszBuffer, "Escape");
		else if(dwHotkey == VK_SPACE)
			lstrcat(pszBuffer, "Space");
		else if(dwHotkey == VK_PRIOR)
			lstrcat(pszBuffer, "Page Up");
		else if(dwHotkey == VK_NEXT)
			lstrcat(pszBuffer, "Page Down");
		else if(dwHotkey == VK_END)
			lstrcat(pszBuffer, "End");
		else if(dwHotkey == VK_HOME)
			lstrcat(pszBuffer, "Home");
		else if(dwHotkey == VK_LEFT)
			lstrcat(pszBuffer, "Left");
		else if(dwHotkey == VK_UP)
			lstrcat(pszBuffer, "Up");
		else if(dwHotkey == VK_RIGHT)
			lstrcat(pszBuffer, "Right");
		else if(dwHotkey == VK_DOWN)
			lstrcat(pszBuffer, "Down");
		else if(dwHotkey == VK_INSERT)
			lstrcat(pszBuffer, "Insert");
		else if(dwHotkey == VK_DELETE)
			lstrcat(pszBuffer, "Delete");
		else if(dwHotkey == VK_LWIN)
			lstrcat(pszBuffer, "Left Win");
		else if(dwHotkey == VK_RWIN)
			lstrcat(pszBuffer, "Right Win");
		else if(dwHotkey == VK_APPS)
			lstrcat(pszBuffer, "Apps");
		else if(dwHotkey == VK_SLEEP)
			lstrcat(pszBuffer, "Sleep");
		else if(dwHotkey == VK_NUMPAD0)
			lstrcat(pszBuffer, "Numpad 0");
		else if(dwHotkey == VK_NUMPAD1)
			lstrcat(pszBuffer, "Numpad 1");
		else if(dwHotkey == VK_NUMPAD2)
			lstrcat(pszBuffer, "Numpad 2");
		else if(dwHotkey == VK_NUMPAD3)
			lstrcat(pszBuffer, "Numpad 3");
		else if(dwHotkey == VK_NUMPAD4)
			lstrcat(pszBuffer, "Numpad 4");
		else if(dwHotkey == VK_NUMPAD5)
			lstrcat(pszBuffer, "Numpad 5");
		else if(dwHotkey == VK_NUMPAD6)
			lstrcat(pszBuffer, "Numpad 6");
		else if(dwHotkey == VK_NUMPAD7)
			lstrcat(pszBuffer, "Numpad 7");
		else if(dwHotkey == VK_NUMPAD8)
			lstrcat(pszBuffer, "Numpad 8");
		else if(dwHotkey == VK_NUMPAD9)
			lstrcat(pszBuffer, "Numpad 9");
		else if(dwHotkey == VK_MULTIPLY)
			lstrcat(pszBuffer, "Numpad *");
		else if(dwHotkey == VK_ADD)
			lstrcat(pszBuffer, "Numpad +");
		else if(dwHotkey == VK_SUBTRACT)
			lstrcat(pszBuffer, "Numpad -");
		else if(dwHotkey == VK_DECIMAL)
			lstrcat(pszBuffer, "Numpad .");
		else if(dwHotkey == VK_DIVIDE)
			lstrcat(pszBuffer, "Numpad /");
		else if(dwHotkey == VK_F1)
			lstrcat(pszBuffer, "F1");
		else if(dwHotkey == VK_F2)
			lstrcat(pszBuffer, "F2");
		else if(dwHotkey == VK_F3)
			lstrcat(pszBuffer, "F3");
		else if(dwHotkey == VK_F4)
			lstrcat(pszBuffer, "F4");
		else if(dwHotkey == VK_F5)
			lstrcat(pszBuffer, "F5");
		else if(dwHotkey == VK_F6)
			lstrcat(pszBuffer, "F6");
		else if(dwHotkey == VK_F7)
			lstrcat(pszBuffer, "F7");
		else if(dwHotkey == VK_F8)
			lstrcat(pszBuffer, "F8");
		else if(dwHotkey == VK_F9)
			lstrcat(pszBuffer, "F9");
		else if(dwHotkey == VK_F10)
			lstrcat(pszBuffer, "F10");
		else if(dwHotkey == VK_F11)
			lstrcat(pszBuffer, "F11");
		else if(dwHotkey == VK_F12)
			lstrcat(pszBuffer, "F12");
		else if(dwHotkey == VK_NUMLOCK)
			lstrcat(pszBuffer, "Numlock");
		else if(dwHotkey == VK_SCROLL)
			lstrcat(pszBuffer, "ScrollLock");
		else if(dwHotkey == VK_OEM_1)
			lstrcat(pszBuffer, ";");
		else if(dwHotkey == VK_OEM_PLUS)
			lstrcat(pszBuffer, "=");
		else if(dwHotkey == VK_OEM_COMMA)
			lstrcat(pszBuffer, ",");
		else if(dwHotkey == VK_OEM_MINUS)
			lstrcat(pszBuffer, "-");
		else if(dwHotkey == VK_OEM_PERIOD)
			lstrcat(pszBuffer, ".");
		else if(dwHotkey == VK_OEM_2)
			lstrcat(pszBuffer, "/");
		else if(dwHotkey == VK_OEM_3)
			lstrcat(pszBuffer, "`");
		else if(dwHotkey == VK_OEM_4)
			lstrcat(pszBuffer, "[");
		else if(dwHotkey == VK_OEM_5)
			lstrcat(pszBuffer, "\\");
		else if(dwHotkey == VK_OEM_6)
			lstrcat(pszBuffer, "]");
		else if(dwHotkey == VK_OEM_7)
			lstrcat(pszBuffer, "'");
		else if((dwHotkey >= '0' && dwHotkey <= '9') || (dwHotkey >= 'A' && dwHotkey <= 'Z'))
		{
			wCharString = (WORD)dwHotkey;
			lstrcat(pszBuffer, (PSZ)&wCharString);
		}
		else
		{
			wsprintf(szString, "(%d)", dwHotkey);
			lstrcat(pszBuffer, szString);
		}
	}
	else
		lstrcat(pszBuffer, "Unbound");

	return pszBuffer;
}

VOID AddHotkey(HWND hwndList, PSZ pszName, DWORD dwDefault)
{
	LVITEM lvi;
	char szINI[MAX_PATH];
	char szBuffer[100];
	INT x;
	PHOTKEY_DATA phd;

	GetModuleFileName(GetModuleHandle(NULL), szINI, MAX_PATH);
	for(x = lstrlen(szINI); x && szINI[x] != '\\'; x--);
	szINI[x] = '\0';
	lstrcat(szINI, "\\bwh.ini");

	phd = (PHOTKEY_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(HOTKEY_DATA));
	phd->pszName = pszName;
	phd->dwDefault = dwDefault;
	phd->dwValue = GetPrivateProfileInt("HOTKEYS", pszName, dwDefault, szINI);

	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.pszText = pszName;
	lvi.lParam = (LPARAM)phd;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	ListView_InsertItem(hwndList, &lvi);

	
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = HotkeyToString(phd->dwValue, szBuffer);
	ListView_SetItem(hwndList, &lvi);
}

VOID SetHotkey(HWND hwndList, DWORD dwKey)
{
	LVITEM lvi;
	char szBuffer[100];
	lvi.iItem = -1;

	lvi.iItem = ListView_GetNextItem(hwndList, lvi.iItem, LVIS_SELECTED);
	while(lvi.iItem != -1)
	{
		lvi.mask = LVIF_PARAM;
		lvi.iSubItem = 0;
		ListView_GetItem(hwndList, &lvi);
		
		if(dwKey == -1)
			PHOTKEY_DATA(lvi.lParam)->dwValue = PHOTKEY_DATA(lvi.lParam)->dwDefault;
		else
			PHOTKEY_DATA(lvi.lParam)->dwValue = dwKey;
		ListView_SetItem(hwndList, &lvi);
		
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = 1;
		lvi.pszText = HotkeyToString(PHOTKEY_DATA(lvi.lParam)->dwValue, szBuffer);
		ListView_SetItem(hwndList, &lvi);

		lvi.iItem = ListView_GetNextItem(hwndList, lvi.iItem, LVIS_SELECTED);
	}
}

WNDPROC g_OrigButtonProc;

LRESULT CALLBACK SubclassedButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
		break;
	case WM_KEYDOWN:
		if(wParam == VK_CONTROL)
		{
			g_bCtrlDown = TRUE;
		}
		else if(wParam == VK_MENU)
		{
			g_bAltDown = TRUE;
		}
		else
		{
			wParam = wParam | (g_bCtrlDown ? 0x80000000 : 0) | (g_bAltDown ? 0x40000000 : 0);
			EndDialog(GetParent(hwnd), wParam);
		}
		break;
	case WM_KEYUP:
		if(wParam == VK_CONTROL)
		{
			if(GetAsyncKeyState(VK_MENU) >= 0)
				g_bAltDown = FALSE;
			g_bCtrlDown = FALSE;
		}
		else if(wParam == VK_MENU)
		{
			if(GetAsyncKeyState(VK_CONTROL) >= 0)
				g_bCtrlDown = FALSE;
			g_bAltDown = FALSE;
		}
		break;
	}
	return CallWindowProc(g_OrigButtonProc, hwnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK AssignProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
	case WM_INITDIALOG:
		g_bCtrlDown = FALSE;
		g_bAltDown = FALSE;
		g_OrigButtonProc = (WNDPROC)LongToPtr(SetWindowLong(GetDlgItem(hwndDlg, IDC_DUMMY), GWL_WNDPROC, PtrToLong(SubclassedButtonProc)));
		break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}
DWORD GetHotkey(HWND hwndDlg)
{
	return (DWORD)DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ASSIGN), hwndDlg, AssignProc, NULL);
}