#include "scan_followdistance.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "FollowDistance"
#define SCAN_REQUIRED FALSE

VOID scanFollowDistance_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// search for fld dword ptr
	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 6; dwCodeIndex++)
	{
		if(*((PWORD)&pbCode[dwCodeIndex]) == 0x05D9)
		{
			// scan for fmul same dword ptr
			for(DWORD dwCodeIndex2 = dwCodeIndex + 6; dwCodeIndex2 - dwCodeIndex < 50; dwCodeIndex2++)
			{
				if(*((PWORD)&pbCode[dwCodeIndex2]) == 0x0DD8 && *((PDWORD)&pbCode[dwCodeIndex+2]) == *((PDWORD)&pbCode[dwCodeIndex2+2]))
				{
					// scan for fstp dword ptr
					for(DWORD dwCodeIndex3 = dwCodeIndex2 + 6; dwCodeIndex3 - dwCodeIndex2 < 50; dwCodeIndex3++)
					{
						if(*((PWORD)&pbCode[dwCodeIndex3]) == 0x1DD9)
						{
							// scan for mov dword ptr[x], 45C80000
							for(DWORD dwCodeIndex4 = dwCodeIndex3 + 6; dwCodeIndex4 - dwCodeIndex3 < 50; dwCodeIndex4++)
							{
								if(*((PWORD)&pbCode[dwCodeIndex4]) == 0x05C7 && *((PDWORD)&pbCode[dwCodeIndex4+6]) == 0x45C80000)
								{
									// convert dword ptr to ptr
									PDWORD pFltOffset = (PDWORD)RvaToPointer(pbFile, *((PDWORD)&pbCode[dwCodeIndex+2]) - pNtHdr->OptionalHeader.ImageBase);
									if(pFltOffset && *pFltOffset == 0x41F00000)
									{
										GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex3+2]);
										SetItemStatus(iItem, hwndDlg, "Success");
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanFollowDistance_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanFollowDistance_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}