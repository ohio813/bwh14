#include "scan_lockspeed.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "LockSpeed"
#define SCAN_REQUIRED FALSE

VOID scanLockSpeed_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// search for 'and reg, FBE07FFF'
	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 4; dwCodeIndex++)
	{
		if(*((PDWORD)&pbCode[dwCodeIndex]) == 0xFBE07FFF && pbCode[dwCodeIndex - 2] == 0x81)
		{
			// search for first call
			for(DWORD dwCodeIndex2 = dwCodeIndex + 6; dwCodeIndex2 - dwCodeIndex < 30; dwCodeIndex2++)
			{
				if(pbCode[dwCodeIndex2] == 0xE8)
				{
					// compute offset of call
					DWORD dwCall1Offset = (dwCodeIndex2 + 5) + *((PDWORD)&pbCode[dwCodeIndex2 + 1]);
					if(dwCall1Offset < dwCodeSize)
					{
						// search for first short jump back
						for(DWORD dwCodeIndex3 = dwCall1Offset; dwCodeIndex3 - dwCall1Offset < 200; dwCodeIndex3++)
						{
							if(pbCode[dwCodeIndex3] == 0x75 && pbCode[dwCodeIndex3+1] & 0x80)
							{
								// search for second call back
								for(DWORD dwCodeIndex4 = dwCodeIndex3 - 5; dwCodeIndex4 > dwCall1Offset; dwCodeIndex4--)
								{
									if(pbCode[dwCodeIndex4] == 0xE8)
									{
										for(DWORD dwCodeIndex5 = dwCodeIndex4 - 5; dwCodeIndex5 > dwCall1Offset; dwCodeIndex5--)
										{
											if(pbCode[dwCodeIndex5] == 0xE8)
											{
												// compute offset of call
												DWORD dwCall2Offset = (dwCodeIndex5 + 5) + *((PDWORD)&pbCode[dwCodeIndex5 + 1]);
												if(dwCall2Offset < dwCodeSize)
												{
													// search for jump switch
													for(DWORD dwCodeIndex6 = dwCall2Offset; dwCodeIndex6 - dwCall2Offset < 100; dwCodeIndex6++)
													{
														if(*((PWORD)&pbCode[dwCodeIndex6]) == 0x24FF)
														{
															PDWORD pJumpTable = (PDWORD)RvaToPointer(pbFile, *((PDWORD)&pbCode[dwCodeIndex6+3]) - pNtHdr->OptionalHeader.ImageBase);
															if(pJumpTable)
															{
																// follow the switch
																PBYTE pCaseCode = RvaToPointer(pbFile, pJumpTable[3] - pNtHdr->OptionalHeader.ImageBase);
																if(pCaseCode)
																{
																	DWORD dwCaseOffset = DWORD(pCaseCode - pbCode);
																	
																	// search for first call
																	for(DWORD dwCodeIndex7 = dwCaseOffset; dwCodeIndex7 - dwCaseOffset < 20; dwCodeIndex7++)
																	{
																		if(pbCode[dwCodeIndex7] == 0xE8)
																		{
																			DWORD dwCall3Offset = (dwCodeIndex7 + 5) + *((PDWORD)&pbCode[dwCodeIndex7 + 1]);
																			if(dwCall3Offset < dwCodeSize)
																			{
																				for(DWORD dwCodeIndex8 = dwCall3Offset; dwCodeIndex8 - dwCall3Offset < 60; dwCodeIndex8++)
																				{
																					if(pbCode[dwCodeIndex8] == 0x89)
																					{
																						GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex8 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
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

VOID scanLockSpeed_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanLockSpeed_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}