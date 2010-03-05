#include "scan_teleporttoplane.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "TeleportToPlane"
#define SCAN_REQUIRED FALSE

VOID scanTeleportToPlane_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize - 8; dwCodeIndex++)
	{
		if(pbCode[dwCodeIndex] == 0xC7 && (pbCode[dwCodeIndex+1] & 0xF8) == 0x40)
		{
			if(pbCode[dwCodeIndex+1] == 0x44) // esp based
			{
				if(*((PDWORD)&pbCode[dwCodeIndex+4]) == 0x3f800000 && *((PDWORD)&pbCode[dwCodeIndex+44]) == 0x3f800000 && *((PDWORD)&pbCode[dwCodeIndex+84]) == 0x3f800000)
				{
					for(DWORD dwCodeIndex2 = dwCodeIndex - 3; dwCodeIndex2; dwCodeIndex2--)
					{
						if(pbCode[dwCodeIndex2] == 0x8B && (pbCode[dwCodeIndex2+1] & 0x80) == 0)
							break;
					}
					if(((pbCode[dwCodeIndex2+1] >> 3) & 0x7) == (pbCode[dwCodeIndex2+1] & 0x7))
					{
						GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex2 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
						SetItemStatus(iItem, hwndDlg, "Success");
						return;
					}
				}
			}
			else
			{
				if(*((PDWORD)&pbCode[dwCodeIndex+3]) == 0x3f800000 && *((PDWORD)&pbCode[dwCodeIndex+38]) == 0x3f800000 && *((PDWORD)&pbCode[dwCodeIndex+73]) == 0x3f800000)
				{
					for(DWORD dwCodeIndex2 = dwCodeIndex - 3; dwCodeIndex2; dwCodeIndex2--)
					{
						if(pbCode[dwCodeIndex2] == 0x8B && (pbCode[dwCodeIndex2+1] & 0x80) == 0)
							break;
					}
					if(((pbCode[dwCodeIndex2+1] >> 3) & 0x7) == (pbCode[dwCodeIndex2+1] & 0x7))
					{
						GetPPD(iItem, hwndDlg)->dwValue = OffsetToRva(pbFile, PtrToUlong(pbCode) + dwCodeIndex2 - PtrToUlong(pbFile)) + pNtHdr->OptionalHeader.ImageBase;
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

VOID scanTeleportToPlane_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanTeleportToPlane_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}