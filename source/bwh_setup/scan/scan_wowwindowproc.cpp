#include "scan_wowwindowproc.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "WOW"
#define SCAN_NAME     "WowWindowProc"
#define SCAN_REQUIRED TRUE

VOID scanWowWindowProc_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	// scan data section for "GxWindowClassD3d"
	for(DWORD dwRDataIndex = 0; dwRDataIndex < dwRDataSize - (sizeof("GxWindowClassD3d") - 1); dwRDataIndex++)
	{
		if(MemoryCompare(&pbRData[dwRDataIndex], (PBYTE)"GxWindowClassD3d", sizeof("GxWindowClassD3d") - 1))
		{
			// calculate va of found string
			DWORD dwStringVA = OffsetToRva(pbFile, PtrToUlong(pbRData) + dwRDataIndex - PtrToUlong(pbFile));
			if(dwStringVA)
			{
				dwStringVA += pNtHdr->OptionalHeader.ImageBase;
				
				// scan code section for 'mov dword ptr[reg+disp], dwStringVA'
				for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 8; dwCodeIndex++)
				{
					if(pbCode[dwCodeIndex] == 0xC7)
					{
						if(pbCode[dwCodeIndex + 1] >= 0x40 && pbCode[dwCodeIndex + 1] <= 0x47)
						{
							BYTE bWndProcMove[4];
							if(pbCode[dwCodeIndex + 1] == 0x44) // esp based
							{
								if(*((PDWORD)&pbCode[dwCodeIndex + 4]) == dwStringVA)
								{
									// found 'mov dword ptr[esp+disp], dwStringVA'
									bWndProcMove[0] = 0xC7;
									bWndProcMove[1] = 0x44;
									bWndProcMove[2] = 0x24;
									bWndProcMove[3] = pbCode[dwCodeIndex + 3] - 0x20;

									// now search for the window proc move instruction backwards
									for(DWORD dwCodeIndex2 = dwCodeIndex - 8; dwCodeIndex - dwCodeIndex2 < 50; dwCodeIndex2--)
									{
										if(MemoryCompare(&pbCode[dwCodeIndex2], bWndProcMove, sizeof(bWndProcMove)))
										{
											GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex2 + 4]) - pNtHdr->OptionalHeader.ImageBase;
											SetItemStatus(iItem, hwndDlg, "Success");
											return;
										}
									}

									// search forward also
									for(DWORD dwCodeIndex2 = dwCodeIndex + 8; dwCodeIndex2 - dwCodeIndex < 50; dwCodeIndex2++)
									{
										if(MemoryCompare(&pbCode[dwCodeIndex2], bWndProcMove, sizeof(bWndProcMove)))
										{
											GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex2 + 4]) - pNtHdr->OptionalHeader.ImageBase;
											SetItemStatus(iItem, hwndDlg, "Success");
											return;
										}
									}
								}
							}
							else // some other register based
							{
								if(*((PDWORD)&pbCode[dwCodeIndex + 3]) == dwStringVA)
								{
									// found 'mov dword ptr[reg+disp], dwStringVA'
									bWndProcMove[0] = 0xC7;
									bWndProcMove[1] = pbCode[dwCodeIndex + 1];
									bWndProcMove[2] = pbCode[dwCodeIndex + 2] - 0x20;

									// now search for the window proc move instruction backwards
									for(DWORD dwCodeIndex2 = dwCodeIndex - 7; dwCodeIndex - dwCodeIndex2 < 50; dwCodeIndex2--)
									{
										if(MemoryCompare(&pbCode[dwCodeIndex2], bWndProcMove, sizeof(bWndProcMove) - 1))
										{
											GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex2 + 3]) - pNtHdr->OptionalHeader.ImageBase;
											SetItemStatus(iItem, hwndDlg, "Success");
											return;
										}
									}

									// search forward also
									for(DWORD dwCodeIndex2 = dwCodeIndex + 7; dwCodeIndex2 - dwCodeIndex < 50; dwCodeIndex2++)
									{
										if(MemoryCompare(&pbCode[dwCodeIndex2], bWndProcMove, sizeof(bWndProcMove) - 1))
										{
											GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex2 + 3]) - pNtHdr->OptionalHeader.ImageBase;
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

VOID scanWowWindowProc_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanWowWindowProc_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}