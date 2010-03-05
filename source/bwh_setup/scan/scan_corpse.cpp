#include "scan_corpse.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "Corpse"
#define SCAN_REQUIRED FALSE

VOID scanCorpse_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
{
	PIMAGE_NT_HEADERS pNtHdr = PIMAGE_NT_HEADERS(pbFile + PIMAGE_DOS_HEADER(pbFile)->e_lfanew);
    PIMAGE_SECTION_HEADER pSecHdr = PIMAGE_SECTION_HEADER(pNtHdr + 1);
    PBYTE pbCode = RvaToPointer(pbFile, pSecHdr[0].VirtualAddress);
    DWORD dwCodeSize = pSecHdr[0].SizeOfRawData;
	PBYTE pbRData = RvaToPointer(pbFile, pSecHdr[1].VirtualAddress);
	DWORD dwRDataSize = pSecHdr[1].SizeOfRawData;
	PBYTE pbData = RvaToPointer(pbFile, pSecHdr[2].VirtualAddress);
    DWORD dwDataSize = pSecHdr[2].SizeOfRawData;
	
	SetItemStatus(iItem, hwndDlg, "Scanning...");

	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize; dwCodeIndex++)
	{
		if(*((PDWORD)&pbCode[dwCodeIndex]) == 0xC9D8C0D9 && *((PDWORD)&pbCode[dwCodeIndex+4]) == 0xCCD8C3D9 && *((PDWORD)&pbCode[dwCodeIndex+8]) == 0xC2D9C1DE && *((PDWORD)&pbCode[dwCodeIndex+12]) == 0xC1DECBD8 && *((PWORD)&pbCode[dwCodeIndex+16]) == 0x1DD8)
		{
			if(*((PWORD)&pbCode[dwCodeIndex - 6]) == 0x25D8)
			{
				for(DWORD dwCodeIndex2 = dwCodeIndex - 6; dwCodeIndex - dwCodeIndex2 < 36; dwCodeIndex2--)
				{
					if(pbCode[dwCodeIndex2] == 0xFF)
					{
						GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex-4]) - 8;
						SetItemStatus(iItem, hwndDlg, "Success");
						return;
					}
				}
			}
		}
	}
	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanCorpse_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanCorpse_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}