#include "scan_fallingteleport.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "FallingTeleport"
#define SCAN_REQUIRED FALSE

VOID scanFallingTeleport_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan code section for 'and reg, ~2'
	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 6; dwCodeIndex++)
	{
		if(pbCode[dwCodeIndex] == 0x83 && pbCode[dwCodeIndex+1] >= 0xE0 && pbCode[dwCodeIndex+1] <= 0xE7 && pbCode[dwCodeIndex+2] == 0xFD)
		{
			// is next instruction 'mov [reg], reg' ?
			BYTE bRegCode = pbCode[dwCodeIndex+1] & 0x0F;
			if(pbCode[dwCodeIndex+3] == 0x89 && (pbCode[dwCodeIndex+4] & 0x40) == 0 && (pbCode[dwCodeIndex+4] & 0x38) >> 3 == bRegCode)
			{
				// make sure we have a ret instruction just following 'mov [reg], reg'
				for(DWORD dwCodeIndex2 = dwCodeIndex + 3; dwCodeIndex2 - (dwCodeIndex + 3) < 10; dwCodeIndex2++)
				{
					if(pbCode[dwCodeIndex2] == 0xC3)
					{
						// search for 'mov reg, [reg] just before 'and reg, ~2'
						for(DWORD dwCodeIndex3 = dwCodeIndex - 1; dwCodeIndex - dwCodeIndex3 < 15; dwCodeIndex3--)
						{
							if(pbCode[dwCodeIndex3] == 0x8B && (pbCode[dwCodeIndex3+1] & 0x40) == 0 && (pbCode[dwCodeIndex3+1] & 0x38) >> 3 == bRegCode)
							{
								// search for a call before 'mov reg, [reg]'
								for(DWORD dwCodeIndex4 = dwCodeIndex3 - 1; dwCodeIndex3 - dwCodeIndex4 < 10; dwCodeIndex4--)
								{
									if(pbCode[dwCodeIndex4] == 0xE8)
									{
										// convert address of that call instruction to a va
										DWORD dwFallingTeleportRVA = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex4 - PtrToUlong(pbFile));
										if(dwFallingTeleportRVA)
										{
											GetPPD(iItem, hwndDlg)->dwValue = dwFallingTeleportRVA + pNtHdr->OptionalHeader.ImageBase;
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
	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanFallingTeleport_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanFallingTeleport_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}