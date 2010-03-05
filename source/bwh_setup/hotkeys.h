#include <windows.h>

#define CTRL 0x80000000
#define ALT  0x40000000
#define SET_DEFAULT -1
#define SET_UNBIND 0
typedef struct _HOTKEY_DATA
{
	PSZ pszName;
	DWORD dwDefault;
	DWORD dwValue;
} HOTKEY_DATA, *PHOTKEY_DATA;

VOID AddHotkey(HWND hwndList, PSZ pszName, DWORD dwDefault);
VOID SetHotkey(HWND hwndList, DWORD dwKey);
DWORD GetHotkey(HWND hwndDlg);