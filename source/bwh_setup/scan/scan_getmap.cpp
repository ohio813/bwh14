#include "scan_getmap.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "GetMap"
#define SCAN_REQUIRED FALSE

VOID scanGetMap_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan data section for "Map:\t\t%u (%s)"
	for(DWORD dwDataIndex = 0; dwDataIndex < dwDataSize - (sizeof("Map:\t\t%u (%s)") - 1); dwDataIndex++)
	{
		if(MemoryCompare(&pbData[dwDataIndex], (PBYTE)"Map:\t\t%u (%s)", sizeof("Map:\t\t%u (%s)") - 1))
		{
			// calculate va of found string
			DWORD dwStringVA = OffsetToRva(pbFile, PtrToUlong(pbData) + dwDataIndex - PtrToUlong(pbFile));
			if(dwStringVA)
			{
				dwStringVA += pNtHdr->OptionalHeader.ImageBase;

				// scan code section for 'push dwStringVA'
				for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 8; dwCodeIndex++)
				{
					if(pbCode[dwCodeIndex] == 0x68 && *((PDWORD)&pbCode[dwCodeIndex + 1]) == dwStringVA)
					{
						// find first call above
						for(DWORD dwCodeIndex2 = dwCodeIndex - 5; dwCodeIndex - dwCodeIndex2 < 50; dwCodeIndex2--)
						{
							if(pbCode[dwCodeIndex2] == 0xE8)
							{
								DWORD dwFollowingVA = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex2 + 5 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
								if(dwFollowingVA)
								{
									GetPPD(iItem, hwndDlg)->dwValue = dwFollowingVA + *((PDWORD)&pbCode[dwCodeIndex2+1]);
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
	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanGetMap_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanGetMap_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}