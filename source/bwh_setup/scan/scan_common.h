#include <windows.h>

typedef VOID(*PFNSCANPROC)(INT iItem, HWND hwndDlg, PBYTE pbFile);

typedef struct _POINTER_DATA
{
	PSZ         pszCategory;
	PSZ         pszName;
	BOOL        bRequired;
	DWORD       dwValue;
	PFNSCANPROC pfnScanProc;
} POINTER_DATA, *PPOINTER_DATA;

PBYTE RvaToPointer(PBYTE pbImage, DWORD dwRva);
PPOINTER_DATA GetPPD(INT iItem, HWND hwndDlg);
VOID SetItemStatus(INT iItem, HWND hwndDlg, PSZ pszStatus);
BOOL MemoryCompare(PBYTE pb1, PBYTE pb2, DWORD dwSize);
DWORD OffsetToRva(PBYTE pbImage, DWORD dwOffset);
