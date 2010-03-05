#include "scan_nofalldamagehook.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "NoFallDamageHook"
#define SCAN_REQUIRED FALSE

VOID scanNoFallDamageHook_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan for 'and reg, 75BFEDFF'
	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 6; dwCodeIndex++)
	{
		if(*((PDWORD)&pbCode[dwCodeIndex]) == 0x75BFEDFF && (pbCode[dwCodeIndex - 1] == 0x25 || ((pbCode[dwCodeIndex - 1] & 0xF8) == 0xE0 && pbCode[dwCodeIndex - 2] == 0x81)))
		{
			// search for long jump or call following and instruction
			for(DWORD dwCodeIndex2 = dwCodeIndex + 4; dwCodeIndex2 - dwCodeIndex < 20; dwCodeIndex2++)
			{
				if(pbCode[dwCodeIndex2] == 0xE9 || pbCode[dwCodeIndex2] == 0xE8)
				{
					// get offset of jump location
					DWORD dwJumpOffset = (dwCodeIndex2 + 5) + *((PDWORD)&pbCode[dwCodeIndex2 + 1]);
					
					// scan for 'test [reg+disp], 4000000h'
					for(DWORD dwCodeIndex3 = dwJumpOffset; dwCodeIndex3 - dwJumpOffset < 500; dwCodeIndex3++)
					{
						if(*((PDWORD)&pbCode[dwCodeIndex3]) == 0x04000000)
						{
							if(pbCode[dwCodeIndex3 - 3] == 0xF7) // short displacement
							{
								// find a je to this instruction
								for(DWORD dwCodeIndex4 = dwCodeIndex3 - 5; dwCodeIndex4 >= dwJumpOffset; dwCodeIndex4--)
								{
									if(pbCode[dwCodeIndex4] == 0x74 && pbCode[dwCodeIndex4+1] == ((dwCodeIndex3 - 3) - (dwCodeIndex4 + 2)))
									{
										// search for 'mov [reg+disp], reg' above the jump
										for(DWORD dwCodeIndex5 = dwCodeIndex4; dwCodeIndex5 >= dwJumpOffset; dwCodeIndex5--)
										{
											if(pbCode[dwCodeIndex5] == 0x89)
											{
												// search for 'mov reg, [reg+disp]
												for(DWORD dwCodeIndex6 = dwCodeIndex5; dwCodeIndex6 >= dwJumpOffset; dwCodeIndex6--)
												{
													if(pbCode[dwCodeIndex6] == 0x8B && (pbCode[dwCodeIndex6+1] & 0x38) == (pbCode[dwCodeIndex5+1] & 0x38))
													{
														GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex2 + 1 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
														SetItemStatus(iItem, hwndDlg, "Success");
														return;
													}
												}
											}
										}
									}
								}
							}
							else if(pbCode[dwCodeIndex3 - 6] == 0xF7) // long displacement
							{
								// find a je to this instruction
								for(DWORD dwCodeIndex4 = dwCodeIndex3 - 8; dwCodeIndex4 >= dwJumpOffset; dwCodeIndex4--)
								{
									if(pbCode[dwCodeIndex4] == 0x74 && pbCode[dwCodeIndex4+1] == ((dwCodeIndex3 - 6) - (dwCodeIndex4 + 2)))
									{
										// search for mov '[reg+disp], reg' above the jump
										for(DWORD dwCodeIndex5 = dwCodeIndex4; dwCodeIndex5 >= dwJumpOffset; dwCodeIndex5--)
										{
											if(pbCode[dwCodeIndex5] == 0x89)
											{
												// search for 'mov reg, [reg+disp]
												for(DWORD dwCodeIndex6 = dwCodeIndex5; dwCodeIndex6 >= dwJumpOffset; dwCodeIndex6--)
												{
													if(pbCode[dwCodeIndex6] == 0x8B && (pbCode[dwCodeIndex6+1] & 0x38) == (pbCode[dwCodeIndex5+1] & 0x38))
													{
														GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex2 + 1 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
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
	}

	GetPPD(iItem, hwndDlg)->dwValue = 0;
	SetItemStatus(iItem, hwndDlg, "Fail");
}

VOID scanNoFallDamageHook_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanNoFallDamageHook_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}