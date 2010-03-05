#include "scan_cameraangle.h"
#include "scan_common.h"
#include <commctrl.h>

#define SCAN_CATEGORY "BWH"
#define SCAN_NAME     "CameraAngle"
#define SCAN_REQUIRED FALSE

VOID scanCameraAngle_ScanProc(INT iItem, HWND hwndDlg, PBYTE pbFile)
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

	for(DWORD dwCodeIndex = 0; dwCodeIndex < dwCodeSize; dwCodeIndex++)
    {
        if(*((PDWORD)&pbCode[dwCodeIndex]) == 0xD8DDD8DD)
        {
            if(*((PWORD)&pbCode[dwCodeIndex+4]) == 0x05D9)
            {
                if(*((PWORD)&pbCode[dwCodeIndex+10]) == 0x05D9)
                {
                    if(*((PWORD)&pbCode[dwCodeIndex+16]) == 0xF3D9)
                    {
                        GetPPD(iItem, hwndDlg)->dwValue = *((PDWORD)&pbCode[dwCodeIndex+6]);
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

VOID scanCameraAngle_Init(HWND hwndList)
{
	PPOINTER_DATA ppd;
	LVITEM lvi;
	
	ppd = (PPOINTER_DATA)HeapAlloc(GetProcessHeap(), 0, sizeof(POINTER_DATA));
	ppd->pszCategory = SCAN_CATEGORY;
	ppd->pszName = SCAN_NAME;
	ppd->bRequired = SCAN_REQUIRED;
	ppd->dwValue = 0;
	ppd->pfnScanProc = scanCameraAngle_ScanProc;

	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.pszText = SCAN_NAME;
	lvi.iSubItem = 0;
	lvi.iItem = ListView_GetItemCount(hwndList);
	lvi.lParam = (LPARAM)ppd;
	ListView_InsertItem(hwndList, &lvi);
}