#include "scan_trackingnpcdataoffset.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "TrackingNpcDataOffset"
#define SCAN_REQUIRED FALSE

VOID scanTrackingNpcDataOffset_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// this the code we need to locate; lots of fun
	// mov reg1, 1
	// ...
	// shl reg1, cl
	// ...
	// test reg2, reg1
	// ...
	// setne al
	// ...
	// retn 4
	for(DWORD dwCodeIndex = 100; dwCodeIndex < dwCodeSize; dwCodeIndex++)
	{
		// find the 'retn 4'
		if(pbCode[dwCodeIndex] == 0xC2 && pbCode[dwCodeIndex + 1] == 0x04 && pbCode[dwCodeIndex + 2] == 0x00)
		{
			for(DWORD dwCodeIndex2 = dwCodeIndex - 3; dwCodeIndex - dwCodeIndex2 < 20; dwCodeIndex2--)
			{
				// find the 'setne al'
				if(pbCode[dwCodeIndex2] == 0x0F && pbCode[dwCodeIndex2 + 1] == 0x95 && pbCode[dwCodeIndex2 + 2] == 0xC0)
				{
					for(DWORD dwCodeIndex3 = dwCodeIndex2 - 2; dwCodeIndex2 - dwCodeIndex3 < 20; dwCodeIndex3--)
					{
						// find the 'test reg2, reg1'
						if(pbCode[dwCodeIndex3] == 0x85 && (pbCode[dwCodeIndex3 + 1] & 0xC0) == 0xC0)
						{
							for(DWORD dwCodeIndex4 = dwCodeIndex3 - 2; dwCodeIndex3 - dwCodeIndex4 < 20; dwCodeIndex4--)
							{
								// find the 'shl reg1, cl'
								if(pbCode[dwCodeIndex4] == 0xD3 && (pbCode[dwCodeIndex4 + 1] & 0xF8) == 0xE0 && ((pbCode[dwCodeIndex4 + 1] & 0x7) == (pbCode[dwCodeIndex3 + 1] & 0x7) || (pbCode[dwCodeIndex4 + 1] & 0x7) == ((pbCode[dwCodeIndex3 + 1] >> 3) & 0x7)))
								{
									for(DWORD dwCodeIndex5 = dwCodeIndex4 - 5; dwCodeIndex4 - dwCodeIndex5 < 20; dwCodeIndex5--)
									{
										// find the 'mov reg1, 1'
										if(pbCode[dwCodeIndex5] == (0xB8 | pbCode[dwCodeIndex4 + 1] & 0x7) && *((PDWORD)&pbCode[dwCodeIndex5 + 1]) == 1)
										{
											// ok, we found that code... now we need to locate the call that immediately follows a return
											for(DWORD dwCodeIndex6 = dwCodeIndex5 - 8; dwCodeIndex5 - dwCodeIndex6 < 100; dwCodeIndex6--)
											{
												if(pbCode[dwCodeIndex6] == 0xE8 && pbCode[dwCodeIndex6 - 1] == 0x00 && pbCode[dwCodeIndex6 - 2] == 0x04 && pbCode[dwCodeIndex6 - 3] == 0xC2)
												{
													// decode the call; change to pointer
													PBYTE pCallHook = RvaToPointer(pbFile, (OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex6 + 5 - PtrToUlong(pbFile))) + *((PDWORD)&pbCode[dwCodeIndex6 + 1]));
													
													// some more important instructions we need to verify exist
													for(DWORD dwCodeIndex7 = dwCodeIndex5 - 6; dwCodeIndex7 >= dwCodeIndex6 + 5 + 6; dwCodeIndex7--)
													{
														// mov reg, [reg+pTrackData]
														if(pbCode[dwCodeIndex7] == 0x8B && (pbCode[dwCodeIndex7+1] & 0xC0) == 0x80)
														{
															for(DWORD dwCodeIndex8 = dwCodeIndex7 - 6; dwCodeIndex8 >= dwCodeIndex6 + 5; dwCodeIndex8--)
															{
																// mov reg, [reg+pTrackStruct]
																if(pbCode[dwCodeIndex8] == 0x8B && (pbCode[dwCodeIndex8+1] & 0xC0) == 0x80)
																{
																	// few things we need to look for inside the hooked call
																	for(DWORD dwCallIndex = 0; dwCallIndex < 100; dwCallIndex++)
																	{
																		// mov reg, [reg+pCharData2]
																		if(pCallHook[dwCallIndex] == 0x8b && (pCallHook[dwCallIndex + 1] & 0xC0) == 0x80)
																			break;
																	}
																	if(dwCallIndex != 100)
																	{
																		// now were looking for a jz that jumps just after a ret
																		for(DWORD dwCallIndex2 = dwCallIndex + 6; dwCallIndex2 < 100; dwCallIndex2++)
																		{
																			if(pCallHook[dwCallIndex2] == 0x74 && pCallHook[ pCallHook[dwCallIndex2 + 1] + dwCallIndex2 + 1] == 0xC3)
																			{
																				for(DWORD dwCallIndex3 = dwCallIndex2 - 6; dwCallIndex3 >= dwCallIndex + 6; dwCallIndex3--)
																				{
																					// mov reg, [reg+npcdata]
																					if(pCallHook[dwCallIndex3] == 0x8b && (pCallHook[dwCallIndex3 + 1] & 0xC0) == 0x80)
																						break;
																				}
																				if(dwCallIndex3 >= dwCallIndex + 6)
																				{
																					for(DWORD dwCallIndex4 = pCallHook[dwCallIndex2 + 1] + dwCallIndex2 + 2; dwCallIndex4 < 100; dwCallIndex4++)
																					{
																						if(*((PWORD)&pCallHook[dwCallIndex4]) == 0xB60F)
																							break;
																					}
																					if(dwCallIndex4 < 100)
																					{
																						// everything checks out if we get this far

																						// return track struct offset
																						GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pCallHook[dwCallIndex3+2]);
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

VOID scanTrackingNpcDataOffset_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanTrackingNpcDataOffset_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}