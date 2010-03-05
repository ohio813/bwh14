#include "scan_followfaction.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "FollowFaction"
#define SCAN_REQUIRED FALSE

VOID scanFollowFaction_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan data section for "Usage: FollowUnit"
	for(DWORD dwDataIndex = 0; dwDataIndex < dwDataSize - (sizeof("Usage: FollowByName") - 1); dwDataIndex++)
	{
		if(MemoryCompare(&pbData[dwDataIndex], (PBYTE)"Usage: FollowByName", sizeof("Usage: FollowByName") - 1))
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
						// search up for a call
						for(DWORD dwCodeIndex2 = dwCodeIndex - 2; dwCodeIndex - dwCodeIndex2 < 120; dwCodeIndex2--)
						{
							if(pbCode[dwCodeIndex2] == 0xE8)
							{
								// calc offset of call
								DWORD dwCallOffset = (dwCodeIndex2 + 5) + *((PDWORD)&pbCode[dwCodeIndex2+1]);
								
								// scan for a ret, push
								for(DWORD dwCodeIndex3 = dwCallOffset; dwCodeIndex3 - dwCallOffset < 250; dwCodeIndex3++)
								{
									if(*((PWORD)&pbCode[dwCodeIndex3]) == 0x68C3)
									{
										// search up for a jump that jumps to the push
										for(DWORD dwCodeIndex4 = dwCodeIndex3 - 2; dwCodeIndex4 > dwCallOffset; dwCodeIndex4--)
										{
											if(pbCode[dwCodeIndex4] >= 0x70 && pbCode[dwCodeIndex4] <= 0x7F && pbCode[dwCodeIndex4+1] == ((dwCodeIndex3 + 1) - (dwCodeIndex4 + 2)))
											{
												GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex4 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
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
		}
	}

	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanFollowFaction_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanFollowFaction_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}