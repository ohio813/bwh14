#include "scan_getguiddata.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "GetGuidData"
#define SCAN_REQUIRED FALSE

VOID scanGetGuidData_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan data section for "Player_C.h"
	for(DWORD dwDataIndex = 0; dwDataIndex < dwDataSize - (sizeof("Player_C.h") - 1); dwDataIndex++)
	{
		if(MemoryCompare(&pbData[dwDataIndex], (PBYTE)"Player_C.h", sizeof("Player_C.h") - 1))
		{
			// found "Player_C.h", now scan for beginning of the string
			DWORD dwStringIndex;
			for(dwStringIndex = dwDataIndex - 1; dwStringIndex && pbData[dwStringIndex] >= 0x20 && pbData[dwStringIndex] <= 0x7E; dwStringIndex--);
			dwStringIndex++;

			// calculate va of found string
			DWORD dwStringVA = OffsetToRva(pbFile, PtrToUlong(pbData) + dwStringIndex - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;

			if(dwStringVA)
			{
				// scan code section for 'mov edx, dwStringVA'
				BYTE bMoveIns[5];
				bMoveIns[0] = 0xBA;
				*((PDWORD)&bMoveIns[1]) = dwStringVA;
				for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - sizeof(bMoveIns); dwCodeIndex++)
				{
					if(MemoryCompare(&pbCode[dwCodeIndex], bMoveIns, sizeof(bMoveIns)))
					{
						// found mov instruction, make sure next instruction is 'mov ecx, constant'
						if(pbCode[dwCodeIndex + 5] == 0xB9)
						{
							// make sure instruction following 'mov ecx, constant' is a call relative
							if(pbCode[dwCodeIndex + 10] == 0xE8)
							{
								// somewhere before the 'mov edx, dwStringVA' instruction there must be this code:
								// call relative
								// push edx
								// push eax
								for(DWORD dwCodeIndex2 = dwCodeIndex - 2; dwCodeIndex - dwCodeIndex2 < 20; dwCodeIndex2--)
								{
									if(*((PWORD)&pbCode[dwCodeIndex2]) == 0x5052 && pbCode[dwCodeIndex2 - 5] == 0xE8)
									{
										// lastly check for a 'push constant' instruction before those two register pushes instruction
										for(DWORD dwCodeIndex3 = dwCodeIndex2 - 5; dwCodeIndex2 - dwCodeIndex3 < 20; dwCodeIndex3--)
										{
											if(pbCode[dwCodeIndex3] == 0x68)
											{
												// we need to decode the call relative following the 'mov ecx, constant' instruction to get the va of GetGuidData
												
												// calculate va of next instruction following call
												DWORD dwFollowingVA = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex + 15 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
												if(dwFollowingVA)
												{
													GetPPD(iItem, hwndDlg)->dwValue = dwFollowingVA + *((PDWORD)&pbCode[dwCodeIndex + 11]);
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
	}
	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanGetGuidData_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanGetGuidData_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}